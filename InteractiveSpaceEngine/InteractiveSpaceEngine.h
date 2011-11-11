#ifndef INTERACTIVE_SPACE_ENGINE_H
#define INTERACTIVE_SPACE_ENGINE_H

class InteractiveSpaceEngine
{
private:
public:
	static InteractiveSpaceEngine* sharedEngine();
	void run();
	void stop();
};

#endif