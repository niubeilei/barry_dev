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
#include "TorturerGen/TorturerGen.h"

#include "Alarm/Alarm.h"
#include "Porting/GetTime.h"
#include "ProgramAid/Util.h"
#include "Random/RandomUtil.h"
#include "TorturerGen/Ptrs.h"
#include "TorturerGen/FuncTesterGen.h"
#include "TorturerGen/TesterGenSimple.h"
#include "TorturerUtil/Util.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"


static OmnString sgCreationDate = "<$$CreationDate>";
static OmnString sgTargetExeName = "<$$TargetExecutableName>";
static OmnString sgTestSuiteName = "<$$TestSuiteName>";
static OmnString sgTestSuiteHeader = "<$$TestSuiteHeaderFile>";


AosTorturerGen::AosTorturerGen(const OmnString &filename) 
:
mFilename(filename)
{
	readConfig();
}


AosTorturerGen::AosTorturerGen(
		const OmnString &srcDirName, 
		const OmnString &testerDirName,
		const OmnString &torturerName, 
		const OmnString &filename,
		const bool withCliSupport, 
		const bool createBasicTester)
:
mSrcDirName(srcDirName),
mTestDirName(testerDirName),
mTorturerName(torturerName),
mWithCliSupport(withCliSupport)
{
//	if (createBasicTester)
//	{
//		AosSimpleTesterGenPtr tester = OmnNew AosSimpleTesterGen(
//				homedir, srcDirName, testerDirName, filename);
//		AosTesterGenPtr test = OmnNew AosFuncTesterGen(homedir, srcDirName, testerDirName);
//		tester->appendTest(test);
//		mTesters.append(tester);
//	}
}


AosTorturerGen::~AosTorturerGen()
{
}


bool
AosTorturerGen::readConfig()
{
	// 
	// 	<Torturer>
	// 		<HomeDirName>
	// 		<SrcDirName>
	// 		<TestDirName>
	// 		<TorturerName>
	// 		<TorturerDesc>
	// 		<WithCliSupport>
	// 		<Testers>
	//      	<Tester>
	//				<Test>...</Test>
	//				<Test>...</Test>
	//				...
	//			</Tester>
	//			...
	// 		</Testers>
	// 	</Torturer>
	//
	// The torturer class name is <TorturerName>. 
	// 

	OmnXmlParser parser;
	if (!parser.readFromFile(mFilename))
	{
		OmnAlarm << "Failed to read the configure file: " 
			<< mFilename << enderr;
		return false;
	}

	OmnXmlItemPtr item = parser.nextItem();
	if (!item)
	{
		OmnAlarm << "Failed to retrieve the torturer definition: "
			<< mFilename << enderr;
		return false;
	}

	// 
	// Retrieve Source Code Directory Name
	//
	mSrcDirName = item->getStr("SrcDirName", "");
	if (mSrcDirName == "")
	{
		OmnAlarm << "Missing Source Directory Name: " 
			<< mFilename << enderr;
		return false;
	}

	// 
	// Retrieve Test Directory Name
	//
	mTestDirName = item->getStr("TestDirName", "");
	if (mTestDirName == "")
	{
		OmnAlarm << "Missing Test Directory Name: " 
			<< mFilename << enderr;
		return false;
	}

	// 
	// Retrieve Torturer Name
	//
	mTorturerName = item->getStr("TorturerName", "");
	if (mTorturerName == "")
	{
		OmnAlarm << "Missing torturer file name: " 
			<< mFilename << enderr;
		return false;
	}

	// 
	// Retrieve Torturer Description 
	//
	mTorturerDesc = item->getStr("TorturerDesc", "");
	if (mTorturerDesc== "")
	{
		mTorturerDesc = mTorturerName;
		mTorturerDesc << "Test Suite";
	}

	// 
	// Retrieve WithCliSupport
	//
	mWithCliSupport = item->getBool("WithCliSupport", false);

	// 
	// Create Testers
	//
	OmnXmlItemPtr testers = item->getItem("Testers");
	if (!testers)
	{
		// 
		// No testers specified
		//
		return true;
	}

	testers->reset();
	while (testers->hasMore())
	{
		OmnXmlItemPtr def = testers->next();
		AosTesterGenPtr tester = AosTesterGen::createTester(
				mSrcDirName, mTestDirName, mTorturerName, def);
		if (!tester)
		{
			OmnAlarm << "Failed to create the tester: " 
				<< def->toString() << enderr;
			return false;
		}

		mTesters.append(tester);
	}

	return true;
}


bool	
AosTorturerGen::addTester(const AosTesterGenPtr &tester)
{
	mTesters.append(tester);
	return true;
}


bool
AosTorturerGen::genCode(const bool force)
{
	// 
	// We need to generate the following files:
	// 
	// a. Makefile
	// b. main.cpp
	// c. TestSuite.cpp
	// d. TestSuite.h
	//
	// createMakefile();
	// createMain();
	// createTestSuite();
	// createTesters();
	OmnString includes;
	for (int i=0; i<mTesters.entries(); i++)
	{
		includes << "#include \"" << mSrcDirName
			 << "/" << mTestDirName 
			 << "/" << mTesters[i]->getHeaderFileName()
			 << ".h\"";
	}
	
	OmnString testers;
	for (int i=0; i<mTesters.entries(); i++)
	{
		testers << "    suite->addTestPkg(OmnNew "
			 << mTesters[i]->getClassName()
			 << "());\n";
	}
	
	OmnString dirname = getDirectoryName();
	AosMakefileType type = (mWithCliSupport)?
		eAosMakefileType_ExeWithCli:eAosMakefileType_Exe;
	AosCodeGenMainType mainType = (mWithCliSupport)?
		eAosCodeGenMainType_Normal:eAosCodeGenMainType_Normal_with_Cli;
		
	bool ret = AosTorturerUtil::createMakefile(dirname, mTorturerName, 
						mTorturerName, force, type);
	ret = ret && AosTorturerUtil::createMain(dirname, mTorturerName, 
			   			mTorturerName, mainType, force);
	ret = ret && AosTorturerUtil::createTestSuite(dirname, mSrcDirName, 
			   			mTestDirName, mTorturerName, mTorturerDesc, 
						includes, testers, force);
	ret = ret && createTesters(force);

	return ret;
}


OmnString
AosTorturerGen::getDirectoryName() const
{
	OmnString str = AosProgramAid_getAosHomeDir();
	str << "/src/" << mSrcDirName;
    str << "/" << mTestDirName;
	return str;
}


bool
AosTorturerGen::createTesters(const bool force)
{
	for (int i=0; i<mTesters.entries(); i++)
	{
		mTesters[i]->generateCode(force);
	}

	return true;
}
