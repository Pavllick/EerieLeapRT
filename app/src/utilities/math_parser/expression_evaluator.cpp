#include <stdexcept>
#include <regex>
#include <zephyr/logging/log.h>

#include "expression_evaluator.h"

namespace eerie_leap::utilities::math_parser {

LOG_MODULE_REGISTER(expression_evaluator_logger);
K_MUTEX_DEFINE(expression_eval_mutex_);

using namespace mu;

static const std::regex& sensorIdRegex() {
    static const std::regex regex_instance(R"(\{([a-z_][a-z0-9_]*)\})");
    
    return regex_instance;
}

ExpressionEvaluator::ExpressionEvaluator(std::shared_ptr<MathParserService> math_parser_service, const std::string& expression_raw)
    : math_parser_service_(std::move(math_parser_service)), expression_raw_(expression_raw) {
    expression_ = UnwrapVariables();
}

double ExpressionEvaluator::Evaluate(double x, const std::unordered_map<std::string, double>& variables) const {
    k_mutex_lock(&expression_eval_mutex_, K_FOREVER);

    math_parser_service_->SetExpression(expression_);

    math_parser_service_->DefineVariable("x", &x);
    for (auto& [key, value] : variables) {
        double unwrapped_value = value;
        math_parser_service_->DefineVariable(key, &unwrapped_value);
    }

    double res = math_parser_service_->Evaluate();

    k_mutex_unlock(&expression_eval_mutex_);
    
    return res;
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