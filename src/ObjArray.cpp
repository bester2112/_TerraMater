#include "ObjArray.h"
#include "ObjType.h"

ObjArray::ObjArray() {
	objs[ObjType::EMPTY] = Obj(Settings::SPHERE_OBJ_PATH);
	objs[ObjType::SPHERE] = Obj(Settings::SPHERE_OBJ_PATH);
	objs[ObjType::SUNFLOWER] = Obj(Settings::SUNFLOWER_OBJ_PATH);
	objs[ObjType::LOTUS] = Obj(Settings::LOTUS_OBJ_PATH);
	objs[ObjType::SUCCULENT] = Obj(Settings::SUCCULENT_OBJ_PATH);
}

ObjArray::~ObjArray() {}
