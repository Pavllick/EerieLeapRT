#pragma once

#include <memory>
#include <string>
#include <optional>
#include <unordered_set>

#include "math_parser.hpp"

namespace eerie_leap::utilities::math_parser {

class ExpressionEvaluator {
private:
    std::unique_ptr<MathParser> math_parser_;

    std::string expression_raw_;
    std::string expression_;
    std::unordered_set<std::string> variable_names_;

    std::string SanitizeExpression(const std::string& expression) const;
    std::string UnwrapVariables() const;
    bool IsValidVariableName(const std::string& str) const;
    std::unordered_set<std::string> ExtractVariables() const;

public:
    explicit ExpressionEvaluator(std::string expression);

    const std::string& GetExpression() const;
    const std::string& GetRawExpression() const;
    const std::unordered_set<std::string> GetVariableNames() const;
    void RegisterVariableValueHandler(const MathParser::VariableFactoryHandler& handler);

    float Evaluate(std::optional<float> x = std::nullopt) const;
};

} // namespace eerie_leap::utilities::math_parser
