#ifndef OBJDATA_H
#define OBJDATA_H

#include "ObjType.h"

#include "glm/glm.hpp"

class ObjData {
public:

	ObjType objType;
	float xOffset;
	float zOffset;
	float yRotation;

	ObjData();
	ObjData(ObjType const objType, float const xOffset, float const zOffset, float const yRotation);
	~ObjData();

private:

};

#endif // !OBJDATA_H
