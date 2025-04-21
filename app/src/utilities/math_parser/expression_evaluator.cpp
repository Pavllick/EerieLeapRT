#include <stdexcept>
#include <regex>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "expression_evaluator.h"
#include "exprtk_configuration.h"

namespace eerie_leap::utilities::math_parser {

LOG_MODULE_REGISTER(expression_evaluator_logger);

using namespace mu;

mu::Parser* ExpressionEvaluator::parser_ = new mu::Parser();

static const std::regex& sensorIdRegex() {
    static const std::regex regex_instance(R"(\{([a-z_][a-z0-9_]*)\})");
    
    return regex_instance;
}

ExpressionEvaluator::ExpressionEvaluator(const std::string& expression_raw) : expression_raw_(expression_raw) {
    expression_ = UnwrapVariables();
}

double ExpressionEvaluator::Evaluate(double x, const std::unordered_map<std::string, double>& variables) const {
    parser_->SetExpr(expression_);

    parser_->DefineVar("x", &x);
    for (auto& [key, value] : variables) {
        double unwrapped_value = value;
        parser_->DefineVar(key, &unwrapped_value);
    }
    
    return parser_->Eval();
}

std::unordered_set<std::string> ExpressionEvaluator::ExtractVariables() const {
    std::unordered_set<std::string> expression_variables;

    auto begin = std::sregex_iterator(expression_.begin(), expression_.end(), sensorIdRegex());
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        expression_variables.insert((*it)[1].str());
    }

    return expression_variables;
}

std::string ExpressionEvaluator::UnwrapVariables() const {
    return std::regex_replace(expression_raw_, sensorIdRegex(), "$1");
}

} // namespace eerie_leap::utilities::math_parser