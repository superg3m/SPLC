#include <ast.h>

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