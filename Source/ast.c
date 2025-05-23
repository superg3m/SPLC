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

Program* program_create(Statement** statements) {
    Program* ret = ckg_alloc(sizeof(Program));
    ret->statements = statements;

    return ret;
}

internal JSON* ast_to_json_helper(JSON* root, ASTNode* node, CJ_Arena* arena) {
    #define TO_CJ_SV(sv) (CJ_StringView){sv.data, 0, sv.length}

    if (node->type == AST_NODE_EXPRESSION) {
        Expression* expr = node->expression;
        if (expr->type == EXPRESSION_TYPE_STRING) {
            return JSON_STRING_VIEW(arena, ((CJ_StringView){expr->str->name.data + 1, 0, expr->str->name.length - 2}));
        } else if (expr->type == EXPRESSION_TYPE_INTEGER) {
            return JSON_INT(arena, expr->integer->value);
        } else if (expr->type == EXPRESSION_TYPE_FLOAT) {
            return JSON_FLOAT(arena, expr->floating->value);
        } else if (expr->type == EXPRESSION_TYPE_BOOLEAN) {
            return JSON_BOOL(arena, expr->boolean->value);
        } else if (expr->type == EXPRESSION_TYPE_IDENTIFER) {
            JSON* nested = cj_create(arena);
            cj_push(nested, "name", TO_CJ_SV(expr->identifier->name));
            cj_push(root, "Identifier", nested);

            return root;
        } else if (expr->type == EXPRESSION_TYPE_UNARY_OPERATION) {
            SPL_Token operation = expr->unary->operation;
            Expression* operand = expr->unary->operand;
            
            JSON* nested = cj_create(arena);
            JSON* operand_json = cj_create(arena);
            ast_to_json_helper(operand_json, ast_node_create(AST_NODE_EXPRESSION, operand), arena);

            cj_push(nested, "op", TO_CJ_SV(operation.name));
            cj_push(nested, "operand", operand_json);
            
            cj_push(root, "UnaryOp", nested);

            return root;
        } else if (expr->type == EXPRESSION_TYPE_BINARY_OPERATION) {
            SPL_Token operation = expr->binary->operation;
            Expression* left = expr->binary->left;
            Expression* right = expr->binary->right;
            
            JSON* nested = cj_create(arena);
            JSON* left_json = cj_create(arena);
            JSON* right_json = cj_create(arena);

            cj_push(nested, "op", TO_CJ_SV(operation.name));
            cj_push(nested, "left", ast_to_json_helper(left_json, ast_node_create(AST_NODE_EXPRESSION, left), arena));
            cj_push(nested, "right", ast_to_json_helper(right_json, ast_node_create(AST_NODE_EXPRESSION, right), arena));

            cj_push(root, "BinaryOp", nested);

            return root;
        } else if (expr->type == EXPRESSION_TYPE_LOGICAL_OPERATION) {
            SPL_Token operation = expr->logical->operation;
            Expression* left = expr->logical->left;
            Expression* right = expr->logical->right;
            
            JSON* nested = cj_create(arena);
            JSON* left_json = cj_create(arena);
            JSON* right_json = cj_create(arena);
            
            cj_push(nested, "op", TO_CJ_SV(operation.name));
            cj_push(nested, "left", ast_to_json_helper(left_json, ast_node_create(AST_NODE_EXPRESSION, left), arena));
            cj_push(nested, "right", ast_to_json_helper(right_json, ast_node_create(AST_NODE_EXPRESSION, right), arena));
            
            cj_push(root, "LogicalOp", nested);

            return root;
        } else if (expr->type == EXPRESSION_TYPE_GROUPING) {
            Expression* inner_expression = expr->grouping->value;
            JSON* grouping_json = cj_create(arena);

            cj_push(root, "Grouping", ast_to_json_helper(grouping_json, ast_node_create(AST_NODE_EXPRESSION, inner_expression), arena));

            return root;
        }
    } else if (node->type == AST_NODE_STATEMENT) {
        Statement* stmt = node->statement;
        if (stmt->type == STATEMENT_TYPE_PRINT) {
            Expression* inner_expression = stmt->print_statement->value;
            JSON* print_json = cj_create(arena);
            cj_push(root, "PrintStatement", ast_to_json_helper(print_json, ast_node_create(AST_NODE_EXPRESSION, inner_expression), arena));

            return root;
        } else if (stmt->type == STATEMENT_TYPE_PRINTLN) {
            Expression* inner_expression = stmt->println_statement->value;
            JSON* print_json = cj_create(arena);
            cj_push(root, "PrintLnStatement", ast_to_json_helper(print_json, ast_node_create(AST_NODE_EXPRESSION, inner_expression), arena));

            return root;
        } else if (stmt->type == STATEMENT_TYPE_ASSIGNMENT) {
            Expression* left = stmt->assignment_statement->left;
            Expression* right = stmt->assignment_statement->right;

            JSON* nested = cj_create(arena);
            JSON* left_json = cj_create(arena);
            JSON* right_json = cj_create(arena);

            cj_push(nested, "left", ast_to_json_helper(left_json, ast_node_create(AST_NODE_EXPRESSION, left), arena));
            cj_push(nested, "right", ast_to_json_helper(right_json, ast_node_create(AST_NODE_EXPRESSION, right), arena));
            
            cj_push(root, "AssignmentStatement", nested);

            return root;
        } else if (stmt->type == STATEMENT_TYPE_IF) {
            Expression* condition = stmt->if_statement->value;
            Statement** if_code_block = stmt->if_statement->if_code_block;
            Statement** else_code_block = stmt->if_statement->else_code_block;

            JSON* nested = cj_create(arena);
            JSON* value_json = cj_create(arena);
            JSON* if_code_block_array = cj_array_create(arena);
            JSON* else_code_block_array = cj_array_create(arena);

            for (int i = 0; if_code_block && i < ckg_vector_count(if_code_block); i++) {
                JSON* if_statement_json = cj_create(arena);
                cj_array_push(if_code_block_array, ast_to_json_helper(if_statement_json, ast_node_create(AST_NODE_STATEMENT, (void*)if_code_block[i]), arena));
            }
   
            for (int i = 0; else_code_block && i < ckg_vector_count(else_code_block); i++) {
                JSON* else_statement_json = cj_create(arena);
                cj_array_push(if_code_block_array, ast_to_json_helper(else_statement_json, ast_node_create(AST_NODE_STATEMENT, (void*)else_code_block[i]), arena));
            }

            cj_push(nested, "condition", ast_to_json_helper(value_json, ast_node_create(AST_NODE_EXPRESSION, condition), arena));
            cj_push(nested, "if_code", if_code_block_array);
            cj_push(nested, "else_code", else_code_block_array);
            
            cj_push(root, "IfStatement", nested);

            return root;
        } else if (stmt->type == STATEMENT_TYPE_WHILE) {
            Expression* condition = stmt->while_statement->value;
            Statement** while_code_block = stmt->while_statement->while_code_block;

            JSON* nested = cj_create(arena);
            JSON* value_json = cj_create(arena);
            JSON* while_code_block_array = cj_array_create(arena);

            for (int i = 0; while_code_block && i < ckg_vector_count(while_code_block); i++) {
                JSON* while_statement_json = cj_create(arena);
                cj_array_push(while_code_block_array, ast_to_json_helper(while_statement_json, ast_node_create(AST_NODE_STATEMENT, (void*)while_code_block[i]), arena));
            }

            cj_push(nested, "condition", ast_to_json_helper(value_json, ast_node_create(AST_NODE_EXPRESSION, condition), arena));
            cj_push(nested, "while_code", while_code_block_array);

            cj_push(root, "WhileStatement", nested);

            return root;
        } else if (stmt->type == STATEMENT_TYPE_FOR) {
            Statement* initialization = stmt->for_statement->initalization;
            Expression* condition = stmt->for_statement->condition;
            Statement* increment = stmt->for_statement->increment;
            Statement** for_code_block = stmt->for_statement->for_code_block;

            JSON* nested = cj_create(arena);
            JSON* initialization_json = cj_create(arena);
            JSON* condition_json = cj_create(arena);
            JSON* increment_json = cj_create(arena);

            JSON* for_code_block_array = cj_array_create(arena);

            for (int i = 0; for_code_block && i < ckg_vector_count(for_code_block); i++) {
                JSON* for_statement_json = cj_create(arena);
                cj_array_push(for_code_block_array, ast_to_json_helper(for_statement_json, ast_node_create(AST_NODE_STATEMENT, (void*)for_code_block[i]), arena));
            }

            cj_push(nested, "Initialization", ast_to_json_helper(initialization_json, ast_node_create(AST_NODE_STATEMENT, initialization), arena));
            cj_push(nested, "Condition", ast_to_json_helper(condition_json, ast_node_create(AST_NODE_EXPRESSION, condition), arena));
            cj_push(nested, "Increment", ast_to_json_helper(increment_json, ast_node_create(AST_NODE_STATEMENT, increment), arena));
            cj_push(nested, "for_code", for_code_block_array);

            cj_push(root, "ForStatement", nested);

            return root;
        }
    } else if (node->type == AST_NODE_PROGRAM) {
        Statement** statements = node->program->statements;

        JSON* nested_array = cj_array_create(arena);
        for (int i = 0;  statements && i < ckg_vector_count(statements); i++) {
            JSON* statement_json = cj_create(arena);
            cj_array_push(nested_array, ast_to_json_helper(statement_json, ast_node_create(AST_NODE_STATEMENT, statements[i]), arena));
        }
        
        cj_push(root, "Program", nested_array);
        
        return root;
    }

    return NULL;
}

internal JSON* ast_to_json(ASTNode* ast, CJ_Arena* arena) {
    JSON* root = cj_create(arena);
    return ast_to_json_helper(root, ast, arena);;
}

void ast_pretty_print(ASTNode* ast) {
    char* indent = "    ";
    cj_set_context_indent(indent);
    CJ_Arena* arena = cj_arena_create(0);

    JSON* json = ast_to_json(ast, arena);
    ckg_assert_msg(json, "Failed to jsonify ast\n");

    char* str = cj_to_string(json);
    printf("%s\n", str);

    cj_arena_free(arena);
}