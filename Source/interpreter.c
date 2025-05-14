#include <interpreter.h>

#define interpreter_runtime_error(line, fmt, ...) \
    CKG_LOG_ERROR("Error Line: %d | " fmt, line, ##__VA_ARGS__); \
    ckg_assert(false)

internal Scope scope_create(Scope* parent_scope) {
    Scope scope = {0};
    scope.parent_scope = parent_scope;
    return scope;
}

// Date: May 13, 2025
// NOTE(Jovanni): REALISTICALLY THIS SHOULD BE A HASHTABLE...
internal InterpreterReturn scope_try_get(Scope* scope, CKG_StringView identifier) {
    for (int i = 0; scope->variables && i < ckg_vector_count(scope->variables); i++) {
        ScopeVar variable = scope->variables[i];
        if (ckg_str_equal(variable.name.data, variable.name.length, identifier.data, identifier.length)) {
            return variable.ret;
        }
    }

    return INVALID_INTERPRETER_RETURN();
}

// Date: May 13, 2025
// NOTE(Jovanni): REALISTICALLY THIS SHOULD BE A HASHTABLE...
internal InterpreterReturn scope_load(Scope* scope, CKG_StringView identifier) {
    InterpreterReturn ret = scope_try_get(scope, identifier);
    if (ret.type == INTERPRETER_RETURN_INVALID) {
        Scope* current_scope = scope->parent_scope;
        while (current_scope && ret.type == INTERPRETER_RETURN_INVALID) {
            ret = scope_try_get(current_scope, identifier);
            current_scope = current_scope->parent_scope;
        }
    }

    return ret;
}

internal void scope_store(Scope* scope, CKG_StringView identifier, InterpreterReturn value) {
    Scope* current_scope = scope->parent_scope;
    for (int i = 0; current_scope && current_scope->variables && i < ckg_vector_count(current_scope->variables); i++) {
        ScopeVar* variable = &current_scope->variables[i];
        if (ckg_str_equal(variable->name.data, variable->name.length, identifier.data, identifier.length)) {
            variable->ret = value;
            return;
        }
    }

    for (int i = 0; scope->variables && i < ckg_vector_count(scope->variables); i++) {
        ScopeVar* variable = &scope->variables[i];
        if (ckg_str_equal(variable->name.data, variable->name.length, identifier.data, identifier.length)) {
            variable->ret = value;
            return;
        }
    }

    ScopeVar new_var;
    new_var.name = identifier;
    new_var.ret = value;
    ckg_vector_push(scope->variables, new_var);
}

InterpreterType interpreter_promote_type(InterpreterType a, InterpreterType b) {
    if (a == INTERPRETER_STRING || b == INTERPRETER_STRING) {
        return INTERPRETER_STRING;
    }

    if ((a == INTERPRETER_FLOAT && b == INTERPRETER_INTEGER) ||
        (a == INTERPRETER_INTEGER && b == INTERPRETER_FLOAT) ||
        (a == INTERPRETER_FLOAT && b == INTERPRETER_FLOAT)) {
        return INTERPRETER_FLOAT;
    }


    if ((a == INTERPRETER_FLOAT && b == INTERPRETER_INTEGER) ||
        (a == INTERPRETER_INTEGER && b == INTERPRETER_FLOAT) ||
        (a == INTERPRETER_FLOAT && b == INTERPRETER_FLOAT)) {
        return INTERPRETER_FLOAT;
    }

    if (a == INTERPRETER_INTEGER && b == INTERPRETER_INTEGER) {
        return INTERPRETER_INTEGER;
    }

    return -1; // invalid promotion
}

InterpreterReturn interpreter_to_string(InterpreterReturn v) {
    InterpreterReturn result = {0};
    result.type = INTERPRETER_STRING;

    u64 str_length = 0;
    if (v.type == INTERPRETER_STRING) {
        if (v.str.data[0] == '"' && v.str.data[v.str.length - 1] == '"') {
            result.str = ckg_sv_create(v.str.data + 1, v.str.length - 2);
        } else {
            result.str = v.str;
        }
    } else if (v.type == INTERPRETER_INTEGER) {
        char* str = ckg_str_sprint(&str_length, "%d", v.i);
        result.str = ckg_sv_create(str, str_length);
    } else if (v.type == INTERPRETER_FLOAT) {
        char* str = ckg_str_sprint(&str_length, "%f", v.f);
        result.str = ckg_sv_create(str, str_length);
    } else {
        interpreter_runtime_error(v.line, "Cannot convert type '%s' to string",
            interpreter_type_strings[v.type]);
    }

    return result;
}

InterpreterReturn interpreter_concat(InterpreterReturn left, InterpreterReturn right) {
    InterpreterReturn result = {0};
    result.type = INTERPRETER_STRING;

    left = interpreter_to_string(left);
    right = interpreter_to_string(right);

    u64 str_length = 0;
    char* str = ckg_str_sprint(&str_length, "%.*s%.*s", left.str.length, left.str.data, right.str.length, right.str.data);
    result.str = ckg_sv_create(str, str_length);

    return result;
}

double interpreter_as_float(InterpreterReturn v) {
    return v.type == INTERPRETER_FLOAT ? v.f : (double)v.i;
}

int interpreter_as_int(InterpreterReturn v) {
    return v.i;
}

InterpreterReturn interpret_expression(Expression* expression, Scope* scope) {
    InterpreterReturn ret = INVALID_INTERPRETER_RETURN();

    if (expression->type == EXPRESSION_TYPE_INTEGER) {
        ret.type = INTERPRETER_INTEGER;
        ret.line = expression->integer->line;
        ret.i = expression->integer->value;
    } else if (expression->type == EXPRESSION_TYPE_FLOAT) {
        ret.type = INTERPRETER_FLOAT;
        ret.line = expression->floating->line;
        ret.f = expression->floating->value;
    } else if (expression->type == EXPRESSION_TYPE_STRING) {
        ret.type = INTERPRETER_STRING;
        ret.line = expression->str->line;
        ret.str = expression->str->name;
    } else if (expression->type == EXPRESSION_TYPE_BOOLEAN) {
        ret.type = INTERPRETER_BOOL;
        ret.line = expression->boolean->line;
        ret.b = expression->boolean->value;
    } else if (expression->type == EXPRESSION_TYPE_IDENTIFER) {
        ret = scope_load(scope, expression->identifier->name);
        ret.line = expression->identifier->line;
        if (ret.type == INTERPRETER_RETURN_INVALID) {
            interpreter_runtime_error(expression->identifier->line, "Undeclared identifier '%.*s'\n", expression->identifier->name.length, expression->identifier->name.data);
        }

        return ret;
    } else if (expression->type == EXPRESSION_TYPE_GROUPING) {
        return interpret_expression(expression->grouping->value, scope);
    } else if (expression->type == EXPRESSION_TYPE_BINARY_OPERATION) {
        InterpreterReturn left = interpret_expression(expression->binary->left, scope);
        InterpreterReturn right = interpret_expression(expression->binary->right, scope);

        SPL_TokenType op = expression->binary->operation.type;

        ret.line = expression->binary->line;
        if (op == SPL_TOKEN_PLUS) {
           InterpreterType promoted = interpreter_promote_type(left.type, right.type);

            if (promoted == INTERPRETER_FLOAT) {
                ret.type = INTERPRETER_FLOAT;
                ret.f = interpreter_as_float(left) + interpreter_as_float(right);
            } else if (promoted == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_INTEGER;
                ret.i = interpreter_as_int(left) + interpreter_as_int(right);
            } else if (promoted == INTERPRETER_STRING) {
                ret = interpreter_concat(left, right);
            } else {
                interpreter_runtime_error(expression->binary->line,
                    "Unsupported promotion for '+' between '%s' and '%s'",
                    interpreter_type_strings[left.type],
                    interpreter_type_strings[right.type]);
            }
        } else if (op == SPL_TOKEN_MINUS) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_INTEGER;
                ret.i = left.i - right.i;
            }
        } else if (op == SPL_TOKEN_STAR) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_INTEGER;
                ret.i = left.i * right.i;
            }
        } else if (op == SPL_TOKEN_DIVISION) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_INTEGER;
                ret.i = left.i / right.i;
            }
        } else if (op == SPL_TOKEN_MODULUS) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_INTEGER;
                ret.i = left.i % right.i;
            }
        } else if (op == SPL_TOKEN_AMPERSAND) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_INTEGER;
                ret.i = left.i & right.i;
            }
        } else if (op == SPL_TOKEN_BITWISE_OR) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_INTEGER;
                ret.i = left.i & right.i;
            }
        } else if (op == SPL_TOKEN_GREATER_THAN) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_BOOL;
                ret.b = left.i > right.i;
            }
        } else if (op == SPL_TOKEN_GREATER_THAN_EQUALS) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_BOOL;
                ret.b = left.i >= right.i;
            }
        } else if (op == SPL_TOKEN_LESS_THAN) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_BOOL;
                ret.b = left.i < right.i;
            }
        } else if (op == SPL_TOKEN_LESS_THAN) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_BOOL;
                ret.b = left.i <= right.i;
            }
        } else if (op == SPL_TOKEN_EQUALS) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_BOOL;
                ret.b = left.i == right.i;
            }
        } else if (op == SPL_TOKEN_EQUALS) {
            if (left.type == INTERPRETER_INTEGER && right.type == INTERPRETER_INTEGER) {
                ret.type = INTERPRETER_BOOL;
                ret.b = left.i != right.i;
            }
        }

        if (ret.type == INTERPRETER_RETURN_INVALID) {
            interpreter_runtime_error(expression->binary->line, "Binary: Something went wrong?\n");
        }
    } else if (expression->type == EXPRESSION_TYPE_LOGICAL_OPERATION) {
        InterpreterReturn left = interpret_expression(expression->logical->left, scope);


        SPL_TokenType op = expression->logical->operation.type;

        ret.line = expression->logical->line;
        if (op == SPL_TOKEN_AND) {
            if (!left.b) {
                ret.type = INTERPRETER_BOOL;
                ret.b = false;

                return ret;
            }

            InterpreterReturn right = interpret_expression(expression->logical->right, scope);
            if (!(left.type == INTERPRETER_BOOL && right.type == INTERPRETER_BOOL)) {
                interpreter_runtime_error(expression->logical->operation.line, "Unsupported operator %.*s between %s and %s.\n", 
                    expression->logical->operation.name.length, expression->logical->operation.name.data, 
                    interpreter_type_strings[left.type], interpreter_type_strings[right.type]);
            }

            ret.type = INTERPRETER_BOOL;
            ret.b = left.b && right.b;
        } else if (op == SPL_TOKEN_OR) {
            if (left.b) {
                ret.type = INTERPRETER_BOOL;
                ret.b = true;

                return ret;
            }

            InterpreterReturn right = interpret_expression(expression->logical->right, scope);
            if (!(left.type == INTERPRETER_BOOL && right.type == INTERPRETER_BOOL)) {
                interpreter_runtime_error(expression->logical->operation.line, "Unsupported operator %.*s between %s and %s.\n", 
                    expression->logical->operation.name.length, expression->logical->operation.name.data, 
                    interpreter_type_strings[left.type], interpreter_type_strings[right.type]);
            }

            ret.type = INTERPRETER_BOOL;
            ret.b = left.b || right.b;
        }

        if (ret.type == INTERPRETER_RETURN_INVALID) {
            interpreter_runtime_error(expression->logical->line, "Logical: Something went wrong?\n");
        }
    }

    return ret;
}

void interpret_statements(Statement** statements, Scope* scope);

void interpret_statement(Statement* statement, Scope* scope) {
    if (statement->type == STATEMENT_TYPE_PRINT) {
        InterpreterReturn value = interpret_expression(statement->print_statement->value, scope);
        if (value.type == INTERPRETER_INTEGER) {
            printf("%d\n", value.i);
        } else if (value.type == INTERPRETER_STRING) {
            value = interpreter_to_string(value);
            printf("\"%.*s\"\n", (int)value.str.length, value.str.data);
        } else if (value.type == INTERPRETER_BOOL) {
            printf(value.b ? "true\n" : "false\n");
        }
    } else if (statement->type == STATEMENT_TYPE_ASSIGNMENT) {
        InterpreterReturn right_ret = interpret_expression(statement->assignment_statement->right, scope);
        scope_store(scope, statement->assignment_statement->left->identifier->name, right_ret);
    } else if (statement->type == STATEMENT_TYPE_IF) {
        InterpreterReturn condition = interpret_expression(statement->if_statement->value, scope);
        if (condition.type != INTERPRETER_BOOL) {
            interpreter_runtime_error(statement->if_statement->line, "Condition must be boolean");
        }

        if (condition.b) {
            Scope if_scope = scope_create(scope);
            interpret_statements(statement->if_statement->if_code_block, &if_scope);
        } else if (statement->if_statement->else_code_block) {
            Scope else_scope = scope_create(scope);
            interpret_statements(statement->if_statement->else_code_block, &else_scope);
        }
    } else if (statement->type == STATEMENT_TYPE_WHILE) {
        while (true) {
            InterpreterReturn condition = interpret_expression(statement->while_statement->value, scope);
            if (condition.type != INTERPRETER_BOOL) {
                interpreter_runtime_error(statement->while_statement->line, "Condition must be boolean");
            }

            if (!condition.b) {
                break;
            }

            Scope while_scope = scope_create(scope);
            interpret_statements(statement->while_statement->while_code_block, &while_scope);
        }
    } else if (statement->type == STATEMENT_TYPE_FOR) {
        Scope for_init_scope = scope_create(scope);
        interpret_statement(statement->for_statement->initalization, &for_init_scope);

        while (true) {
            InterpreterReturn condition = interpret_expression(statement->for_statement->condition, &for_init_scope);
            if (condition.type != INTERPRETER_BOOL) {
                interpreter_runtime_error(statement->for_statement->line, "Condition must be boolean");
            }

            if (!condition.b) {
                break;
            }

            Scope for_inside_scope = scope_create(&for_init_scope);
            interpret_statements(statement->for_statement->for_code_block, &for_inside_scope);
            interpret_statement(statement->for_statement->increment, &for_init_scope);
        }
    }
}

void interpret_statements(Statement** statements, Scope* scope) {
    for (int i = 0; statements && i < ckg_vector_count(statements); i++) {
        Statement* s = statements[i];
        interpret_statement(s, scope);
    }
}

void interpret_program(Program* program, Scope* scope) {
    interpret_statements(program->statements, scope);
}

void interpret_ast(ASTNode* ast) {
    Scope global_scope = scope_create(NULLPTR);
    interpret_program(ast->program, &global_scope);
}