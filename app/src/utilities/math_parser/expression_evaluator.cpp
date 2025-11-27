#include <stdexcept>
#include <string>
#include <algorithm>
#include <zephyr/logging/log.h>

#include "utilities/string/string_helpers.h"

#include "expression_evaluator.h"

namespace eerie_leap::utilities::math_parser {

using namespace eerie_leap::utilities::string;

LOG_MODULE_REGISTER(expression_evaluator_logger);

using namespace mu;

ExpressionEvaluator::ExpressionEvaluator(std::string expression_raw)
    : expression_raw_(std::move(expression_raw)) {
    expression_ = UnwrapVariables();
    variables_ = ExtractVariables();

    math_parser_ = std::make_unique<MathParser>(expression_);
}

float ExpressionEvaluator::Evaluate(const std::unordered_map<size_t, float*>& variables, std::optional<float> x) const {
    if(x.has_value())
        math_parser_->DefineVariable("x", &x.value());

    for(const auto& [hash, name] : variables_) {
        if(!variables.contains(hash))
            throw std::runtime_error("Variable '" + name + "' required for evaluation not found.");

        math_parser_->DefineVariable(name, variables.at(hash));
    }

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

std::unordered_map<size_t, std::string> ExpressionEvaluator::ExtractVariables() const {
    std::string sanitized_expression = SanitizeExpression(expression_raw_);
    std::unordered_map<size_t, std::string> variables;

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

        variables.insert({StringHelpers::GetHash(var), var});
    }

    return variables;
}

const std::string& ExpressionEvaluator::GetExpression() const {
    return expression_;
}

const std::string& ExpressionEvaluator::GetRawExpression() const {
    return expression_raw_;
}

const std::unordered_set<size_t> ExpressionEvaluator::GetVariableNameHashes() const {
    std::unordered_set<size_t> result;
    for(const auto& [hash, _] : variables_)
        result.insert(hash);

    return result;
}

const std::unordered_set<std::string> ExpressionEvaluator::GetVariableNames() const {
    std::unordered_set<std::string> result;
    for(const auto& [_, name] : variables_)
        result.insert(name);

    return result;
}

const std::string& ExpressionEvaluator::GetVariableName(size_t hash) const {
    return variables_.at(hash);
}

} // namespace eerie_leap::utilities::math_parser
