#include "InputHandler.h"
#include <string>
#include "Settings.h"
#include "ComputeSettings.h"

char InputHandler::KEY_W = GLFW_RELEASE;
char InputHandler::KEY_A = GLFW_RELEASE;
char InputHandler::KEY_S = GLFW_RELEASE;
char InputHandler::KEY_D = GLFW_RELEASE;
char InputHandler::KEY_UP = GLFW_RELEASE;
char InputHandler::KEY_DOWN = GLFW_RELEASE;
char InputHandler::KEY_LEFT = GLFW_RELEASE;
char InputHandler::KEY_RIGHT = GLFW_RELEASE;
char InputHandler::KEY_SPACE = GLFW_RELEASE;
char InputHandler::KEY_NUMBER = GLFW_RELEASE;
char InputHandler::KEY_NUMBER_ACTION = GLFW_RELEASE;
char InputHandler::KEY_PLUS = GLFW_RELEASE;
char InputHandler::KEY_MINUS = GLFW_RELEASE;
char InputHandler::CURSOR_IN_SCREEN = GLFW_FALSE;

char InputHandler::MOUSE_BUTTON_RIGHT = GLFW_RELEASE;
char InputHandler::MOUSE_BUTTON_LEFT = GLFW_RELEASE;

char InputHandler::MOUSE_SCROLL = 0;

double InputHandler::CURSOR_X = -1;
double InputHandler::CURSOR_Y = -1;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void cursor_enter_callback(GLFWwindow * window, int entered);
void cursor_scroll_callback(GLFWwindow * window, double xOffset, double yOffset);


InputHandler::InputHandler() {
	
}

InputHandler::~InputHandler() {}

void InputHandler::initialize(GLFWwindow* window, INPUT *input) {
	initVariables();

	glfwSetKeyCallback(window, key_callback);					// set the key strokes
	glfwSetMouseButtonCallback(window, mouse_button_callback);	// sets the callback function for mouse button events
	glfwSetCursorPosCallback(window, cursor_position_callback); // sets the callback function for the position of the mouse
	glfwSetCursorEnterCallback(window, cursor_enter_callback);	// sets the callback function for entering / leaving the window with the cursor
	glfwSetScrollCallback(window, cursor_scroll_callback);		// sets the callback function for scrolling with the mouse

}

void InputHandler::initVariables() {
	//cursorX = 0;
	//cursorY = 0;
}

void InputHandler::terminate() {
	glfwTerminate();
}

int i = 0;

void InputHandler::update(GLFWwindow* window, INPUT *in)
{
	getInputValues(in);
	i++;
	if (i > 150000) {
		/*std::cout << " InputHandler -> " << std::endl;
		std::cout << " input.walkForwards " << in->walkForwards << std::endl;
		std::cout << " input.walkBackwards " << in->walkBackwards << std::endl;
		std::cout << " input.strafeRight " << in->strafeRight << std::endl;
		std::cout << " input.strafeLeft " << in->strafeLeft << std::endl;
		std::cout << " input.isSpacePressed " << in->jump << std::endl;
		std::cout << " input.numberPressed " << in->numberPressed << std::endl;
		std::cout << " input.numberPressedAction " << in->numberPressedAction << std::endl;
		std::cout << " input.cursorX " << in->cursorX << std::endl;
		std::cout << " input.cursorY " << in->cursorY << std::endl;
		std::cout << " input.leftBackwards " << in->leftBackwards << std::endl;
		std::cout << " input.rightBackwards " << in->rightBackwards << std::endl;
		std::cout << " input.leftForwards " << in->leftForwards << std::endl;
		std::cout << " input.rightForwards " << in->rightForwards << std::endl;
		std::cout << " input.mouseLeft " << in->mouseLeft << std::endl;
		std::cout << " input.mouseRight " << in->mouseRight << std::endl;
		std::cout << " input.mouseScroll " << in->mouseScroll << std::endl;*/
		i = 0;
	}
}


/*
 * @brief callback function for the mouse button events
 */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		InputHandler::MOUSE_BUTTON_RIGHT = action;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		InputHandler::MOUSE_BUTTON_LEFT = action;
	}

	if (button != GLFW_MOUSE_BUTTON_LEFT) {
		return;
	}

	if (action == GLFW_PRESS) {
		std::cout << "Left button pressed" << std::endl;

		//double xPos, yPos; // x y Position

		//  changes the mode of the cursor in the window
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // GLFW_CURSOR_DISABLED, GLFW_CURSOR_HIDDEN, GLFW_CURSOR_NORMAL
		//glfwGetCursorPos(window, &xPos, &yPos);

		//InputHandler::Instance().addPosition(glm::vec2(xPos, yPos));
	}

	if (action == GLFW_RELEASE) {
		std::cout << "Left button RELEASED" << std::endl;
	}
}

/*
 * @brief callback funktion for the cursor motion events
 */
void cursor_position_callback(GLFWwindow* window, double x, double y) {
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
		InputHandler::CURSOR_X = x;
		InputHandler::CURSOR_Y = y;
	}
}

/*
 * @brief callback funktion if the cursor entered / left the Window
 */
void cursor_enter_callback(GLFWwindow* window, int entered) {
	InputHandler::CURSOR_IN_SCREEN = entered;
	//InputHandler::Instance().windowEntered(entered);
	if (entered) {
		std::cout << " InputHandler : Cursor ENTERED the window" << std::endl;
	}
	else {
		std::cout << " InputHandler : Cursor LEFT the window" << std::endl;
	}
}

/*
 * @brief callback funktion for scrolling with the mouse
 */
void cursor_scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	InputHandler::MOUSE_SCROLL = yOffset;
	std::cout << " InputHandler : scroll xOffset: " << xOffset << "\t yOffset: " << yOffset << std::endl;
}

/*
 * @brief handling Key Strokes
 */
 // GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	std::string actionName = "";

	switch (action) {
	case GLFW_PRESS:
		actionName = "GLFW_PRESS";
		break;
	case GLFW_RELEASE:
		actionName = "GLFW_RELEASE";
		break;
	case GLFW_REPEAT:
		actionName = "GLFW_REPEAT";
		action = GLFW_PRESS;
		break;
	default:
		actionName = "This case shouldn't happen";
		break;
	}

	//std::cout << " KEY : " << key << " Action : " << actionName << " action : " << action << " scancode: " << scancode << " mods: " << mods << std::endl;

	switch (scancode) {
	case 27: // KEY 93
	case 78: // KEY 334
		// PLUS
		InputHandler::KEY_PLUS = action;
		break;
	case 53: // KEY 47
	case 74: // KEY 333
		// MINUS
		InputHandler::KEY_MINUS = action;
		break;
	default:
		break;
	}

	//if (action == GLFW_PRESS) {
		// handles the input of ECS, W-A-S-D and UP-LEFT-DOWN-RIGHT Keys 
	switch (key)
	{
	case GLFW_KEY_ESCAPE: 	// if the escape key is pressed the window will get closed
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_W:
		//std::cout << " KEY W Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_W = action;
		break;
	case GLFW_KEY_UP:
		//std::cout << " KEY ^ Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_UP = action;
		break;
	case GLFW_KEY_S:
		//std::cout << " KEY S Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_S = action;
		break;
	case GLFW_KEY_DOWN:
		//std::cout << " KEY V Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_DOWN = action;
		break;
	case GLFW_KEY_A:
		//std::cout << " KEY A Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_A = action;
		break;
	case GLFW_KEY_LEFT:
		//std::cout << " KEY <- Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_LEFT = action;
		break;
	case GLFW_KEY_D:
		//std::cout << " KEY D Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_D = action;
		break;
	case GLFW_KEY_RIGHT:
		//std::cout << " KEY -> Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_RIGHT = action;
		break;
	case GLFW_KEY_SPACE:
		//std::cout << " KEY SPACE Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_SPACE = action;
		break;
	case GLFW_KEY_1:
		//std::cout << " KEY 1 Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_NUMBER = 1;
		InputHandler::KEY_NUMBER_ACTION = action;
		break;
	case GLFW_KEY_2:
		//std::cout << " KEY 2 Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_NUMBER = 2;
		InputHandler::KEY_NUMBER_ACTION = action;
		break;
	case GLFW_KEY_3:
		//std::cout << " KEY 3 Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_NUMBER = 3;
		InputHandler::KEY_NUMBER_ACTION = action;
		break;
	case GLFW_KEY_4:
		//std::cout << " KEY 4 Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_NUMBER = 4;
		InputHandler::KEY_NUMBER_ACTION = action;
		break;
	case GLFW_KEY_5:
		//std::cout << " KEY 5 Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_NUMBER = 5;
		InputHandler::KEY_NUMBER_ACTION = action;
		break;
	case GLFW_KEY_6:
		//std::cout << " KEY 6 Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_NUMBER = 6;
		InputHandler::KEY_NUMBER_ACTION = action;
		break;
	case GLFW_KEY_7:
		//std::cout << " KEY 7 Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_NUMBER = 7;
		InputHandler::KEY_NUMBER_ACTION = action;
		break;
	case GLFW_KEY_8:
		//std::cout << " KEY 8 Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_NUMBER = 8;
		InputHandler::KEY_NUMBER_ACTION = action;
		break;
	case GLFW_KEY_9:
		//std::cout << " KEY 9 Action : " << actionName << " " << action << std::endl;
		InputHandler::KEY_NUMBER = 9;
		InputHandler::KEY_NUMBER_ACTION = action;
		break;
	case GLFW_KEY_P:
		if (action == GLFW_PRESS) {
			Settings::IN_PHOTO_MODE = !Settings::IN_PHOTO_MODE;
		}
		break;
	case GLFW_KEY_F:
		if (action == GLFW_PRESS) {
			Settings::UPDATE_FRUSTUM = !Settings::UPDATE_FRUSTUM;
		}
		break;
	case GLFW_KEY_I:
		if (action == GLFW_PRESS) {
			ComputeSettings::iData.x = (ComputeSettings::iData.x + 1) % ComputeSettings::integratorCount;
		}
		break;
	case GLFW_KEY_K:
		if (action == GLFW_PRESS) {
			ComputeSettings::iData.x = (ComputeSettings::iData.x - 1 < 0) ? ComputeSettings::integratorCount - 1 : ComputeSettings::iData.x - 1;
		}
		break;
	case GLFW_KEY_Y:
		if (action == GLFW_PRESS) {
			ComputeSettings::iData.z = ComputeSettings::iData.z + 1;
		}
		break;
	case GLFW_KEY_H:
		if (action == GLFW_PRESS) {
			ComputeSettings::iData.z = ComputeSettings::iData.z - 1;
			if (ComputeSettings::iData.z <= 0) {
				ComputeSettings::iData.z = 1;
			}
		}
		break;
	case GLFW_KEY_U:
		if (action == GLFW_PRESS) {
			ComputeSettings::iData.y = (ComputeSettings::iData.y + 1) % ComputeSettings::cameraCount;
		}
		break;
	case GLFW_KEY_J:
		if (action == GLFW_PRESS) {
			ComputeSettings::iData.y = (ComputeSettings::iData.y - 1 < 0) ? ComputeSettings::cameraCount - 1 : ComputeSettings::iData.y - 1;
		}
		break;
	case GLFW_KEY_T:
		if (action == GLFW_PRESS) {
			ComputeSettings::benchmarkStatus = BenchmarkStatus::TIME;
		}
		break;
	case GLFW_KEY_G:
		if (action == GLFW_PRESS) {
			ComputeSettings::benchmarkStatus = BenchmarkStatus::ITERATION;
		}
		break;
	case GLFW_KEY_B:
		if (action == GLFW_PRESS) {
			ComputeSettings::benchmarkStatus = BenchmarkStatus::OFF;
		}
		break;
	default:
		//std::cout << " KEY Action : " << actionName << " action : " << action << " scancode: " << scancode << " mods: " << mods << std::endl;
		break;
	}
	//}
}


void InputHandler::setWalk(char key1, char key2, int* walk) {
	if (key1 || key2) {
		*walk = GLFW_PRESS;
	}
	else {
		*walk = GLFW_RELEASE;
	}
}

void InputHandler::getInputValues(INPUT* inp) {
	inp->numberPressed = KEY_NUMBER;
	inp->numberPressedAction = KEY_NUMBER_ACTION;

	/*// W & UP
	if (KEY_W || KEY_UP) {
		inp->walkForwards = GLFW_PRESS;
	} else {
		inp->walkForwards = GLFW_RELEASE;
	}

	// S & DOWN
	if (KEY_S || KEY_DOWN) {
		inp->walkBackwards = GLFW_PRESS;
	} else {
		inp->walkBackwards = GLFW_RELEASE;
	}

	// A & LEFT
	if (KEY_A || KEY_LEFT) {
		inp->strafeLeft = GLFW_PRESS;
	} else {
		inp->strafeLeft = GLFW_RELEASE;
	}

	// D & RIGHT
	if (KEY_D || KEY_RIGHT) {
		inp->strafeRight = GLFW_PRESS;
	}
	else {
		inp->strafeRight = GLFW_RELEASE;
	}*/

	// Same result like the 4 if before
	setWalk(KEY_W, KEY_UP, &inp->walkForwards);
	setWalk(KEY_S, KEY_DOWN, &inp->walkBackwards);
	setWalk(KEY_A, KEY_LEFT, &inp->strafeLeft);
	setWalk(KEY_D, KEY_RIGHT, &inp->strafeRight);

	// left & right => stop
	if (inp->strafeLeft && inp->strafeRight) {
		inp->strafeLeft = GLFW_RELEASE;
		inp->strafeRight = GLFW_RELEASE;
	}

	// forward & back => stop
	if (inp->walkForwards && inp->walkBackwards) {
		inp->walkForwards = GLFW_RELEASE;
		inp->walkBackwards = GLFW_RELEASE;
	}

	// left & forward
	if (inp->strafeLeft && inp->walkForwards) {
		inp->leftForwards = GLFW_PRESS;
		inp->strafeLeft = GLFW_RELEASE;
		inp->walkForwards = GLFW_RELEASE;
	} else {
		inp->leftForwards = GLFW_RELEASE;
	}

	// right & forward
	if (inp->strafeRight && inp->walkForwards) {
		inp->rightForwards = GLFW_PRESS;
		inp->strafeRight = GLFW_RELEASE;
		inp->walkForwards = GLFW_RELEASE;
	} else {
		inp->rightForwards = GLFW_RELEASE;
	}

	// left & back
	if (inp->strafeLeft && inp->walkBackwards) {
		inp->leftBackwards = GLFW_PRESS;
		inp->strafeLeft = GLFW_RELEASE;
		inp->walkBackwards = GLFW_RELEASE;
	} else {
		inp->leftBackwards = GLFW_RELEASE;
	}

	// right & back
	if (inp->strafeRight && inp->walkBackwards) {
		inp->rightBackwards = GLFW_PRESS;
		inp->strafeRight = GLFW_RELEASE;
		inp->walkBackwards = GLFW_RELEASE;
	} else {
		inp->rightBackwards = GLFW_RELEASE;
	}

	inp->jump = KEY_SPACE;
	if (CURSOR_IN_SCREEN) {
		inp->cursorX = CURSOR_X;
		inp->cursorY = CURSOR_Y;
	}

	inp->plus = KEY_PLUS;
	inp->minus = KEY_MINUS;

	inp->mouseLeft = MOUSE_BUTTON_LEFT;
	inp->mouseRight = MOUSE_BUTTON_RIGHT;
	inp->mouseScroll = MOUSE_SCROLL;
}