#include <interpreter.h>

#define interpreter_runtime_error(line, fmt, ...) \
    CKG_LOG_ERROR("Error Line: %d | " fmt, line, ##__VA_ARGS__); \
    ckg_assert(false)

internal Scope scope_create(Scope* parent_scope) {
    Scope scope = {0};
    scope.parent_scope = parent_scope;
    return scope;
}

internal InterpreterReturn scope_try_get(Scope* scope, CKG_StringView identifier) {
    for (int i = 0; scope->variables && i < ckg_vector_count(scope->variables); i++) {
        ScopeVar variable = scope->variables[i];
        if (ckg_str_equal(variable.name.data, variable.name.length, identifier.data, identifier.length)) {
            return variable.ret;
        }
    }
    return INVALID_INTERPRETER_RETURN();
}

internal InterpreterReturn scope_load(Scope* scope, CKG_StringView identifier) {
    InterpreterReturn ret = scope_try_get(scope, identifier);
    Scope* current_scope = scope->parent_scope;

    while (current_scope && ret.type == INTERPRETER_RETURN_INVALID) {
        ret = scope_try_get(current_scope, identifier);
        current_scope = current_scope->parent_scope;
    }

    return ret;
}

internal void scope_assign(Scope* scope, CKG_StringView identifier, InterpreterReturn value) {
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

InterpreterReturn interpret_expression(Expression* expression, Scope* scope) {
    InterpreterReturn ret = INVALID_INTERPRETER_RETURN();

    if (expression->type == EXPRESSION_TYPE_INTEGER) {
        ret.type = INTERPRETER_NUMBER;
        ret.i = expression->integer->value;
    } else if (expression->type == EXPRESSION_TYPE_FLOAT) {
        ret.type = INTERPRETER_NUMBER;
        ret.f = expression->floating->value;
    } else if (expression->type == EXPRESSION_TYPE_STRING) {
        ret.type = INTERPRETER_STRING;
        ret.str = expression->str->name;
    } else if (expression->type == EXPRESSION_TYPE_BOOLEAN) {
        ret.type = INTERPRETER_BOOL;
        ret.b = expression->boolean->value;
    } else if (expression->type == EXPRESSION_TYPE_IDENTIFER) {
        ret = scope_load(scope, expression->identifier->name);
        if (ret.type == INTERPRETER_RETURN_INVALID) {
            interpreter_runtime_error(expression->identifier->line, "Undeclared identifier '%.*s'\n", expression->identifier->name.length, expression->identifier->name.data);
        }

        return ret;
    } else if (expression->type == EXPRESSION_TYPE_BINARY_OPERATION) {
        InterpreterReturn left = interpret_expression(expression->binary->left, scope);
        InterpreterReturn right = interpret_expression(expression->binary->right, scope);

        SPL_TokenType op = expression->binary->operation.type;

        if (op == SPL_TOKEN_PLUS) {
            if (left.type == INTERPRETER_NUMBER && right.type == INTERPRETER_NUMBER) {
                ret.type = INTERPRETER_NUMBER;
                ret.i = left.i + right.i;
            }
        }
    } else if (expression->type == EXPRESSION_TYPE_LOGICAL_OPERATION) {
        InterpreterReturn left = interpret_expression(expression->logical->left, scope);
        InterpreterReturn right = interpret_expression(expression->logical->right, scope);

        SPL_TokenType op = expression->logical->operation.type;

        if (op == SPL_TOKEN_AND) {
            if (left.type == INTERPRETER_BOOL && right.type == INTERPRETER_BOOL) {
                ret.type = INTERPRETER_BOOL;
                ret.b = left.b && right.b;
            }
        } else if (op == SPL_TOKEN_OR) {
            if (left.type == INTERPRETER_BOOL && right.type == INTERPRETER_BOOL) {
                ret.type = INTERPRETER_BOOL;
                ret.b = left.b || right.b;
            }
        }
    }

    return ret;
}

void interpret_statement(Statement* statement, Scope* scope) {
    if (statement->type == STATEMENT_TYPE_PRINT) {
        InterpreterReturn value = interpret_expression(statement->print_statement->value, scope);
        if (value.type == INTERPRETER_NUMBER) {
            printf("%d\n", value.i);
        } else if (value.type == INTERPRETER_STRING) {
            printf("%.*s\n", (int)value.str.length, value.str.data);
        } else if (value.type == INTERPRETER_BOOL) {
            printf(value.b ? "true\n" : "false\n");
        }
    } else if (statement->type == STATEMENT_TYPE_ASSIGNMENT) {
        InterpreterReturn right_ret = interpret_expression(statement->assignment_statement->right, scope);
        scope_assign(scope, statement->assignment_statement->left->identifier->name, right_ret);
    } else if (statement->type == STATEMENT_TYPE_IF) {
        // InterpreterReturn condition = interpret_expression(statement->if_statement->value, scope);
        // if (condition.type != INTERPRETER_BOOL) {
        //     interpreter_runtime_error(statement->if_statement->line, "Condition must be boolean");
        // }

        // if (condition.b) {
        //     interpret_statement(statement->if_statement->if_code_block, scope);
        // } else if (statement->if_statement->else_code_block) {
        //     interpret_statement(statement->if_statement->else_code_block, scope);
        // }
    } else if (statement->type == STATEMENT_TYPE_WHILE) {
        // while (true) {
        //     InterpreterReturn condition = interpret_expression(statement->while_statement->condition, scope);
        //     if (condition.type != INTERPRETER_BOOL) {
        //         interpreter_runtime_error(statement->line, "Condition must be boolean");
        //     }
        //     if (!condition.b) break;
        //     interpret_statement(statement->while_statement->body, scope);
        // }
    } else if (statement->type == STATEMENT_TYPE_FOR) {
        //Scope for_scope = scope_create(scope);

        //interpret_statement(statement->for_statement->initializer, &for_scope);

        //while (true) {
        //    InterpreterReturn condition = interpret_expression(statement->for_statement->condition, &for_scope);
        //    if (condition.type != INTERPRETER_BOOL) {
        //        interpreter_runtime_error(statement->line, "Condition must be boolean");
        //    }
        //    if (!condition.b) break;

        //    interpret_statement(statement->for_statement->body, &for_scope);
        //    interpret_statement(statement->for_statement->increment, &for_scope);
        //}
    }
}

void interpret_program(Program* program, Scope* scope) {
    for (int i = 0; program->statements && i < ckg_vector_count(program->statements); i++) {
        interpret_statement(program->statements[i], scope);
    }
}

void interpret_ast(ASTNode* ast) {
    Scope global_scope = scope_create(NULLPTR);
    interpret_program(ast->program, &global_scope);
}