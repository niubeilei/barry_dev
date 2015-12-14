////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This class automatically creates the header and source files for
// a class. 
//
// To Do List:
// 1. Need to get the current directory instead of asking users to 
//    enter it through the command line.
// 2. Need to create a C header file or a C source file.
//
// Modification History:
// 03/16/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ProgramAid/CreateClassFiles.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/GetTime.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "XmlParser/XmlItem.h"
#include <stdlib.h>

const OmnString sgCreationDate = "<$$CreationDate>";
const OmnString sgCreatorName = "<$$CreatorName>";
const OmnString sgProjectPrefix= "<$$ProjectPrefix>";
const OmnString sgSrcDirName = "<$$SourceDirectoryName>";
const OmnString sgHeaderFileName = "<$$HeaderFileName>";
const OmnString sgClassName = "<$$ClassName>";

AosCreateClassFiles::AosCreateClassFiles(
				const OmnString &projHomeDir, 
				const OmnString &dirname, 
				const OmnString &className, 
				const OmnString &projPrefix,
				const bool withSmartPointerSupport, 
				const bool force, 
				const OmnString &srcFileExt)
:
mProjHomeDir(projHomeDir),
mDirName(dirname),
mClassName(className),
mProjPrefix(projPrefix),
mSmartPtrSupport(withSmartPointerSupport), 
mForce(force),
mSrcFileExt(srcFileExt)
{
	createHeaderFile();
	createSourceFile();
}


AosCreateClassFiles::~AosCreateClassFiles()
{
}


bool
AosCreateClassFiles::createHeaderFile()
{
	OmnString filename = "./";
	filename << mClassName << ".h";
	
	if (!mForce && OmnFile::fileExist(filename))
	{
		//
		// The file is there. Do nothing.
		// 
		return true;
	}

	// 
	// Copy the template file to the new location
	//
	OmnString cmd = "cp ";
	if (mSmartPtrSupport)
	{
		cmd << mProjHomeDir << "/Data/ProgramGen/ClassHeader_SPtr.h ./" << filename;
	}
	else
	{
		cmd << mProjHomeDir << "/Data/ProgramGen/ClassHeader.h ./" << filename;
	}

	OmnTrace << "To run command: " << cmd << endl;
	system(cmd.data());

	// 
	// Open the Makefile template
	//
	OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the file: " 
			<< filename << enderr;
		return false;
	}

	OmnString username = getenv("USER");
	if (!file.replace(sgCreationDate, OmnGetMDY(), false))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	if (!file.replace(sgCreatorName, username, false))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	if (!file.replace(sgProjectPrefix, mProjPrefix, true))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	if (!file.replace(sgSrcDirName, mDirName, true))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	if (!file.replace(sgHeaderFileName, mClassName, true))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	OmnString classname = mProjPrefix;
	classname << mClassName;
	if (!file.replace(sgClassName, classname, true))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	// 
	// Create the call section. 
	//
	
	file.flushFileContents();
	return true;
}


bool
AosCreateClassFiles::createSourceFile()
{
	OmnString filename = "./";
	filename << mClassName << "." << mSrcFileExt;
	
	if (!mForce && OmnFile::fileExist(filename))
	{
		//
		// The file is there. Do nothing.
		// 
		return true;
	}

	// 
	// Copy the template file to the new location
	//
	OmnString cmd = "cp ";
	cmd << mProjHomeDir << "/Data/ProgramGen/ClassCpp.cpp ./" << filename;

	OmnTrace << "To run command: " << cmd << endl;
	system(cmd.data());

	// 
	// Open the Makefile template
	//
	OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the file: " 
			<< filename << enderr;
		return false;
	}

	OmnString username = getenv("USER");
	OmnString headerFilename = mClassName;
	headerFilename << ".h";
	OmnString classname = mProjPrefix;
	classname << mClassName;
	bool ret = file.replace(sgCreationDate, OmnGetMDY(), false) &&
			   file.replace(sgCreatorName, username, false) &&
			   file.replace(sgSrcDirName, mDirName, false) &&
			   file.replace(sgHeaderFileName, headerFilename, false) &&
			   file.replace(sgClassName, classname, true);

	if (!ret)
	{
		OmnAlarm << "Failed to create the file" << enderr;
		return false;
	}

	// 
	// Create the call section. 
	//
	
	file.flushFileContents();
	return true;
}


