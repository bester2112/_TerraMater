#include "Camera.h"

Camera::Camera()
	:view({ 0,0,0,0 }), projection({ 0,0,0,0 }), cameraPosition({ 0.0f, 100.0f, 3.0f }), cameraUp({ 0.0f, 1.0f, 0.0f }), cameraFront({ 0.0f, 0.0f, -1.0f }),
	 firstTime(true), yaw( -90.0f ), pitch( 0.0f ), fov( 45.0f ), lastX( Settings::WINDOW_WIDTH / 2.0f ), lastY( Settings::WINDOW_HEIGHT / 2.0 ) {}

Camera::~Camera()
{
}

void Camera::initialize(GLFWwindow* window, INPUT* input)
{	
	view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}

int k = 0;

void Camera::update(GLFWwindow* window, INPUT* in)
{
	k++;
	//this->input = *in;
	if (k > 150000) {

		/*std::cout << " Camera -> " << std::endl;
		std::cout << " in->walkForwards " << in->walkForwards << std::endl;
		std::cout << " in->walkBackwards " << in->walkBackwards << std::endl;
		std::cout << " in->strafeRight " << in->strafeRight << std::endl;
		std::cout << " in->strafeLeft " << in->strafeLeft << std::endl;
		std::cout << " in->isSpacePressed " << in->jump << std::endl;
		std::cout << " in->numberPressed " << in->numberPressed << std::endl;
		std::cout << " in->numberPressedAction " << in->numberPressedAction << std::endl;
		std::cout << " in->cursorX " << in->cursorX << std::endl;
		std::cout << " in->cursorY " << in->cursorY << std::endl;
		std::cout << " in->leftBackwards " << in->leftBackwards << std::endl;
		std::cout << " in->rightBackwards " << in->rightBackwards << std::endl;
		std::cout << " in->leftForwards " << in->leftForwards << std::endl;
		std::cout << " in->rightForwards " << in->rightForwards << std::endl;
		std::cout << " in->mouseLeft " << in->mouseLeft << std::endl;
		std::cout << " in->mouseRight " << in->mouseRight << std::endl;
		std::cout << " in->mouseScroll " << in->mouseScroll << std::endl;*/

		k = 0;
	}

	moveCamera(window, in);
	cursorMovement(window, in);

	if (Settings::WINDOW_WIDTH != 0 && Settings::WINDOW_HEIGHT != 0) {
		projection = glm::perspective(glm::radians(fov), (float)Settings::WINDOW_WIDTH / (float)Settings::WINDOW_HEIGHT, 0.1f, 10000.0f);
	}

	// camera / view transformation
	view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

}

glm::mat4 Camera::getView()
{
	return this->view;
}

glm::mat4 Camera::getProjection()
{
	return this->projection;
}

glm::vec3 Camera::getCameraPosition()
{
	return this->cameraPosition;
}

void Camera::cursorMovement(GLFWwindow* window, INPUT* input) {
	if (firstTime && input->cursorX == -1 && input->cursorY == -1) {
		return;
	}
	if (firstTime) {
		lastX = input->cursorX;
		lastY = input->cursorY;
		firstTime = false;
	}

	double xOffset = input->cursorX - lastX;
	double yOffset = lastY - input->cursorY;
	lastX = input->cursorX;
	lastY = input->cursorY;

	const float sensitivity = 0.05f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraFront = glm::normalize(direction);
}

void Camera::moveCamera(GLFWwindow* window, INPUT* in)
{
	switch (in->walkForwards) {
		case GLFW_PRESS:
			cameraPosition += cameraSpeed * cameraFront;
			break;
		case GLFW_RELEASE:
			break;
		default: // not available
			break;
	}

	switch (in->walkBackwards) {
		case GLFW_PRESS:
			cameraPosition -= cameraSpeed * cameraFront;
			break;
		case GLFW_RELEASE:
			break;
		default: // not available
			break;
	}

	switch (in->strafeRight) {
		case GLFW_PRESS:
			cameraPosition += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
			break;
		case GLFW_RELEASE:
			break;
		default: // not available
			break;
	}

	switch (in->strafeLeft) {
		case GLFW_PRESS:
			cameraPosition -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
			break;
		case GLFW_RELEASE:
			break;
		default: // not available
			break;
	}

	switch (in->leftForwards) {
	case GLFW_PRESS:
		cameraPosition -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp) - cameraFront);
		break;
	case GLFW_RELEASE:
		break;
	default: // not available
		break;
	}

	switch (in->rightForwards) {
	case GLFW_PRESS:
		cameraPosition += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp) + cameraFront);
		break;
	case GLFW_RELEASE:
		break;
	default: // not available
		break;
	}

	switch (in->leftBackwards) {
	case GLFW_PRESS:
		cameraPosition += cameraSpeed * glm::normalize(glm::cross((-cameraFront), cameraUp) - cameraFront);
		break;
	case GLFW_RELEASE:
		break;
	default: // not available
		break;
	}

	switch (in->rightBackwards) {
	case GLFW_PRESS:
		cameraPosition += cameraSpeed * glm::normalize(glm::cross((cameraFront), cameraUp) - cameraFront);
		break;
	case GLFW_RELEASE:
		break;
	default: // not available
		break;
	}

	switch (in->jump) {
	case GLFW_PRESS:
		cameraPosition += cameraSpeed * cameraUp;
		break;
	case GLFW_RELEASE:
		break;
	default: // not available
		break;
	}

	switch (in->plus) {
	case GLFW_PRESS:
		fov += 0.1f;
		std::cout << "fov : " << fov << std::endl;
		break;
	case GLFW_RELEASE:
		break;
	default: // not available
		break;
	}

	switch (in->minus) {
	case GLFW_PRESS:
		fov -= 0.1f;
		std::cout << "fov : " << fov << std::endl;
		break;
	case GLFW_RELEASE:
		break;
	default: // not available
		break;
	}
}

