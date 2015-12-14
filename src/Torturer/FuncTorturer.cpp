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
// 01/04/2008: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#include "Torturer/FuncTorturer.h"
#include "Torturer/FuncModule.h"
#include "Torturer/TorUtil.h"
#include "RVG/ConfigMgr.h"
#include "Util/File.h"
#include "Util/OmnNew.h"

static std::string sgRegistInst = "<$$RegisterFunc>";
static std::string sgIncludes = "<$$FuncIncludes>";
static std::string sgLibName = "<$$AdditionalLibs>";
static std::string sgInitFuncs = "<$$InitFuncs>";
static std::string sgTorturerName = "<$$TorturerName>";
static std::string sgProductName = "<$$ProductName>";


AosFuncCmdPtrArray AosFuncTorturer::mRunTimeFuncs;


AosFuncTorturer::AosFuncTorturer(const std::string& torturerName, const std::string& productName)
:
AosTorturer(torturerName, productName)
{
	mIsGood = init();
}


AosFuncTorturer::AosFuncTorturer()
{
	mIsGood = init();
}


AosFuncTorturer::~AosFuncTorturer()
{
}


bool 	
AosFuncTorturer::genCode(const bool force)
{
	std::string torturerDir;
	if (!AosConfigMgr::getDestGenCodeDir(mProductName, mTorturerName, torturerDir))
	{
		return false;
	}

	mRegistStmt = "";
	mIncludes = "";
	std::list<std::string> cmdNames;
	getCommandNames(cmdNames);
	std::list<std::string>::iterator iter;
	for (iter=cmdNames.begin(); iter!=cmdNames.end(); iter++)
	{
		AosFuncCmdPtr func = OmnNew AosFuncCmd(*iter, mProductName);
		if (!func->readFromFile())
		{
			OmnAlarm << "Read function config error" << enderr;
			return false;
		}
		if (!func->generateCode(torturerDir, force))
		{
			OmnAlarm << "Failed to generate code" << enderr;
			return false;
		}
		mRegistStmt += "	" + func->getClassName() + "::registInst();\n";
		mIncludes += func->getIncludeStmt() + "\n";
	}

	// 
	// Generate the torturer and its makefile
	//
	if (!createMain(torturerDir, force))
	{
		OmnAlarm << "Failed to create the main" << enderr;
		return false;
	}

	if (!createMakefile(torturerDir, force))
	{
		OmnAlarm << "Failed to create the makefile" << enderr;
		return false;
	}
	return true;
}


bool
AosFuncTorturer::createMain(const std::string& torturerDir, const bool force)
{
	// 
	// Create the main.cpp
	//
	if (!AosTorturerUtil::createMain(torturerDir, "", 
			mTorturerName, eAosCodeGenMainType_FuncTorturer, force))
	{
		OmnAlarm << "Failed to create main" << enderr;
		return false;
	}

    std::string filename = torturerDir;
	filename +=  "/main.cpp";
	
	OmnFile file(filename.c_str(), OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the main template: "
			<< filename << enderr;
		return false;
	}

	std::string str;
	std::list<std::string>::iterator iter;
	for (iter=mInitIncludes.begin(); iter!=mInitIncludes.end(); iter++)
	{
		str = *iter;
		if (!str.empty())
		{
			if (str[0] != '#')
			{
				str = "#include \"" + *iter + "\"";
			}
			str += "\n";
			mIncludes += str;
		}
	}

//	registStmt << AosRVGInstGen::genInstRegisterCode();
//	includes << AosRVGInstGen::genInstGenIncludes(
//					AosfuncTorturer::getSelf()->getFuncTorturerDir(), 
//					""); 

	if (!file.replace(sgInitFuncs.c_str(), mInitFuncs.c_str(), false))
	{
		OmnAlarm << "Failed to replace InitFuncs" << enderr;
		return false;
	}

	if (!file.replace(sgRegistInst.c_str(), mRegistStmt.c_str(), false))
	{
		OmnAlarm << "Failed to replace register statement" << enderr;
		return false;
	}

	if (!file.replace(sgIncludes.c_str(), mIncludes.c_str(), false))
	{
		OmnAlarm << "Failed to replace includes" << enderr;
		return false;
	}

	if (!file.replace(sgTorturerName.c_str(), mTorturerName.c_str(), false))
	{
		OmnAlarm << "Failed to replace TorturerName" << enderr;
		return false;
	}

	if (!file.replace(sgProductName.c_str(), mProductName.c_str(), false))
	{
		OmnAlarm << "Failed to replace ProductName" << enderr;
		return false;
	}

	file.flushFileContents();
	return true;
}


bool
AosFuncTorturer::createMakefile(const std::string& torturerDir, const bool force) const
{
	
	if (!AosTorturerUtil::createMakefile(torturerDir, "",
			mTorturerName, force, eAosMakefileType_Exe))
	{
		OmnAlarm << "Failed to create the makefile" << enderr;
		return false;
	}

    std::string filename;
	filename += torturerDir;
  	filename +=	"/Makefile";

	OmnFile file(filename.c_str(), OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to open the makefile: "
			<< filename << enderr;
		return false;
	}

	std::string libs;
	for (std::list<std::string>::const_iterator iter=mAdditionalLibs.begin(); 
			iter!=mAdditionalLibs.end(); iter++)
	{
		// 
		// If the lib name starts with "-l", we will use it as it is. 
		// Otherwise, we will construct a line in the form:
		//     $(LIBDIR)/libxxxx.a
		// where 'xxxx' is the line
		//
		std::string lib = *iter;
		if (lib.length() > 2 && lib[0] == '-' && lib[1] == 'l')
		{
			libs += "        ";
		   	libs += lib + " \\\n";
		}
		else 
		{
			libs += "$(LIBDIR)/lib";
		   	libs += lib + ".a \\\n";
		}
	}

	if (!file.replace(sgLibName.c_str(), libs.c_str(), false))
	{
		OmnAlarm << "Failed to replace the libname: " << filename << enderr;
		return false;
	}

	file.flushFileContents();
	return true;
}


bool 
AosFuncTorturer::start()
{
	return AosTorturer::start();
}


bool 
AosFuncTorturer::init()
{
	if (!AosTorturer::init())
	{
		return false;
	}

	//
	// add the runtime funcs into the modules 
	//
	int moduleNum = mModules.size();
	AosFuncCmdPtrArray::iterator iter;
	for (iter=mRunTimeFuncs.begin(); iter!=mRunTimeFuncs.end(); iter++)
	{
		mLogIncludes.push_back((*iter)->getIncludeStmt());

		for (int i=0; i<moduleNum; i++)
		{
			AosFuncModulePtr ptr = dynamic_cast<AosFuncModule*>(mModules[i].getPtr());
			if (!ptr)
			{
				OmnAlarm << "Module config error" << enderr;
				return false;
			}
			if (ptr->isCmdNameExist((*iter)->getName()))
			{
				ptr->addRunTimeFunc((*iter));
				break;
			}
		}
	}

	if (mLogType == AosTorturerLogType::eFile)
	{
		if (!createNextLogFile())
		{
			return false;
		}
	}
	return true;
}


bool	
AosFuncTorturer::addRunTimeFunc(const AosFuncCmdPtr& func)
{
	if (!func)
	{
		return false;
	}
	AosFuncCmdPtrArray::iterator iter;
	iter = std::find(mRunTimeFuncs.begin(), mRunTimeFuncs.end(), func);
	if (iter != mRunTimeFuncs.end())
	{
		//
		// exist already
		//
		OmnWarn << "The function is already in the list" << enderr;
		return false;
	}
	mRunTimeFuncs.push_back(func);
	return true;
}

