#ifndef DYNAMIXEL_CONSOLE_H
#define DYNAMIXEL_CONSOLE_H

#include "Dynamixel.h"


struct DynamixelCommand
{
	typedef void (*FunPtr)(int argc, char **argv);
	const char *mName;
	//const char *mDescription;
	FunPtr mCallback;
};


class DynamixelConsole
{
	public:
	
	DynamixelConsole(DynamixelInterface &aInterface, Stream &aConsole);
	
	void loop();
	
	private:
	
	void run();
	int parseCmd(char **argv);
	
	const static size_t sLineBufferSize=256;
	char mLineBuffer[sLineBufferSize];
	char *mLinePtr;
	DynamixelInterface &mInterface;
	Stream &mConsole;
	
	const static DynamixelCommand sCommand[];
};

#endif
