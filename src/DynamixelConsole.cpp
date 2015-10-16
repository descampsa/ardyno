#include "DynamixelConsole.h"


const DynamixelCommand DynamixelConsole::sCommand[] =
	{};

DynamixelConsole::DynamixelConsole(DynamixelInterface &aInterface, Stream &aConsole):
	mInterface(aInterface), mConsole(aConsole)
{
	mLinePtr=&(mLineBuffer[0]);
}

void DynamixelConsole::loop()
{
	//empty input buffer
	while(mConsole.available())
		mConsole.read();
	
	//write new command prompt
	mConsole.write(">");
	
	// read one command line
	char c;
	while((c=mConsole.read())!='\n')
	{
		if(c>0 && (mLinePtr-&(mLineBuffer[0]))<sLineBufferSize)
		{
			mConsole.write(c);
			*mLinePtr=c;
			++mLinePtr;
		}
	}
	//new line
	mConsole.write("\n\r");
	// run command
	run();
	// reset buffer
	mLinePtr=&(mLineBuffer[0]);
}

void DynamixelConsole::run()
{
	char *argv[16];
	int argc=parseCmd(argv);
	
	const int commandNumber=sizeof(sCommand)/sizeof(DynamixelCommand);
	for(int i=0; i<commandNumber; ++i)
	{
		if(strcmp(argv[0],sCommand[i].mName)==0)
		{
			sCommand[i].mCallback(argc, argv);
			break;
		}
	}
}


int DynamixelConsole::parseCmd(char **argv)
{
	int argc=0;
	char *ptr=&mLineBuffer[0];
	while(argc<15)
	{
		while(*ptr==' ' && ptr<mLinePtr)
		{
			++ptr;
		}
		if(ptr==mLinePtr)
		{
			break;
		}
		argv[argc]=ptr;
		while(*ptr!=' ' && ptr<mLinePtr)
		{
			++ptr;
		}
		*ptr='\0';
		++ptr;
		++argc;
	}
	argv[argc]=0;
	return argc;
}

