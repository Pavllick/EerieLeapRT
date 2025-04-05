#include <regex>
#include "exprtk_configuration.h"
#include "libs/exprtk/exprtk.hpp"
#include "expression_evaluator.h"

namespace eerie_leap::utilities {

double ExpressionEvaluator::Evaluate(const std::string& expression2, double raw_value, const std::unordered_map<std::string, double>& variables) {
    // return 1.4;

    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double>   expression_t;
    typedef exprtk::parser<double>       parser_t;

    const std::string expression_string = "z := x - (3 * y)";

    double x = double(123.456);
    double y = double(98.98);
    double z = double(0.0);

    symbol_table_t symbol_table;
    symbol_table.add_variable("x",x);
    symbol_table.add_variable("y",y);
    symbol_table.add_variable("z",z);

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;

    if (!parser.compile(expression_string, expression)) {
        // printf("Compilation error...\n");
        return -1.0;
    }

    return expression.value();
}

static const std::regex& sensorIdRegex() {
    static const std::regex regex_instance(R"(\{([a-z_][a-z0-9_]*)\})");
    return regex_instance;
}

std::unordered_set<std::string> ExpressionEvaluator::ExtractSensorIds(const std::string& expression) {
    std::unordered_set<std::string> sensorIds;

    auto begin = std::sregex_iterator(expression.begin(), expression.end(), sensorIdRegex());
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        sensorIds.insert((*it)[1].str());
    }

    return sensorIds;
}

std::string ExpressionEvaluator::UnwrapVariables(const std::string& expression) {
    return std::regex_replace(expression, sensorIdRegex(), "$1");
}

} // namespace eerie_leap::utilities