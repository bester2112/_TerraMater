/**
 * @file Settings.h
 * @author Thomas Rzepka (uqdll@student.kit.edu)
 * @brief Holds the Input structures
 * @version 0.1
 * @date 2020-05-05
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "glm/glm.hpp"

#ifndef INPUT_H
#define INPUT_H


struct INPUT {
    int walkForwards    = GLFW_RELEASE;
    int walkBackwards   = GLFW_RELEASE;
    int strafeRight     = GLFW_RELEASE;
    int strafeLeft      = GLFW_RELEASE;
    int leftForwards    = GLFW_RELEASE;
    int rightForwards   = GLFW_RELEASE;
    int leftBackwards   = GLFW_RELEASE;
    int rightBackwards  = GLFW_RELEASE;
    int jump            = GLFW_RELEASE;
    int plus            = GLFW_RELEASE;
    int minus           = GLFW_RELEASE;
    int numberPressed   = 0; // default value = 1, value bewtween 1 <= x <= 9
    int numberPressedAction = GLFW_RELEASE;
    
    double cursorX = 0.0f;
    double cursorY = 0.0f;

    int mouseRight      = GLFW_RELEASE;
    int mouseLeft       = GLFW_RELEASE;
    int mouseScroll     = 0;
};

static INPUT input;

/*class Input {
public:
    //static INPUT inputs;

private:

};*/

#endif // !INPUT_H