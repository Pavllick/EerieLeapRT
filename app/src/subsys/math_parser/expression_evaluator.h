#pragma once

#include <memory>
#include <string>
#include <optional>
#include <unordered_set>

#include "math_parser.h"

namespace eerie_leap::subsys::math_parser {

class ExpressionEvaluator {
private:
    std::unique_ptr<MathParser> math_parser_;
    std::string expression_;
    float x_;

public:
    explicit ExpressionEvaluator(std::string expression);
    virtual ~ExpressionEvaluator() = default;

    ExpressionEvaluator(const ExpressionEvaluator&) = delete;
    ExpressionEvaluator& operator=(const ExpressionEvaluator&) = delete;

    const std::string& GetExpression() const;
    const std::unordered_set<std::string> GetVariableNames() const;
    void RegisterVariableValueHandler(const MathParser::VariableFactoryHandler& handler);

    float Evaluate(std::optional<float> x = std::nullopt);
};

} // namespace eerie_leap::subsys::math_parser
