#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <zephyr/kernel.h>
#include <muParser.h>

namespace eerie_leap::utilities::math_parser {

using namespace mu;
class ExpressionEvaluator {
private:
    static mu::Parser* parser_;

    std::string expression_raw_;
    std::string expression_;
    
public:
    ExpressionEvaluator(const std::string& expression);

    std::string GetExpression() const { return expression_; }
    std::string GetRawExpression() const { return expression_raw_; }

    double Evaluate(double x, const std::unordered_map<std::string, double>& variables) const;
    std::unordered_set<std::string> ExtractVariables() const;
    std::string UnwrapVariables() const;
};

} // namespace eerie_leap::utilities::math_parser