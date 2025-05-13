#include <spl_parser.h>

// <logical>    ::= <comparison> (("||" | "&&") <comparison>)*
internal parse_logical_expression(Parser* parser) {
    Expression* expression = parse_comparison_expression(parser);
}

// <expression> ::= <logical_expr>
internal ASTNode* parse_expression(Parser* parser) {
    return parse_logical_expression(parser);
}

ASTNode* parse(Parser* parser) {
    ASTNode* ast = parse_expression(parser);
    
    return ast;
}