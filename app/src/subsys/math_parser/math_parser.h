#pragma once

#include <string>
#include <functional>
#include <unordered_set>
#include <muParser.h>

#include <zephyr/kernel.h>

namespace eerie_leap::subsys::math_parser {

using namespace mu;

class MathParser {
private:
    mu::Parser parser_;
    std::unordered_set<std::string> variable_names_;

public:
    using VariableFactoryHandler = std::function<float*(const std::string&)>;

    explicit MathParser(const std::string& expression) {
        parser_.SetExpr(expression);

        auto variable_names = parser_.GetUsedVar();
        for(auto& [name, value] : variable_names)
            variable_names_.insert(name);
    }

    const std::unordered_set<std::string>& GetVariableNames() const {
        return variable_names_;
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

} // namespace eerie_leap::subsys::math_parser
