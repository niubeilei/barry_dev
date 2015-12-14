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
// 03/01/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerGen/TesterGenSimple.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "TorturerGen/TestGen.h"
#include "TorturerUtil/Util.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"
#include "XmlParser/XmlItem.h"


AosSimpleTesterGen::AosSimpleTesterGen(
			const OmnString &srcDirName, 
			const OmnString &testDirName, 
			const OmnString &filename, 
			const int tries)
:
AosTesterGen(srcDirName, testDirName),
mTries(tries)
{
	mClassName = filename;
}


AosSimpleTesterGen::~AosSimpleTesterGen()
{
}


bool
AosSimpleTesterGen::config(const OmnXmlItemPtr &def)
{
	// 
	// 
	// 	<Tester>
	// 		<Type>Simple</Type>
	// 		<ClassName>Random</ClassName>
	//      <DefaultTries>10000</DefaultTries>
	//      <MemberData>
	//          int         mMember1;
	//          OmnString   member2;
	//      </MemberData>
	//      <HeaderIncludes>
	//          "Util/String.h"
	//          "Util/Random.h"
	//      </HeaderIncludes>
	//      <SourceIncludes>
	//          "Util1/Time.h"
	//          "UtilComm/Tcp.h"
	//      </SourceIncludes>
	//      <Tests>
	//			<Test>
	//			...
	//			</Test>
	//			...
	//		</Tests>
	//	</Tester>
	//
	mClassName = def->getStr("ClassName", "");
	if (mClassName == "")
	{
		OmnAlarm << "Missing Torturer ClassName: " << def->toString() << enderr;
		return false;
	}

	mTries = def->getInt("DefaultTries", 10000);
	mMemberData = def->getStr("MemberData", "");
	mHeaderIncludes = def->getStr("HeaderIncludes", "");
	mSourceIncludes = def->getStr("SourceIncludes", "");

	if (!createTests(def))
	{
		return false;
	}

	return true;
}


bool
AosSimpleTesterGen::createTests(const OmnXmlItemPtr &def)
{
	// 
	//	<Tests>
	//		<Test>
	//		...
	//		</Test>
	//	</Tester>
	//
	OmnXmlItemPtr tests = def->getItem("Tests");
	if (!tests)
	{
		// 
		// No tests
		//
		return true;
	}

	tests->reset();
	while (tests->hasMore())
	{
		OmnXmlItemPtr item = tests->next();
		AosTestGenPtr test = AosTestGen::createTest(item);
		if (!test)
		{
			OmnAlarm << "Failed to create test: " << item->toString() << enderr;
			return false;
		}

		mTests.append(test);
	}

	return true;
}


bool
AosSimpleTesterGen::generateCode(const bool force)
{
	return createHeaderFile(force) && createSourceFile(force);
}


bool
AosSimpleTesterGen::createHeaderFile(const bool force)
{
	OmnString filename = getFilename();
	filename << ".h";
	if (!force && OmnFile::fileExist(filename))
	{
		return true;
	}

	OmnFile file(filename, OmnFile::eCreate);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to create the header file: " 
			<< filename << enderr;
		return false;
	}

	file << aos_get_file_declaimer();
	file << aos_get_header_guard(mSrcDirName, mTestDirName, getClassName());
	file << "#include \"Debug/Debug.h\"\n"
		 << "#include \"Tester/TestPkg.h\"\n"
		 << "#include \"Util/Ptrs.h\"\n";
	file << getIncludes(mHeaderIncludes);
	file << "\n\n\n";

	file << "class " << getClassName() << " : public OmnTestPkg\n"
		 << "{\n"
		 << "private:\n"
		 << "    int              mBasicTestRepeat;\n"
		 << "    AosGenTablePtr   mTable;\n"; 

	addMemberData(file);

	// 
	// Create default constructor and destructor
	//
	file << "\npublic:\n";
	file << "    " << getClassName() 
		 << "();\n"
		 << "    ~" << getClassName() << "();\n"
		 << "\n"
		 << "    virtual bool     start();\n"
		 << "\n\n"
		 << "private:\n";
	
	addMemberFunctionToHeaderFile(file);

	file << "};\n"
		 << "#endif\n"
		 << "\n";
	file.closeFile();
	return true;
}


bool
AosSimpleTesterGen::addMemberData(OmnFile &file)
{
	OmnString memData = mMemberData;
	while (memData.length() > 0)
	{
		OmnString theLine = memData.getLine(true);
		if (theLine == "")
		{
			break;
		}

		theLine.removeLeadingWhiteSpace();
		if (theLine != "")
		{
			file << "    " << theLine;
		}
	}

	return true;
}


bool
AosSimpleTesterGen::addMemberFunctionToHeaderFile(OmnFile &file)
{
	for (int i=0; i<mTests.entries(); i++)
	{
		file << "    " << mTests[i]->getFuncDeclaration() << "\n";
	}

	return true;
}


bool
AosSimpleTesterGen::createSourceFile(const bool force)
{
	OmnString filename = getFilename();
	filename << ".cpp";
	if (!force && OmnFile::fileExist(filename))
	{
		return true;
	}

	OmnFile file(filename, OmnFile::eCreate);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to create the file: " 
			<< filename << enderr;
		return false;
	}

	file << aos_get_file_declaimer();
	file << "#include \""
		 << mSrcDirName << "/" 
		 << mTestDirName << "/"
		 << mClassName << ".h\"\n";
	file << "\n";
	file << "#include \"Debug/Debug.h\"\n";
	file << "#include \"Tester/Test.h\"\n";
	file << "#include \"Tester/TestMgr.h\"\n";
	file << "#include \"Util/GenTable.h\"\n";

	// 
	// Add the include files
	//
	file << getIncludes(mSourceIncludes);

	file << "\n\n";

	// 
	// Default Constructor
	//
	OmnString tries;
	tries << mTries;
	file << getClassName() << "::" << getClassName() << "()\n"
		 << "{\n"
		 << "    mBasicTestRepeat = " << tries << ";\n"
		 << "}\n"
		 << "\n\n";

	// 
	// Destructor 
	//
	file << getClassName() << "::~" << getClassName() 
		 << "()\n"
		 << "{\n"
		 << "}\n"
		 << "\n\n";

	// 
	// Create the ::start() function
	//
	file << "bool\n" 
		 << getClassName() << "::start()\n"
		 << "{\n";

	// 
	// Calling all the test functions
	//
	int i;
	for (i=0; i<mTests.entries(); i++)
	{
		file << "    " << mTests[i]->getFuncName() 
			 << "(mBasicTestRepeat, mTable);\n";
	}

	file << "    return true;\n"
		 << "}\n";

	// 
	// Create all the tests
	//
	for (i=0; i<mTests.entries(); i++)
	{
		file << mTests[i]->genCode(getClassName()) << "\n\n";
	}

	file.closeFile();

	return true;
}


