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
            return JSON_STRING_VIEW(arena, TO_CJ_SV(expr->str->name));
        } else if (expr->type == EXPRESSION_TYPE_INTEGER) {
            return JSON_INT(arena, expr->integer->value);
        } else if (expr->type == EXPRESSION_TYPE_FLOAT) {
            return JSON_FLOAT(arena, expr->floating->value);
        } else if (expr->type == EXPRESSION_TYPE_BOOLEAN) {
            return JSON_BOOL(arena, expr->boolean->value);
        } else if (expr->type == EXPRESSION_TYPE_IDENTIFER) {
            JSON* nested = cj_create(arena);
            cj_push(nested, "name", TO_CJ_SV(expr->identifier->name));
            cj_push(json, "Identifier", nested);
        } else if (expr->type == EXPRESSION_TYPE_UNARY_OPERATION) {
            SPL_Token operation = expr->unary->operation;
            Expression* operand = expr->unary->operand;
            
            JSON* nested = cj_create(arena);
            cj_push(nested, "op", TO_CJ_SV(operation.name));
            cj_push(nested, "operand", ast_to_json_helper(json, ast_node_create(AST_NODE_EXPRESSION, operand), arena));

            cj_push(json, "UnaryOp", nested);
        } else if (expr->type == EXPRESSION_TYPE_BINARY_OPERATION) {
            SPL_Token operation = expr->unary->operation;
            Expression* left = expr->binary->left;
            Expression* right = expr->binary->right;
            
            JSON* nested = cj_create(arena);
            cj_push(nested, "op", TO_CJ_SV(operation.name));
            cj_push(nested, "left", ast_to_json_helper(json, ast_node_create(AST_NODE_EXPRESSION, left), arena));
            cj_push(nested, "right", ast_to_json_helper(json, ast_node_create(AST_NODE_EXPRESSION, right), arena));

            cj_push(json, "BinaryOp", nested);
        } else if (expr->type == EXPRESSION_TYPE_LOGICAL_OPERATION) {
            SPL_Token operation = expr->unary->operation;
            Expression* left = expr->binary->left;
            Expression* right = expr->binary->right;
            
            JSON* nested = cj_create(arena);
            cj_push(nested, "op", TO_CJ_SV(operation.name));
            cj_push(nested, "left", ast_to_json_helper(json, ast_node_create(AST_NODE_EXPRESSION, left), arena));
            cj_push(nested, "right", ast_to_json_helper(json, ast_node_create(AST_NODE_EXPRESSION, right), arena));

            cj_push(json, "LogicalOp", nested);
        } else if (expr->type == EXPRESSION_TYPE_GROUPING) {
            Expression* inner_expression = expr->grouping->value;
            cj_push(json, "Grouping", ast_to_json_helper(json, ast_node_create(AST_NODE_EXPRESSION, inner_expression), arena));
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