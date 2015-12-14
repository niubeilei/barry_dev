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
//
// Modification History:
// 3/20/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "ProgramAid/Util.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/GetTime.h"
#include "Util/File.h"



const OmnString sgCreationDate = "<$$CreationDate>";
const OmnString sgCreatorName = "<$$CreatorName>";
const OmnString sgProjectPrefix= "<$$ProjectPrefix>";
const OmnString sgSrcDirName = "<$$SourceDirectoryName>";
const OmnString sgHeaderFileName = "<$$HeaderFileName>";
 


OmnString AosProgramAid_getAosHomeDir()
{
	OmnString name = getenv("AOS_HOME");
	if (name == "")
	{
		OmnAlarm << "AOS_HOME environment variable is not specified. "
			<< "This variable points to the parent directory of 'src'" 
			<< enderr;
		return "";
	}

	return name;
}


bool
AosCreatePtrFile(const OmnString &projPrefix,
				 const OmnString &dirname,
				 const OmnString &filename)
{
	if (OmnFile::fileExist(filename))
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
	cmd << AosProgramAid_getAosHomeDir() 
		<< "/Data/ProgramGen/Ptrs.h " << filename;

	OmnTrace << "To run command: " << cmd << endl;
	system(cmd.data());

	// 
	// Open the 
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

	if (!file.replace(sgProjectPrefix, projPrefix, true))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	OmnString dir = dirname;
	dir.substitute('/', '_', true);
	if (!file.replace(sgSrcDirName, dir, true))
	{
		OmnAlarm << "Failed to replace the creation date" << enderr;
		return false;
	}

	if (!file.replace(sgHeaderFileName, "Ptrs", true))
	{
		OmnAlarm << "Failed to replace HeaderFile Name" << enderr;
		return false;
	}

	file.flushFileContents();
	return true;
}


bool AosAddSmartPtr(
			const OmnString &projPrefix, 
			const OmnString &srcDirName, 
			const OmnString &className, 
			const OmnString &filename)
{
	if (!OmnFile::fileExist(filename))
	{
		if (!AosCreatePtrFile(projPrefix, srcDirName, filename))
		{
			OmnAlarm << "Failed to create Ptrs.h" << enderr;
			return false;
		}
	}

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

	OmnString decl = "OmnPtrDecl(";
	decl << projPrefix 
		 << className
		 << ", "
		 << projPrefix
		 << className
		 << "Ptr)\n";
	
	if (file.find(decl) < 0)
	{
		// 
		// It is not in the file. Need to insert it into the file
		//
		decl << "#endif";
		if (!file.replace("#endif", decl, false))
		{
			OmnAlarm << "Failed to insert the declaration: " 
				<< filename << enderr;
			return false;
		}

		file.flushFileContents();
	}

	return true;
}


bool 
AosProgramAid_dataTypeSupported(const OmnString &type)
{
	return true;
}

