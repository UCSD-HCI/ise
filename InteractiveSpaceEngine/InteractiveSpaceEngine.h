#ifndef INTERACTIVE_SPACE_ENGINE_H
#define INTERACTIVE_SPACE_ENGINE_H

#include "ise.h"
#include <memory>

typedef enum
{
    ISE_FLAG_ENABLED = 0x1,
    ISE_DEBUG_WINDOW = 0x2,
    ISE_COLOR_MODEL = 0x4,
    //The values above should be the same as ise::DetectorFlags defined in InteractiveSpaceCvLib/DataTypes.h

    ISE_STABLIZE = 0x8,
    ISE_OMNI_TOUCH = 0x10,
    ISE_RECTIFIED_TABLETOP = 0x20
} InteractiveSpaceEngineFlags;

class InteractiveSpaceEngine
{
private:
    std::unique_ptr<KinectSensor> kinectSensor;
	std::unique_ptr<ImageProcessingFactory> ipf;
	
	std::unique_ptr<OmniTouchFingerTracker> omniTracker;
	std::unique_ptr<FingerSelector> fingerSelector;
	std::unique_ptr<FingerEventsGenerator> fingerEventsGenerator;
	std::unique_ptr<HandTracker> handTracker;

	std::unique_ptr<Calibrator> calibrator;

	std::unique_ptr<VideoRecorder> videoRecorder;

	std::unique_ptr<TuioExporter> tuioExporter;

	long long kinectSensorFrameCount;
	volatile long long engineFrameCount;
	
	int fpsCounter;
	volatile float fps;
    std::unique_ptr<Timer> fpsTimer;

	Callback engineUpdateCallback;
    
    unsigned int flags;

public:
	InteractiveSpaceEngine();
	virtual ~InteractiveSpaceEngine();

    static std::unique_ptr<InteractiveSpaceEngine> instance;

    inline static InteractiveSpaceEngine* sharedEngine()
    {
        return instance.get();
    }

    inline static void sharedEngineInit()
    {
        instance.reset(new InteractiveSpaceEngine());
    }

    inline static void sharedEngineDispose()
    {
        instance.reset();
    }
    
	void mainLoopUpdate();

    inline KinectSensor* getKinectSensor() { return kinectSensor.get(); }
	inline ImageProcessingFactory* getImageProcessingFactory() {return ipf.get(); }
	inline OmniTouchFingerTracker* getOmniTouchFingerTracker() { return omniTracker.get(); }
	inline FingerSelector* getFingerSelector() { return fingerSelector.get(); }
	inline HandTracker* getHandTracker() { return handTracker.get(); }
	inline Calibrator* getCalibrator() { return calibrator.get(); }
	inline FingerEventsGenerator* getFingerEventsGenerator() { return fingerEventsGenerator.get(); }
	inline VideoRecorder* getVideoRecorder() { return videoRecorder.get(); }
	inline TuioExporter* getTuioExporter() { return tuioExporter.get(); }

	inline long long getFrameCount() 
	{
		return engineFrameCount; 
	}
	inline float getFPS()
	{
		return fps; 
	}

	inline void setEngineUpdateCallback(Callback callback)
	{
		engineUpdateCallback = callback;
	}

    inline unsigned int getFlags() const
    {
        return flags;
    }

    inline void setFlags(InteractiveSpaceEngineFlags flags)
    {
        setFlags((unsigned int)flags);
    }

    void setFlags(unsigned int flags);
    void clearFlags(InteractiveSpaceEngineFlags flags);

    bool isFlagEnabled(InteractiveSpaceEngineFlags flag) const
    {
        return (this->flags & flag) != 0;
    }
};

#endif
