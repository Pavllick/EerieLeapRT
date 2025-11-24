#include <zephyr/ztest.h>

#include "utilities/math_parser/math_parser.hpp"

using namespace eerie_leap::utilities::math_parser;

ZTEST_SUITE(math_parser, NULL, NULL, NULL, NULL, NULL);

ZTEST(math_parser, test_MathParser_Evaluate_returns_correct_value) {
    MathParser math_parser("(x + y) * 4");
    float x = 8.0;
    math_parser.DefineVariable("x", &x);
    float y = 2.0;
    math_parser.DefineVariable("y", &y);

    zassert_equal(math_parser.Evaluate(), 40.0);
}

ZTEST_EXPECT_FAIL(math_parser, test_MathParser_missing_vars_fail);
ZTEST(math_parser, test_MathParser_missing_vars_fail) {
    MathParser math_parser("(x + y) * 4");
    float y = 2.0;
    math_parser.DefineVariable("y", &y);

    try {
        math_parser.Evaluate();
        zassert_true(true, "Evaluation expected to fail, but it didn't.");
    } catch(mu::ParserError const& e) {
        zassert_true(false, "Evaluation failed as expected due to missing var.");
    }
}

ZTEST_EXPECT_FAIL(math_parser, test_MathParser_no_vars_fail);
ZTEST(math_parser, test_MathParser_no_vars_fail) {
    MathParser math_parser("(x + y) * 4");

    try {
        math_parser.Evaluate();
        zassert_true(true, "Evaluation expected to fail, but it didn't.");
    } catch(mu::ParserError const& e) {
        zassert_true(false, "Evaluation failed as expected due to missing var.");
    }
}
