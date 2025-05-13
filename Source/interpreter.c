#include <interpreter.h>

internal Scope scope_create(Scope* parent_scope) {
    Scope scope = {0};
    scope.parent_scope = parent_scope;

    return scope;
}

#define interpreter_runtime_error(line, fmt, ...) \
CKG_LOG_ERROR("String: %s\n", token_strings[parser_peek_nth_token(parser, 0).type]);               \
CKG_LOG_ERROR("Error Line: %d | %s", line, fmt, ##__VA_ARGS__); \
ckg_assert(false)  

internal InterpreterReturn scope_try_get(Scope* scope, char* variable_name, u64 str_length) {
    for (int i = 0; scope->variables && i < ckg_vector_count(scope->variables); i++) {
        ScopeVar varaible = scope->variables[i];
        if (ckg_str_equal(varaible.name.data, varaible.name.length, variable_name, str_length)) {
            return varaible.ret;
        }
    }

    return INVALID_INTERPRETER_RETURN();
}

internal InterpreterReturn scope_load(Scope* scope, char* variable_name, u64 str_length) {
    InterpreterReturn ret = INVALID_INTERPRETER_RETURN();
    if (scope_try_get(scope, variable_name, str_length).type == INTERPRETER_RETURN_INVALID) {
        Scope* current_scope = scope->parent_scope;
        while (current_scope && ret.type == INTERPRETER_RETURN_INVALID) {
            ret = scope_try_get(current_scope, variable_name, str_length);
            current_scope = current_scope->parent_scope;
        }
    }

    return ret;
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
    } else if (expression->type == EXPRESSION_TYPE_LOGICAL_OPERATION) {
        InterpreterReturn left_ret = interpret_expression(expression->logical->left, scope);
        if (left_ret.type != INTERPRETER_BOOL) {
            interpreter_runtime_error("Unsupported operator %.*s between %s and %s.", expression->logical->operation.line, expression->logical->operation.name.length, expression->logical->operation.name.data, interpreter_type_strings[left_ret.type], interpreter_type_strings[right_ret.type]);
        }

        if (expression->logical->operation.type == SPL_TOKEN_AND) {
            if (left_ret.type == INTERPRETER_NUMBER && right_ret.type == INTERPRETER_NUMBER) {
                ret.type = INTERPRETER_NUMBER;
                ret.i = left_ret.i + right_ret.i;
            }
        } else if (expression->logical->operation.type == SPL_TOKEN_OR) {

        }
        


        InterpreterReturn right_ret = interpret_expression(expression->binary->right, scope);


    } else if (expression->type == EXPRESSION_TYPE_BINARY_OPERATION) {
        InterpreterReturn left_ret = interpret_expression(expression->binary->left, scope);
        InterpreterReturn right_ret = interpret_expression(expression->binary->right, scope);

        if (expression->binary->operation.type == SPL_TOKEN_PLUS) {
            if (left_ret.type == INTERPRETER_NUMBER && right_ret.type == INTERPRETER_NUMBER) {
                ret.type = INTERPRETER_NUMBER;
                ret.i = left_ret.i + right_ret.i;
            }
        }
    } 

    return ret;
}

void interpret_statement(Statement* statement, Scope* scope) {
    if (statement->type == STATEMENT_TYPE_PRINT) {
        InterpreterReturn ret = interpret_expression(statement->print_statement->value, scope);
        printf("%d\n", ret.i);
    } else if (statement->type == STATEMENT_TYPE_ASSIGNMENT) {

    } else if (statement->type == STATEMENT_TYPE_IF) {

    } else if (statement->type == STATEMENT_TYPE_WHILE) {

    } else if (statement->type == STATEMENT_TYPE_FOR) {

    }
}

void interpret_program(Program* program, Scope* scope) {
    for (int i = 0; program->statements && i < ckg_vector_count(program->statements); i++) {
        Statement* statement = program->statements[i];
        interpret_statement(statement, scope);
    }
}

void interpret_ast(ASTNode* ast) {
    Scope global_scope = scope_create(NULLPTR);
    interpret_program(ast->program, &global_scope);
}