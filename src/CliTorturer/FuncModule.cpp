////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FuncModule.h
// Description:
//   
//
// Modification History:
// 05/29/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "CliTorturer/FuncModule.h"

#include "Alarm/Alarm.h"
#include "aosUtil/Alarm.h"
#include "Debug/Debug.h"
#include "CliTorturer/CliTorturer.h"
#include "ProgramAid/Util.h"
#include "Tester/TestMgr.h"
#include "Parms/RVGInstGen.h"
#include "TorturerGen/FuncTesterGen.h"
#include "TorturerUtil/Util.h"
#include "Util/File.h"
#include "XmlParser/XmlItem.h"

static OmnString sgRegistInst = "<$$RegisterApiFunc>";
static OmnString sgIncludes = "<$$ApiFuncIncludes>";
static OmnString sgLibName = "<$$AdditionalLibs>";
static OmnString sgInitAPI = "<$$InitAPI>";


AosFuncModule::AosFuncModule( const OmnString &moduleId)
:
mModuleId(moduleId)
{
	return;
}


AosFuncModule::~AosFuncModule()
{
}


bool
AosFuncModule::parseApi(const OmnXmlItemPtr &def,
						const OmnString &srcDir)
{
	AosFuncTesterGenPtr func = OmnNew AosFuncTesterGen(srcDir, "");
	if (!func)
	{
		OmnAlarm << "Failed to create func: " << def->toString() << enderr;
		return false;
	}

	if (!func->config(def))
	{
		OmnAlarm << "Failed to config the function: " 
			<< def->toString() << enderr;
			return false;
	}

	// 
	// Make sure the function is turned on
	//
	if (!func->getStatus())
	{
		return true;
	}

	return addFunc(func);
}


bool 			
AosFuncModule::addFunc(const AosFuncTesterGenPtr &func)
{
	// 
	// Make sure the same command is not added multiple times
	//
	if (mFuncs.entries() >= eMaxFuncs)
	{
		OmnAlarm << "Too many Functions in a module. The maximum allowed is: " 
			<< eMaxFuncs << enderr;
		return false;
	}

	// 
	// Make sure the command is not already in the array
	//
	for (int i=0; i<mFuncs.entries(); i++)
	{
		if (mFuncs[i] == func)
		{
			// The func is already in the list
			OmnWarn << "The function is already in the list" << enderr;
			return false;
		}
	}

	// The function is not in the list. 
	mFuncs.append(func);

	return true;
}


bool	
AosFuncModule::genCode(const OmnString &srcDir, 
					   const OmnString &torturerName,
					   const OmnString &additionalLibs,
					   const bool force) const
{
	for (int i=0; i<mFuncs.entries(); i++)
	{
		if (!mFuncs[i]->generateCode(force))
		{
			OmnAlarm << "Failed to generate code" << enderr;
			return false;
		}
	}

	// 
	// Need to generate the makefile
	//
//	if (!AosTorturerUtil::createMakefile(srcDir, "", libName, force, 
//			eAosMakefileType_Lib))
//	{
//		OmnAlarm << "Failed to create Makefile" << enderr;
//		return false;
//	}

	// 
	// Generate the torturer and its makefile
	//
	if (!createMain(srcDir, torturerName, force))
	{
		OmnAlarm << "Failed to create the main" << enderr;
		return false;
	}

//	OmnString libs = libName;
//	libs.removeLeadingWhiteSpace();
//	libs.removeTailWhiteSpace();
//	libs << "\n" << additionalLibs;
	if (!createMakefile(srcDir, torturerName, additionalLibs, force))
	{
		OmnAlarm << "Failed to create the makefile" << enderr;
		return false;
	}

	return true;
}


bool
AosFuncModule::createMain(const OmnString &torturerDir, 
						  const OmnString &torturerName, 
						  const bool force) const
{
	// 
	// Create the main.cpp
	//
	if (!AosTorturerUtil::createMain(torturerDir, "", 
			torturerName, eAosCodeGenMainType_ApiTorturer, force))
	{
		OmnAlarm << "Failed to create main" << enderr;
		return false;
	}

    OmnString filename = AosProgramAid_getAosHomeDir();
	filename << "/src/" << torturerDir << "/main.cpp";
	
	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the main template: "
			<< filename << enderr;
		return false;
	}

	OmnString registStmt;
	OmnString includes;
	int i;
	for (i=0; i<mFuncs.entries(); i++)
	{
		registStmt << mFuncs[i]->getClassName()
			<< "::registInst();\n";
		includes << mFuncs[i]->getIncludeStmt();
	}

	OmnString incs = AosCliTorturer::getSelf()->getInitIncludes();
	while (incs != "")
	{
		OmnString line = incs.getLine(true);
		if (line == "") break;

		line.removeLeadingWhiteSpace();
		line.removeTailWhiteSpace();
		if (line == "") continue;

		line.removeLeadingWhiteSpace();
		includes << "#include \"" << line << "\"\n";
	}
	// includes << AosCliTorturer::getSelf()->genInstGenIncludes();
	// registStmt << AosCliTorturer::getSelf()->genInstGenCode();
	registStmt << AosRVGInstGen::genInstRegisterCode();
	includes << AosRVGInstGen::genInstGenIncludes(
					AosCliTorturer::getSelf()->getFuncTorturerDir(), 
					""); 

	if (!file.replace(sgInitAPI, 
			AosCliTorturer::getSelf()->getInitAPI(), false))
	{
		OmnAlarm << "Failed to replace InitAPI" << enderr;
		return false;
	}

	if (!file.replace(sgRegistInst, registStmt, false))
	{
		OmnAlarm << "Failed to replace register statement" << enderr;
		return false;
	}

	if (!file.replace(sgIncludes, includes, false))
	{
		OmnAlarm << "Failed to replace includes" << enderr;
		return false;
	}

	file.flushFileContents();
	return true;
}


bool
AosFuncModule::createMakefile(const OmnString &torturerDir, 
						      const OmnString &torturerName, 
							  const OmnString &libnames, 
							  const bool force) const
{
	if (!AosTorturerUtil::createMakefile(torturerDir, "",
			torturerName, force, eAosMakefileType_Exe))
	{
		OmnAlarm << "Failed to create the makefile" << enderr;
		return false;
	}

    OmnString filename = AosProgramAid_getAosHomeDir();
	filename << "/src/" << torturerDir << "/Makefile";

	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the makefile: "
			<< filename << enderr;
		return false;
	}

	OmnString lib;
	OmnString libs = libnames;
	while (libs != "")
	{
		OmnString line = libs.getLine(true);
		if (line == "") break;
		line.removeLeadingWhiteSpace();
		line.removeTailWhiteSpace();
		if (line == "") continue;

		// 
		// If the lib name starts with "-l", we will use it as it is. 
		// Otherwise, we will construct a line in the form:
		//     $(LIBDIR)/libxxxx.a
		// where 'xxxx' is the line
		//
		const char *data = line.data();
		if (line.length() > 2 && data[0] == '-' && data[1] == 'l')
		{
			lib << "        " << line << " \\\n";
		}
		else 
		{
			lib << "$(LIBDIR)/lib" << line << ".a \\\n";
		}
	}

	if (!file.replace(sgLibName, lib, false))
	{
		OmnAlarm << "Failed to replace the libname: " << filename << enderr;
		return false;
	}

	file.flushFileContents();
	return true;
}

