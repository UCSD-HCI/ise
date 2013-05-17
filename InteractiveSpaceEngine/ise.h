#ifndef ISE_H_
#define ISE_H_

class KinectSensor;
class ImageProcessingFactory;
class OmniTouchFingerTracker;
class FingerSelector;
class FingerEventsGenerator;
class HandTracker;
class Calibrator;
class VideoRecorder;
class TuioExporter;
class Timer;

struct IntPoint3D;
struct FloatPoint3D;
struct DoublePoint3D;
struct Quadrilateral;
struct Matrix33;

typedef void (*Callback)();
typedef void (*ViscaCommandCallback)(bool isCommandCompleted, void* state);
typedef void (*RGBCalibrationFinishedCallback) (FloatPoint3D* checkPoints, int checkPointNum, FloatPoint3D* depthRefCorners, int depthRefCornerNum);


#endif
