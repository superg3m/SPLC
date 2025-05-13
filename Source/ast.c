#include <ast.h>
#include <cj.h>

ASTNode* ast_node_create(AST_NodeType type, void* node) {
    ASTNode* ret = ckg_alloc(sizeof(ASTNode));
    ret->type = type;

    if (ret->type == AST_NODE_EXPRESSION) {
        ret->expression = (Expression*)node;
    } else if (ret->type == AST_NODE_STATEMENT) {
        ret->statement = (Statement*)node;
    } else if (ret->type == AST_NODE_PROGRAM) {
        ret->program = (Program*)node;
    }

    return ret;
}

static bool is_primitive_type(Expression* expr) {
    return expr->type == EXPRESSION_TYPE_STRING ||
           expr->type == EXPRESSION_TYPE_INTEGER ||
           expr->type == EXPRESSION_TYPE_FLOAT ||
           expr->type == EXPRESSION_TYPE_BOOLEAN;
}

static const char* bool_to_string(bool value) {
    return value ? "true" : "false";
}

internal JSON* ast_to_json_helper(JSON* json, ASTNode* node, CJ_Arena* arena) {
    #define TO_CJ_SV(sv) (CJ_StringView){sv.data, 0, sv.length}

    if (node->type == AST_NODE_EXPRESSION) {
        Expression* expr = node->expression;
        if (expr->type == EXPRESSION_TYPE_STRING) {

        } else if (expr->type == EXPRESSION_TYPE_INTEGER) {

        } else if (expr->type == EXPRESSION_TYPE_FLOAT) {

        } else if (expr->type == EXPRESSION_TYPE_BOOLEAN) {

        } else if (expr->type == EXPRESSION_TYPE_IDENTIFER) {
            JSON* nested = cj_create(arena);
            cj_push(nested, "name", TO_CJ_SV(expr->identifier->name));
            cj_push(json, "Identifier", nested);
        } else if (expr->type == EXPRESSION_TYPE_UNARY_OPERATION) {
            SPL_Token operation = expr->unary->operation;
            Expression* operand = expr->unary->operand;
            
            JSON* nested = cj_create(arena);
            cj_push(nested, "op", TO_CJ_SV(operation.name));

            if (is_primitive_type(expr->unary->operand)) {
                if (operand->type == EXPRESSION_TYPE_STRING) {
                    cj_push(nested, "operand", TO_CJ_SV(operand->str->name));
                } else if (operand->type == EXPRESSION_TYPE_INTEGER) {
                    cj_push(nested, "operand", operand->integer->value);
                } else if (operand->type == EXPRESSION_TYPE_FLOAT) {
                    cj_push(nested, "operand", operand->floating->value);
                } else if (operand->type == EXPRESSION_TYPE_BOOLEAN) {
                    cj_push(nested, "operand", (Boolean)operand->boolean->value);
                }
            } else {

                cj_push(nested, "operand", TO_CJ_SV(expr->unary->operation.name));
            }




            cj_push(json, "Unary", nested);
        } else if (expr->type == EXPRESSION_TYPE_BINARY_OPERATION) {

        } else if (expr->type == EXPRESSION_TYPE_LOGICAL_OPERATION) {

        } else if (expr->type == EXPRESSION_TYPE_GROUPING) {

        }
    } 

    return NULL;
}

internal JSON* ast_to_json(ASTNode* ast, CJ_Arena* arena) {
    JSON* root = cj_create(arena);
    ast_to_json_helper(root, ast, arena);

    return root;
}

void ast_pretty_print(ASTNode* ast) {
    char* indent = "    ";
    cj_set_context_indent(indent);
    CJ_Arena* arena = cj_arena_create(0);

    char* str = cj_to_string(ast_to_json(ast, arena));
    printf("%s\n", str);

    cj_arena_free(arena);
}