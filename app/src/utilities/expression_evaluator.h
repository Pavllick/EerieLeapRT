#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

class ExpressionEvaluator {
public:
    static double Evaluate(const std::string& expression, double raw_value, const std::unordered_map<std::string, double>& variables);
    static std::unordered_set<std::string> ExtractSensorIds(const std::string& expression);
    static std::string UnwrapVariables(const std::string& expression);
};

} // namespace eerie_leap::utilities