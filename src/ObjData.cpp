#include "ObjData.h"

ObjData::ObjData()
	:objType(ObjType::EMPTY), xOffset(0.0f), zOffset(0.0f), yRotation(0.0f) {}

ObjData::ObjData(ObjType const objType, float const xOffset, float const zOffset, float const yRotation)
	:objType(objType), xOffset(xOffset), zOffset(zOffset), yRotation(yRotation) {}

ObjData::~ObjData() {}
