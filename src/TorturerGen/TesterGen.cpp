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
// 02/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerGen/TesterGen.h"

#include "Alarm/Alarm.h"
#include "ProgramAid/Util.h"
#include "Random/RandomUtil.h"
#include "TorturerGen/TesterGenSimple.h"
#include "TorturerGen/TestGen.h"
#include "TorturerGen/FuncTesterGen.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"
#include "XmlParser/XmlItem.h"


AosTesterGen::AosTesterGen(const OmnString &srcDirName, 
						   const OmnString &testDirName) 
:
mSrcDirName(srcDirName),
mTestDirName(testDirName)
{
}


AosTesterGen::~AosTesterGen()
{
}


AosTesterGenPtr
AosTesterGen::createTester(
		const OmnString &srcDirName, 
		const OmnString &testDirName, 
		const OmnString &filename,
		const OmnXmlItemPtr &def)
{
	OmnString type = def->getStr("Type", "Simple");
	AosTesterGenPtr gen;
	if (type == "Simple")
	{
		gen = OmnNew AosSimpleTesterGen(srcDirName, testDirName, filename);

		return gen;
	}
	else if (type == "FuncTest")
	{
		gen = OmnNew AosFuncTesterGen(srcDirName, testDirName);
	}
	else
	{
		OmnAlarm << "TesterGen Type not supported: " << type << enderr;
	}

	if (gen->config(def))
	{
		return gen;
	}

	OmnAlarm << "Failed to create the object: " << def->toString() << enderr;
	return 0;
}


OmnString
AosTesterGen::getFilename() const
{
	// 
	// Filename is: "Aos<TesterName>Tester.h/.cpp"
	// This function returns the root of the file name (i.e., without .h or .cpp)
	//
	OmnString name = getDirectoryName();
	name << "/" << mClassName;
	return name;
}


OmnString
AosTesterGen::getDirectoryName() const
{
    OmnString str = AosProgramAid_getAosHomeDir();
	str << "/src/" << mSrcDirName;
	str << "/" << mTestDirName;
	return str;
}


OmnString
AosTesterGen::getIncludes(const OmnString &files) const
{
	// 
	// "files" is the names of a list of files, separated by '\n'. 
	// This function will create a string that is in the form of:
	//
	// #include file1
	// #include file2
	// ...
	//
	
	OmnString includes;
	OmnString str = files;
	while (str != "")
	{
		OmnString theLine = str.getLine(true);

		if (theLine == "")
		{
			break;
		}

		theLine.removeLeadingWhiteSpace();
		if (theLine != "")
		{
			includes << "#include " << theLine;
		}
	}

	return includes;
}
	

bool
AosTesterGen::appendTest(const AosTestGenPtr &test)
{
	mTests.append(test);
	return true;
}
