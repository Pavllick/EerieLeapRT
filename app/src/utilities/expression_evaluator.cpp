#include <stdexcept>
#include <regex>

#include "expression_evaluator.h"
#include "exprtk_configuration.h"
#include "libs/exprtk/exprtk.hpp"

namespace eerie_leap::utilities {

static const std::regex& sensorIdRegex() {
    static const std::regex regex_instance(R"(\{([a-z_][a-z0-9_]*)\})");
    
    return regex_instance;
}

double ExpressionEvaluator::Evaluate(const std::string& expression, double x, const std::unordered_map<std::string, double>& variables) {
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double>   expression_t;
    typedef exprtk::parser<double>       parser_t;

    symbol_table_t symbol_table;
    symbol_table.add_constants();
    symbol_table.add_variable("x", x);

    for (const auto& [key, value] : variables) {
        double unwrapped_value = value;
        symbol_table.add_variable(key, unwrapped_value);
    }

    expression_t evaluator;
    evaluator.register_symbol_table(symbol_table);

    parser_t parser;

    if (!parser.compile(expression, evaluator))
        throw std::runtime_error("Failed to compile expression: " + expression);

    return evaluator.value();
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