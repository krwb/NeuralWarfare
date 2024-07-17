#pragma once
#include <numbers>
#include <cmath>

/// <summary>
/// Function to normalize an angle to between -pi and pi
/// </summary>
static double normalizeAngle(double angle) {
    angle = fmod(angle, 2 * std::numbers::pi);
    if (angle > std::numbers::pi) {
        angle -= 2 * std::numbers::pi;
    }
    else if (angle < -std::numbers::pi) {
        angle += 2 * std::numbers::pi;
    }
    return angle;
}