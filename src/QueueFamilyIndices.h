/**
 * @file QueueFamilyIndices.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief This contains a data class, to save the indices of the choosen queue families.
 * @version 0.1
 * @date 2020-05-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef QUEUEFAMILYINDICES_H
#define QUEUEFAMILYINDICES_H

#include <stdint.h>

/**
 * @brief Data class to store the indices of the choosen queue families, also checks if all indices are set.
 */
class QueueFamilyIndices {
public:
	uint32_t graphicsFamilyIndex;
	uint32_t presentFamilyIndex;
	uint32_t transferFamilyIndex;
	uint32_t computeFamilyIndex;

	bool hasGraphicsFamily = false;
	bool hasPresentFamily = false;
	bool hasTransferFamily = false;
	bool hasComputeFamily = false;

	QueueFamilyIndices();
	~QueueFamilyIndices();

	bool hasAllIndices() const;

	QueueFamilyIndices &operator+(QueueFamilyIndices const &queuFamilyIndices);
};

#endif // !QUEUEFAMILYINDICES_H