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
#include "Torturer/CliCmd.h"

#include "aosUtil/Alarm.h"
#include "Torturer/Torturer.h"
#include "RVG/RVG.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


AosCliCmd::AosCliCmd()
{
}


AosCliCmd::~AosCliCmd()
{
}


bool 
AosCliCmd::serialize(TiXmlNode& node)
{
	if (!AosCommand::serialize(node))
	{
		OmnAlarm << "Failed to config the object" << enderr;
		return false;
	}
	return true;
}


bool 
AosCliCmd::deserialize(TiXmlNode* node)
{
	if (!AosCommand::deserialize(node))
	{
		OmnAlarm << "Failed to config the object" << enderr;
		return false;
	}
	return true;
}


bool 	
AosCliCmd::check()
{
	return true;
}


std::string
AosCliCmd::toString()
{
	std::string str;
	str = "Cli: \n";
	str += "   	CliName:            ";
	str += mCliName;
   	str +=  "\n";
	str	+= "    Number of Arguments: ";
	str += mArgs.size();
	str += "\n";

	AosParmPtrArray::iterator itr;
	for (itr=mArgs.begin(); itr!=mArgs.end(); itr++)
	{
		//str << (*itr)->toString(4) << "\n";
	}

	return str;
}


std::string
AosCliCmd::getCmdPrefix() const
{
	std::string str = mCliName;
	str += " ";
	return str;
}


bool
AosCliCmd::constructCmd(const u32 argIndex, 
							  std::string &decl, 
							  std::string &deleteStmt,
							  std::string &cmd)
{
	if (argIndex < 0 || argIndex >= mArgs.size())
	{
		return false;
	}
	//
	// Argument value was generated, we need now to generate 
	// a string for how to use the value in the function call. 
	// This is done by calling the function "getCrtValueAsArg".
	// If it needs to declare some local variables, the declarations
	// are returned through "decl".
	//
	std::string value;
	if (!mArgs[argIndex]->getCrtValueAsArg(value, decl, argIndex))
	{
		OmnAlarm << "Failed to get the argument: " 
			<< mArgs[argIndex]->getName() << enderr;
		return false;
	}

	// 
	// If local variables are declared and memory are allocated
	// for them, then we need to construct statements to delete
	// the memory after calling the funciton. This is what 
	// "getDeleteStmt(...)" does.
	//
	if (!mArgs[argIndex]->getDeleteStmt(deleteStmt, argIndex))
	{
		OmnAlarm << "Failed to get the delete statement: " 
			<< mArgs[argIndex]->getName() << enderr;
		return false;
	}

	if (argIndex == 0)
	{
		cmd += value;
	}
	else
	{
		cmd += " ";
	   	cmd += value;
	}

	return true;
}


// 
// Description:
// All Parms have been instantiated and a command was generated.
// It is the time to finish up the function call. 
//
void
AosCliCmd::finishUpCmd(const u32 cmdId, 
							 const std::string &decl, 
							 const std::string &deleteStmt,
							 std::string &cmd)
{
	// 
	// The function call has been constructed. Need to determine 
	// whether local variable declarations are needed. This is 
	// determined by "decl".
	//
	cmd += ")";
	if (decl != "")
	{
		std::string theCmd = "\n    {\n";
		theCmd += "        ";
	    theCmd += decl;
		theCmd += "\n        cout << \"CmdId ";
		theCmd += AosValue::itoa(cmdId);
	   	theCmd +=  ": \" << ";
	    theCmd += cmd;
	   	theCmd += " << endl;\n";
		if (deleteStmt != "")
		{
			theCmd += "        ";
		   	theCmd += deleteStmt;
		   	theCmd += "\n";
		}
		theCmd += "    }";
		cmd = theCmd;
	}
	else
	{
		std::string theCmd = "\n    cout << \"CmdId ";
		theCmd += AosValue::itoa(cmdId);
	   	theCmd += ": \" << ";
		theCmd += cmd;
	   	theCmd += " << endl;\n";
		cmd = theCmd;
	}
}


bool 
AosCliCmd::createInstance(std::string configFile, AosCliCmdPtr& cli)
{
	cli = OmnNew AosCliCmd();
	return cli->readFromFile(configFile);
}
