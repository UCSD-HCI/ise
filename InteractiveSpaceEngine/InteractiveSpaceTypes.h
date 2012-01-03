#ifndef INTERACTIVE_SPACE_TYPES
#define INTERACTIVE_SPACE_TYPES

struct IntPoint3D
{
	int x, y, z;
	IntPoint3D() { }
	IntPoint3D(int x, int y, int z) : x(x), y(y), z(z) { }
};

struct FloatPoint3D
{
	float x, y, z;
	FloatPoint3D() { }
	FloatPoint3D(float x, float y, float z) : x(x), y(y), z(z) { }
	FloatPoint3D(const IntPoint3D& ref) : x(ref.x), y(ref.y), z(ref.z) { }

	double squaredDistanceTo(const FloatPoint3D& ref) const
	{
		return (x - ref.x) * (x - ref.x) + (y - ref.y) * (y - ref.y) + (z - ref.z) * (z - ref.z);
	}
};

struct DoublePoint3D
{
	double x, y, z;
	DoublePoint3D() { }
	DoublePoint3D(double x, double y, double z) : x(x), y(y), z(z) { }
	DoublePoint3D(const IntPoint3D& ref) : x(ref.x), y(ref.y), z(ref.z) { }
	DoublePoint3D(const FloatPoint3D& ref) : x(ref.x), y(ref.y), z(ref.z) { }

	operator FloatPoint3D()
	{
		return FloatPoint3D((float)x, (float)y, (float)z);
	}
};

typedef void (*Callback)();

typedef void (*RGBCalibrationFinishedCallback) (FloatPoint3D* checkPoints, int checkPointNum, FloatPoint3D* depthRefCorners, int depthRefCornerNum);

#endif