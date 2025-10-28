#pragma once

#include <memory>
#include <string>
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <zephyr/kernel.h>

#include "math_parser_service.hpp"

namespace eerie_leap::utilities::math_parser {

class ExpressionEvaluator {
private:
    std::shared_ptr<MathParserService> math_parser_service_;

    std::string expression_raw_;
    std::string expression_;
    std::unordered_set<std::string> variables_;

    std::string SanitizeExpression(const std::string& expression) const;
    std::string UnwrapVariables() const;
    bool IsValidVariableName(const std::string& str) const;
    std::unordered_set<std::string> ExtractVariables() const;

public:
    ExpressionEvaluator(std::shared_ptr<MathParserService> math_parser_service, const std::string& expression);

    const std::string& GetExpression() const { return expression_; }
    const std::string& GetRawExpression() const { return expression_raw_; }
    const std::unordered_set<std::string>& GetVariables() const { return variables_; }

    float Evaluate(const std::unordered_map<std::string, float*>& variables, std::optional<float> x = std::nullopt) const;
};

} // namespace eerie_leap::utilities::math_parser
