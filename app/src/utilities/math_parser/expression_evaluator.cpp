#include <stdexcept>
#include <string>
#include <algorithm>
#include <zephyr/logging/log.h>

#include "expression_evaluator.h"

namespace eerie_leap::utilities::math_parser {

LOG_MODULE_REGISTER(expression_evaluator_logger);

using namespace mu;

ExpressionEvaluator::ExpressionEvaluator(std::string expression_raw)
    : expression_raw_(std::move(expression_raw)) {
    expression_ = UnwrapVariables();
    variable_names_ = ExtractVariables();

    math_parser_ = std::make_unique<MathParser>(expression_);
}

void ExpressionEvaluator::RegisterVariableValueHandler(const MathParser::VariableFactoryHandler& handler) {
    math_parser_->SetVariableFactory(handler);
}

float ExpressionEvaluator::Evaluate(std::optional<float> x) const {
    if(x.has_value())
        math_parser_->DefineVariable("x", &x.value());

    return math_parser_->Evaluate();
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
    std::unordered_set<std::string> variables;

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

        variables.insert(var);
    }

    return variables;
}

const std::string& ExpressionEvaluator::GetExpression() const {
    return expression_;
}

const std::string& ExpressionEvaluator::GetRawExpression() const {
    return expression_raw_;
}

const std::unordered_set<std::string> ExpressionEvaluator::GetVariableNames() const {
    return variable_names_;
}

} // namespace eerie_leap::utilities::math_parser
