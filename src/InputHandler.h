/**
 * @file InputHandler.h
 * @author Thomas Rzepka (uqdll@student.kit.edu)
 * @brief Includes the InputHandler class which manages the input from the user with GLFW.
 * @version 0.1
 * @date 2020-05-03
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#if defined(_MSC_VER)
 // Make MS math.h define M_PI
#define _USE_MATH_DEFINES
#endif

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "Input.h"
#include <iostream>

 /**
  * @brief The InputHandler handles all the Input from Mouse, Cursor and Keys from the Keyboard
  */
    class InputHandler {
    public:
        InputHandler();
        ~InputHandler();

        // saved status of the keys
        static char KEY_W;
        static char KEY_A;
        static char KEY_S;
        static char KEY_D;
        static char KEY_UP;
        static char KEY_DOWN;
        static char KEY_LEFT;
        static char KEY_RIGHT;
        static char KEY_SPACE;
        static char KEY_NUMBER;
        static char KEY_NUMBER_ACTION;
        static char KEY_PLUS;
        static char KEY_MINUS;

        // position of the current cursor
        static double CURSOR_X;
        static double CURSOR_Y;
        /*
         * @brief saves if the cursor is actual in screen
         */
        static char CURSOR_IN_SCREEN;
        static char MOUSE_BUTTON_RIGHT;
        static char MOUSE_BUTTON_LEFT;
        static char MOUSE_SCROLL;

        /*
         * @brief updates the necessary input functions, that are needed
         */
        void update(GLFWwindow* window, INPUT* input);

        /**
         * @brief initialisation of GLFW
         */
        void initialize(GLFWwindow* window, INPUT *input);

    private:
        //static bool initialized;

        
        /**
         * @brief terminates all initinialized things, before the app exits
         */
        void terminate();

        /**
         * @brief sets variables that are neccessary for this class
         */
        void initVariables();

        /*
         * @brief returns the Input from all key inputs
         */
        void getInputValues(INPUT* inp);

        /*
         * @brief sets the direction if one of the keys is pressed
         */
        void setWalk(char key1, char key2, int* walk);
};

#endif // INPUTHANDLER_H