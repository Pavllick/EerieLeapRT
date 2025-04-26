#pragma once

#include <memory>
#include <string>
#include <vector>
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
    
public:
    ExpressionEvaluator(std::shared_ptr<MathParserService> math_parser_service, const std::string& expression);

    std::string GetExpression() const { return expression_; }
    std::string GetRawExpression() const { return expression_raw_; }

    double Evaluate(const std::unordered_map<std::string, double>& variables, std::optional<double> x = std::nullopt) const;
    std::unordered_set<std::string> ExtractVariables() const;
    std::string UnwrapVariables() const;
};

} // namespace eerie_leap::utilities::math_parser