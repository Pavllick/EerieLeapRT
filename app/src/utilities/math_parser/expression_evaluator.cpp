#include <stdexcept>
#include <string>
#include <algorithm>
#include <zephyr/logging/log.h>

#include "expression_evaluator.h"
#include <muParser.h>

namespace eerie_leap::utilities::math_parser {

LOG_MODULE_REGISTER(expression_evaluator_logger);
K_MUTEX_DEFINE(expression_eval_mutex_);

using namespace mu;

ExpressionEvaluator::ExpressionEvaluator(std::shared_ptr<MathParserService> math_parser_service, const std::string& expression_raw)
    : math_parser_service_(std::move(math_parser_service)), expression_raw_(expression_raw) {
    expression_ = UnwrapVariables();
}

float ExpressionEvaluator::Evaluate(const std::unordered_map<std::string, float*>& variables, std::optional<float> x) const {

    k_mutex_lock(&expression_eval_mutex_, K_FOREVER);

    math_parser_service_->SetExpression(expression_);

    if(x.has_value())
        math_parser_service_->DefineVariable("x", &x.value());

    for(const auto& [key, value]: variables)
        math_parser_service_->DefineVariable(key, value);

    float res = math_parser_service_->Evaluate();

    k_mutex_unlock(&expression_eval_mutex_);

    return res;
}

std::string ExpressionEvaluator::SanitizeExpression(const std::string& expression) const {
    std::string result;
    result.reserve(expression.size());

    for(char c : expression) {
        if(c != ' ')
            result += c;
    }

    return result;
}

std::string ExpressionEvaluator::UnwrapVariables() const {
    std::string result;
    result.reserve(expression_raw_.size());

    for(char c : expression_raw_) {
        if(c != '{' && c != '}' && c != ' ')
            result += c;
    }

    return result;
}

bool ExpressionEvaluator::IsValidVariableName(const std::string& str) const {
    if(str.empty())
        return false;

    if(!std::isalpha(str[0]) && str[0] != '_')
        return false;

    return std::ranges::all_of(str, [](char c) {
        return std::isalnum(c) || c == '_';
    });
}

std::unordered_set<std::string> ExpressionEvaluator::ExtractVariables() const {
    std::string sanitized_expression = SanitizeExpression(expression_raw_);
    std::unordered_set<std::string> vars;

    std::size_t close_brace_i = 0;

    while(true) {
        std::size_t open = sanitized_expression.find('{', close_brace_i);
        std::size_t close = sanitized_expression.find('}', close_brace_i);

        if(open == std::string::npos && close == std::string::npos)
            break;
        else if(close == std::string::npos)
            throw std::runtime_error("Missing closing brace");
        else if(open == std::string::npos)
            throw std::runtime_error("Missing opening brace");

        close_brace_i = close + 1;

        std::string var = sanitized_expression.substr(open + 1, close - open - 1);
        if(var.empty() || var == "x")
            continue;

        if(!IsValidVariableName(var))
            throw std::runtime_error("Invalid variable name");

        vars.insert(var);
    }

    return vars;
}

} // namespace eerie_leap::utilities::math_parser
