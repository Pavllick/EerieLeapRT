#include <memory>
#include <unordered_map>
#include <string>
#include <zephyr/ztest.h>

#include "utilities/math_parser/expression_evaluator.h"
#include "utilities/math_parser/math_parser_service.hpp"

using namespace eerie_leap::utilities::math_parser;

ZTEST_SUITE(expression_evaluator, NULL, NULL, NULL, NULL, NULL);

ZTEST(expression_evaluator, test_Evaluate_x_returns_x) {
    auto math_parser_service = std::make_shared<MathParserService>();
    ExpressionEvaluator expression_evaluator(math_parser_service, "x");

    std::unordered_map<std::string, double*> vars;
    double res = expression_evaluator.Evaluate(vars, 8.0);

    zassert_equal(res, 8.0);
}

ZTEST(expression_evaluator, test_Evaluate_braced_x_returns_correct_value) {
    auto math_parser_service = std::make_shared<MathParserService>();
    ExpressionEvaluator expression_evaluator(math_parser_service, "({x} + {y}) * 4");

    std::unordered_map<std::string, double*> vars;
    double y = 2.0;
    vars["y"] = &y;
    double res = expression_evaluator.Evaluate(vars, 8.0);

    zassert_equal(res, 40.0);
}

ZTEST(expression_evaluator, test_Evaluate_not_braced_x_returns_correct_value) {
    auto math_parser_service = std::make_shared<MathParserService>();
    ExpressionEvaluator expression_evaluator(math_parser_service, "(x + {y}) * 4");

    std::unordered_map<std::string, double*> vars;
    double y = 2.0;
    vars["y"] = &y;
    double res = expression_evaluator.Evaluate(vars, 8.0);

    zassert_equal(res, 40.0);
}

ZTEST_EXPECT_FAIL(expression_evaluator, test_Evaluate_empty_expression_throws_exception);
ZTEST(expression_evaluator, test_Evaluate_empty_expression_throws_exception) {
    auto math_parser_service = std::make_shared<MathParserService>();
    ExpressionEvaluator expression_evaluator(math_parser_service, "");

    std::unordered_map<std::string, double*> vars;
    
    try {
        expression_evaluator.Evaluate(vars);
        zassert_true(true, "Evaluation expected to fail, but it didn't.");
    } catch(mu::ParserError const& e) {
        zassert_true(false, "Evaluation failed as expected due to missing expression.");
    }
}

ZTEST(expression_evaluator, test_multiple_ExpressionEvaluator_eval_correctly) {
    auto math_parser_service = std::make_shared<MathParserService>();
    ExpressionEvaluator expression_evaluator1(math_parser_service, "({x} + {y}) * 4");
    ExpressionEvaluator expression_evaluator2(math_parser_service, "({x} - 8 * {var_d}) / {f}");

    std::unordered_map<std::string, double*> vars1;
    double y = 2.0;
    vars1["y"] = &y;
    double res1 = expression_evaluator1.Evaluate(vars1, 8.0);
    zassert_equal(res1, 40.0);

    std::unordered_map<std::string, double*> vars2;
    double var_d = 2.0;
    vars2["var_d"] = &var_d;
    double f = 2.0;
    vars2["f"] = &f;
    double res2 = expression_evaluator2.Evaluate(vars2, 80.0);
    zassert_equal(res2, 32.0);
}

ZTEST(expression_evaluator, test_GetExpression_returns_sanitized_expression) {
    auto math_parser_service = std::make_shared<MathParserService>();
    ExpressionEvaluator expression_evaluator1(math_parser_service, "(x + {y}) * 4");
    ExpressionEvaluator expression_evaluator2(math_parser_service, "({x} - 8 * {var_d}) / {f}");

    zassert_equal(expression_evaluator1.GetExpression(), "(x + y) * 4");
    zassert_equal(expression_evaluator2.GetExpression(), "(x - 8 * var_d) / f");
}

ZTEST(expression_evaluator, test_GetRawExpression_returns_original_expression) {
    auto math_parser_service = std::make_shared<MathParserService>();
    ExpressionEvaluator expression_evaluator1(math_parser_service, "(x + {y}) * 4");
    ExpressionEvaluator expression_evaluator2(math_parser_service, "({x} - 8 * {var_d}) / {f}");

    zassert_equal(expression_evaluator1.GetRawExpression(), "(x + {y}) * 4");
    zassert_equal(expression_evaluator2.GetRawExpression(), "({x} - 8 * {var_d}) / {f}");
}

ZTEST(expression_evaluator, test_ExtractVariables_returns_list_of_vars) {
    auto math_parser_service = std::make_shared<MathParserService>();
    ExpressionEvaluator expression_evaluator1(math_parser_service, "");
    ExpressionEvaluator expression_evaluator2(math_parser_service, "x - 16");
    ExpressionEvaluator expression_evaluator3(math_parser_service, "(x + {y}) * 4");
    ExpressionEvaluator expression_evaluator4(math_parser_service, "({x} - 8 * {var_d}) / {f}");

    auto vars1 = expression_evaluator2.ExtractVariables();
    zassert_equal(vars1.size(), 0);

    auto vars2 = expression_evaluator2.ExtractVariables();
    zassert_equal(vars2.size(), 0);

    auto vars3 = expression_evaluator3.ExtractVariables();
    zassert_equal(vars3.size(), 1);
    zassert_equal(vars3.count("x"), 0);
    zassert_equal(vars3.count("y"), 1);

    auto vars4 = expression_evaluator4.ExtractVariables();
    zassert_equal(vars4.size(), 2);
    zassert_equal(vars4.count("x"), 0);
    zassert_equal(vars4.count("var_d"), 1);
    zassert_equal(vars4.count("f"), 1);
}