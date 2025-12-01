#include <string>

#include "expression_evaluator.h"

namespace eerie_leap::subsys::math_parser {

using namespace mu;

ExpressionEvaluator::ExpressionEvaluator(std::string expression)
    : expression_(std::move(expression)), x_(0.0) {

    math_parser_ = std::make_unique<MathParser>(expression_);

    if(math_parser_->GetVariableNames().contains("x"))
        math_parser_->DefineVariable("x", &x_);
}

void ExpressionEvaluator::RegisterVariableValueHandler(const MathParser::VariableFactoryHandler& handler) {
    math_parser_->SetVariableFactory(handler);
}

float ExpressionEvaluator::Evaluate(std::optional<float> x) {
    if(math_parser_->GetVariableNames().contains("x"))
        x_ = x.value();

    return math_parser_->Evaluate();
}

const std::string& ExpressionEvaluator::GetExpression() const {
    return expression_;
}

const std::unordered_set<std::string> ExpressionEvaluator::GetVariableNames() const {
    return math_parser_->GetVariableNames();
}

} // namespace eerie_leap::subsys::math_parser
