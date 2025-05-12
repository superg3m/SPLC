#include <expression.h>
#include <cj.h>

Expression* expressionPrimaryIntegerCreate(CKIT_Arena* parser_arena, int num) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_PRIMARY;
    ret->primary.PrimaryType = PRIMARY_INTEGER;
    ret->primary.integer_num = num;

    return ret;
}

Expression* expressionPrimaryFloatCreate(CKIT_Arena* parser_arena, float num) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_PRIMARY;
    ret->primary.PrimaryType = PRIMARY_FLOAT;
    ret->primary.float_num = num;

    return ret;
}

Expression* expressionUnaryCreate(CKIT_Arena* parser_arena, SPL_Token op, Expression* operand) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_URARY;
    ret->unary.op = op;
    ret->unary.operand = operand;

    return ret;
}

Expression* expressionTermCreate(CKIT_Arena* parser_arena, SPL_Token op, Expression* left, Expression* right) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_TERM;
    ret->term.op = op;
    ret->term.left = left;
    ret->term.right = right;

    return ret;
}

Expression* expressionFactorCreate(CKIT_Arena* parser_arena, SPL_Token op, Expression* left, Expression* right) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_FACTOR;
    ret->term.op = op;
    ret->term.left = left;
    ret->term.right = right;

    return ret;
}

Expression* expressionGroupingCreate(CKIT_Arena* parser_arena, Expression* expression) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_GROUPING;
    ret->grouping.expression = expression;

    return ret;
}

String expressionString(Expression* expression) {
    char* leftFMT[] = {
        "\"left\": {%s}",
        "\"left\": %s",
    };

    char* rightFMT[] = {
        "\"right\": {%s}",
        "\"right\": %s",
    };

    switch (expression->type) {
        case EXPRESSION_PRIMARY: {
            if (expression->primary.PrimaryType == PRIMARY_INTEGER) {
                return ckit_str_sprint(NULLPTR, "%d", expression->primary.integer_num);
            } else if (expression->primary.PrimaryType == PRIMARY_FLOAT) {
                return ckit_str_sprint(NULLPTR, "%f", expression->primary.float_num);
            }
        } break;

        case EXPRESSION_URARY: {
            char* operation = expression->unary.op.lexeme;
            if (expression->unary.operand->type == EXPRESSION_PRIMARY) {
                return ckit_str_sprint(NULLPTR, "\"UnaryOp\": {\"op\": \"%s\", \"operand\": %s}", operation, expressionString(expression->unary.operand));
            } else {
                return ckit_str_sprint(NULLPTR, "\"UnaryOp\": {\"op\": \"%s\", \"operand\": {%s}}", operation, expressionString(expression->unary.operand));
            }
        } break;
        
        case EXPRESSION_FACTOR: {
            char* operation = expression->factor.op.lexeme;
            Boolean left_is_primary = expression->factor.left->type == EXPRESSION_PRIMARY;
            Boolean right_is_primary = expression->factor.right->type == EXPRESSION_PRIMARY;
            
            String left_to_replace = ckit_str_sprint(NULLPTR, leftFMT[left_is_primary], expressionString(expression->factor.left));
            String right_to_replace = ckit_str_sprint(NULLPTR, rightFMT[right_is_primary], expressionString(expression->factor.right));

            return ckit_str_sprint(NULLPTR, "\"BinaryOp\": {\"op\": \"%s\", %s, %s}", operation, left_to_replace, right_to_replace);
        } break;

        case EXPRESSION_TERM: {
            char* operation = expression->term.op.lexeme;
            Boolean left_is_primary = expression->term.left->type == EXPRESSION_PRIMARY;
            Boolean right_is_primary = expression->term.right->type == EXPRESSION_PRIMARY;
            
            String left_to_replace = ckit_str_sprint(NULLPTR, leftFMT[left_is_primary], expressionString(expression->term.left));
            String right_to_replace = ckit_str_sprint(NULLPTR, rightFMT[right_is_primary], expressionString(expression->term.right));

            return ckit_str_sprint(NULLPTR, "\"BinaryOp\": {\"op\": \"%s\", %s, %s}", operation, left_to_replace, right_to_replace);
        } break;

        case EXPRESSION_GROUPING: {
            return ckit_str_sprint(NULLPTR, "\"Grouping\": {%s}", expressionString(expression->grouping.expression));
        } break;
    }

    return NULLPTR;
}


void expressionPrint(Expression* expression) {
    char* json_string = ckit_cstr_sprint(NULLPTR, "{%s}", expressionString(expression));

    CJ_Arena* arena = cj_arena_create(0);

    JSON* json = cj_parse(arena, json_string);
    ckit_assert_msg(json, "NOT VALID JSON");

    char* pretty_json = cj_to_string(json);
    printf("%s\n", pretty_json);

    cj_arena_free(arena);
    free(pretty_json);

    ckit_free(json_string);
}