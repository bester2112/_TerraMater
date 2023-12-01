#ifndef OBJARRAY_H
#define OBJARRAY_H

#include "Settings.h"
#include "Obj.h"

class ObjArray {
public:
	ObjArray();
	~ObjArray();

	Obj objs[Settings::OBJ_COUNT];

private:

};

#endif // !OBJARRAY_H
