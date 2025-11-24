#pragma once

#include <string>
#include <muParser.h>

namespace eerie_leap::utilities::math_parser {

using namespace mu;

class MathParser {
private:
    mu::Parser parser_;

public:
    explicit MathParser(const std::string& expression) {
        parser_.SetExpr(expression);
    }

    float Evaluate() const {
        return parser_.Eval();
    }

    void DefineVariable(const std::string& name, float* value) {
        parser_.DefineVar(name, value);
    }
};

} // namespace eerie_leap::utilities::math_parser
