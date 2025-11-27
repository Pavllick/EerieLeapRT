#pragma once

#include <string>
#include <functional>
#include <muParser.h>

namespace eerie_leap::utilities::math_parser {

using namespace mu;

class MathParser {
private:
    mu::Parser parser_;

public:
    using VariableFactoryHandler = std::function<float*(const std::string&)>;

    explicit MathParser(const std::string& expression) {
        parser_.SetExpr(expression);
    }

    float Evaluate() const {
        return parser_.Eval();
    }

    void DefineVariable(const std::string& name, float* value) {
        parser_.DefineVar(name, value);
    }

    void SetVariableFactory(const VariableFactoryHandler& handler) {
        parser_.SetVarFactory([handler](string_type& name, void*) {
            return handler(name);
        });
    }
};

} // namespace eerie_leap::utilities::math_parser
