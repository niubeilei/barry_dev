////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ConfigReader.cpp
// Description:
//   
//
// Modification History:
// 2013/05/29 Commented out by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Config/ConfigReader.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Config/ConfigEntity.h"
#include "Config/ConfigMgr.h"
#include "Debug/ErrId.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Network/Network.h"
#include "Porting/GetErrno.h"
#include "Porting/SystemError.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"



OmnString	OmnConfigReader::eDefaultConfigFileName = "";



OmnConfigReader::OmnConfigReader(const OmnString &fileName)
:
mFile(OmnNew OmnFile(AosMemoryCheckerArgsBegin)),
mLock(OmnNew OmnMutex())
{
	mFile->setName(fileName);
	createParser();
}


OmnConfigReader::OmnConfigReader(const int argc, char **argv)
:
mFile(OmnNew OmnFile(AosMemoryCheckerArgsBegin)),
mLock(OmnNew OmnMutex())
{
	//
	// This function takes two parameters that are passed on
	// through the command line. It assumes the following:
	//
	// -c <configFileName>
	//
	// If the arguments are there, it retrieves the file name 
	// from **argv. Otherwise, there will be no configuration file.
	//
	for (int i=1; i<argc; i++)
	{
		// 
		// '-s' for printing the debugging information to the stdio
		//
		if (OmnString(argv[i]) == OmnString("-s"))
		{
			// 
			// Debug information shall be on the stdio
			//
			OmnTracer::mStdio = true;
			continue;
		}

		// 
		// '-c filename' is used to specify configuration file name.
		// If it is not specified, a local 'appconfig.txt' will be assumed.
		//
		if (OmnString(argv[i]) == OmnString("-c"))
		{
			//
			// Found it.
			//
			i++;
			if (i >= argc)
			{
				OmnExcept e(OmnFileLine, OmnErrId::eProgError,
					"Incorrect command line arguments: -c");
				throw e;
			}
			
			mFile->setName(argv[i]);
			OmnTrace << "Found configuration file: " 
				<< mFile->getFileName() << endl;
			continue;
		}
	}

	OmnRslt rslt = createParser();
	if (!rslt)
    {
		//
		// Since the application is not up yet, we will print the error
		// message to the screen
		//
		cout << "<" << __FILE__ << ":" << __LINE__ << "> **********"
			<< "Failed to create config parser: " << rslt.toString().data() << endl;

        OmnExcept e(OmnFileLine, rslt.getErrId(), 
			OmnString("Failed to create config parser: ") 
			<< rslt.toString());
        throw e;
    }
}


OmnConfigReader::~OmnConfigReader()
{
}


OmnRslt
OmnConfigReader::createParser()
{
	//
	// It reads data from configuration file by invoking readData().
	//

	// 
	// If no file name specified, there is no need to create parser
	//
	if (mFile->getFileName().length() <= 0)
	{
		return true;
	}

	OmnRslt	rslt(true);

	rslt = readData();
	if (!rslt)
	{
		return rslt;
	}

	OmnXmlItemPtr	item;
	mParser = OmnNew OmnXmlParser(mData);

	if (mParser.isNull())
	{
		return OmnWarn << "Failed to create parser" << enderr;
	}
	
	OmnTrace << "Configuration Parser created successfully!" << endl;
	return true;
}


OmnRslt
OmnConfigReader::readData()
{
    OmnRslt rslt(true);
	if (!mFile->isGood())
	{
		if (!mFile->openFile(OmnFile::eReadOnly))
		{
			// 
			// Since the system is not up yet, we will print it to screen, too.
			//
			OmnScreen << "********** Failed to open config file: " 
				<< mFile->getFileName() << endl;

			return OmnWarn	<< "Failed to open configuration file: " 
					<< mFile->getFileName() << enderr;
		}
	}

	int	size = mFile->readToString(mData);
	if ( size <= 0 )
	{
		OmnScreen << "********** Failed to read from config file: " 
			<< mFile->getFileName() << endl;

		return OmnAlarm << "Failed to read from configuration file: " 
			<< ". File name: " 
			<< mFile->getFileName() << enderr;
	}

	mFile->closeFile();
    return true;
}


OmnXmlParserPtr		
OmnConfigReader::getParser() const 
{
	return mParser;
}
#endif
