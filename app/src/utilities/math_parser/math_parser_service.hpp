#pragma once

#include <string>
#include <muParser.h>

namespace eerie_leap::utilities::math_parser {

using namespace mu;

class MathParserService {
private:
    mu::Parser parser_;

public:
    void SetExpression(const std::string& expression) {
        parser_.SetExpr(expression);
        parser_.ClearVar();
    }

    float Evaluate() const {
        return parser_.Eval();
    }

    void DefineVariable(const std::string& name, float* value) {
        parser_.DefineVar(name, value);
    }
};

} // namespace eerie_leap::utilities::math_parser
