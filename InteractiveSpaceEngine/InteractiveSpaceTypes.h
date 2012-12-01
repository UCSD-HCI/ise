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

	operator IntPoint3D()
	{
		return IntPoint3D((int)x, (int)y, (int)z);
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

struct Quadrilateral
{
	FloatPoint3D p1, p2, p3, p4;
	Quadrilateral() { }
	Quadrilateral(const FloatPoint3D& p1, const FloatPoint3D& p2, const FloatPoint3D& p3, const FloatPoint3D& p4) : p1(p1), p2(p2), p3(p3), p4(p4) { }
	Quadrilateral(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) : p1(x1,y1,0), p2(x2,y2,0), p3(x3,y3,0), p4(x4,y4,0) { }

	FloatPoint3D getCenter() const
	{
		return FloatPoint3D( (p1.x + p2.x + p3.x + p4.x) / 4.0f, (p1.y + p2.y + p3.y + p4.y) / 4.0f, (p1.z + p2.z + p3.z + p4.z) / 4.0f );
	}
};

struct Matrix33
{
	double m11, m12, m13, m21, m22, m23, m31, m32, m33;

	Matrix33(double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33) : 
		m11(m11), m12(m12), m13(m13), m21(m21), m22(m22), m23(m23), m31(m31), m32(m32), m33(m33) 
		{ }
};

typedef void (*Callback)();
typedef void (*ViscaCommandCallback)(bool isCommandCompleted, void* state);
typedef void (*RGBCalibrationFinishedCallback) (FloatPoint3D* checkPoints, int checkPointNum, FloatPoint3D* depthRefCorners, int depthRefCornerNum);
typedef void (*WebcamCalibrationFinishedCallback) (FloatPoint3D* checkPoints, int checkPointNum);
typedef void (*DocumentAddedCallback) (char* documentTag);

#endif