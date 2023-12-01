/**
 * @file main.cpp
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Starting point into the program, includes the main function which creates and starts an Application, which handles the game loop.
 * @version 0.1
 * @date 2020-05-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Application.h"
#include "ThreadPool.h"

#include <iostream>

/**
 * @brief main function which creates and starts an Application, any exceptions are caught here.
 * 
 * @return int the exit status of the program
 */
int main() {
	ThreadPool::getInstance().start();

	Application application;

	try {
		application.start();
	}
	catch (const std::exception &exception) {
		std::cerr << exception.what() << std::endl;
		
		ThreadPool::getInstance().stop();

		system("pause");

		return EXIT_FAILURE;
	}

	ThreadPool::getInstance().stop();

	return EXIT_SUCCESS;
}