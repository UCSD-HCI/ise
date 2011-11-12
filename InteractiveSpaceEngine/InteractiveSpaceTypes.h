#ifndef INTERACTIVE_SPACE_TYPES
#define INTERACTIVE_SPACE_TYPES

#include <XnCppWrapper.h>

struct IntPoint3D
{
	int x, y, z;
	IntPoint3D() { }
	IntPoint3D(int x, int y, int z) : x(x), y(y), z(z) { }
	
	operator XnPoint3D()
	{
		XnPoint3D r;
		r.X = x;
		r.Y = y;
		r.Z = z;
		return r;
	}
};

struct FloatPoint3D
{
	float x, y, z;
	FloatPoint3D() { }
	FloatPoint3D(float x, float y, float z) : x(x), y(y), z(z) { }
	FloatPoint3D(const XnPoint3D& ref) : x(ref.X), y(ref.Y), z(ref.Z) { }
	
	operator XnPoint3D()
	{
		XnPoint3D r;
		r.X = x;
		r.Y = y;
		r.Z = z;
		return r;
	}
};

#endif