#include "QueueFamilyIndices.h"

QueueFamilyIndices::QueueFamilyIndices()
	:graphicsFamilyIndex(0), presentFamilyIndex(0), transferFamilyIndex(0) {}

QueueFamilyIndices::~QueueFamilyIndices() {}

bool QueueFamilyIndices::hasAllIndices() const {
	return hasGraphicsFamily && hasPresentFamily && hasTransferFamily && hasComputeFamily;
}

QueueFamilyIndices &QueueFamilyIndices::operator+(QueueFamilyIndices const &queueFamilyIndices) {
	graphicsFamilyIndex = queueFamilyIndices.graphicsFamilyIndex;
	presentFamilyIndex = queueFamilyIndices.presentFamilyIndex;
	transferFamilyIndex = queueFamilyIndices.transferFamilyIndex;
	computeFamilyIndex = queueFamilyIndices.computeFamilyIndex;

	hasGraphicsFamily = queueFamilyIndices.hasGraphicsFamily;
	hasPresentFamily = queueFamilyIndices.hasPresentFamily;
	hasTransferFamily = queueFamilyIndices.transferFamilyIndex;
	hasComputeFamily = queueFamilyIndices.hasComputeFamily;

	return *this;
}
