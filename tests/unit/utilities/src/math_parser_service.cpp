#include <zephyr/ztest.h>

#include "utilities/math_parser/math_parser_service.hpp"

using namespace eerie_leap::utilities::math_parser;

ZTEST_SUITE(math_parser_service, NULL, NULL, NULL, NULL, NULL);

ZTEST(math_parser_service, test_MathParserService_Evaluate_returns_correct_value) {
    MathParserService math_parser_service;
    math_parser_service.SetExpression("(x + y) * 4");
    double x = 8.0;
    math_parser_service.DefineVariable("x", &x);
    double y = 2.0;
    math_parser_service.DefineVariable("y", &y);

    zassert_equal(math_parser_service.Evaluate(), 40.0);
}

ZTEST(math_parser_service, test_MathParserService_evaluator_reuse) {
    MathParserService math_parser_service;
    math_parser_service.SetExpression("(x + y) * 4");
    double x = 8.0;
    math_parser_service.DefineVariable("x", &x);
    double y = 2.0;
    math_parser_service.DefineVariable("y", &y);

    zassert_equal(math_parser_service.Evaluate(), 40.0);

    math_parser_service.SetExpression("(x - 8 * var_d) / f");
    x = 80.0;
    math_parser_service.DefineVariable("x", &x);
    double var_d = 2.0;
    math_parser_service.DefineVariable("var_d", &var_d);
    double f = 2.0;
    math_parser_service.DefineVariable("f", &f);

    zassert_equal(math_parser_service.Evaluate(), 32.0);
}

ZTEST_EXPECT_FAIL(math_parser_service, test_MathParserService_missing_vars_fail);
ZTEST(math_parser_service, test_MathParserService_missing_vars_fail) {
    MathParserService math_parser_service;
    math_parser_service.SetExpression("(x + y) * 4");

    double x = 8.0;
    math_parser_service.DefineVariable("x", &x);
    double y = 2.0;
    math_parser_service.DefineVariable("y", &y);

    zassert_equal(math_parser_service.Evaluate(), 40.0);

    math_parser_service.SetExpression("(x + y) * 4");
    math_parser_service.DefineVariable("y", &y);

    try {
        math_parser_service.Evaluate();
        zassert_true(true, "Evaluation expected to fail, but it didn't.");
    } catch(mu::ParserError const& e) {
        zassert_true(false, "Evaluation failed as expected due to missing var.");
    }
}

ZTEST_EXPECT_FAIL(math_parser_service, test_MathParserService_no_vars_fail);
ZTEST(math_parser_service, test_MathParserService_no_vars_fail) {
    MathParserService math_parser_service;
    math_parser_service.SetExpression("(x + y) * 4");

    try {
        math_parser_service.Evaluate();
        zassert_true(true, "Evaluation expected to fail, but it didn't.");
    } catch(mu::ParserError const& e) {
        zassert_true(false, "Evaluation failed as expected due to missing var.");
    }
}