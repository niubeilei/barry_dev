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
//
// Modification History:
// Friday, January 04, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#include "Torturer/Module.h"
#include "Torturer/Torturer.h"
#include "Torturer/FuncModule.h"
#include "Torturer/CliModule.h"
#include "RVG/ConfigMgr.h"
#include "Util/OmnNew.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

AosModule::AosModule()
:
mTotalCmdsRun(0),
mTotalCorrectCmds(0),
mTotalIncorrectCmds(0),
mTotalIgnoreCmds(0),
mTotalFailedChecks(0)
{
}


AosModule::AosModule(const std::string &moduleName, const std::string &productName)
:
mName(moduleName),
mProductName(productName),
mTotalCmdsRun(0),
mTotalCorrectCmds(0),
mTotalIncorrectCmds(0),
mTotalIgnoreCmds(0),
mTotalFailedChecks(0)
{
}


AosModule::~AosModule()
{
}


bool 
AosModule::serialize(TiXmlNode& node)
{
	// 
	// <Module>
	// 		<Name>
	// 		<Weight>
	// 		<MinNumCmds>
	// 		<MaxNumCmds>
	// 		<Status>
	// 		<Cmds>
	// 			<CmdName></CmdName>
	// 			<CmdName></CmdName>
	//			...
	// 		</Cmds>
	// 	</Module>
	//

	node.SetValue("Module");
	//
	// add Name node
	//
	if (mName.empty())
	{
		return false;
	}
	node.addElement("Name", mName);
	//
	// add Weight node
	//
	node.addElement("Weight", AosValue::itoa(mWeight));
	//
	// add MinNumCmds node
	//
	node.addElement("MinNumCmds", AosValue::itoa(mMinNumCmds));
	//
	// add MaxNumCmds node
	//
	node.addElement("MaxNumCmds", AosValue::itoa(mMaxNumCmds));
	//
	// add Status node
	//
	node.addElement("Status", mStatus ? "true" : "false");
	//
	// add Command name list
	//
	TiXmlElement cmds("Cmds");
	std::list<std::string>::iterator iter;
	for (iter=mCmdNames.begin(); iter!=mCmdNames.end(); iter++)
	{
		cmds.addElement("CmdName", *iter);
	}
	node.InsertEndChild(cmds);

	return true;
}


bool 
AosModule::deserialize(TiXmlNode* node)
{
	// 
	// <Module>
	// 		<Name>
	// 		<Weight>
	// 		<MinNumCmds>
	// 		<MaxNumCmds>
	// 		<Status>
	// 		<Cmds>
	// 			<CmdName></CmdName>
	// 			<CmdName></CmdName>
	//			...
	// 		</Cmds>
	// 	</Module>
	//
	if (node == NULL)
	{
		return false;
	}
	//
	// parse name, mandatory field
	//
	if (!node->getElementTextValue("Name", mName))
	{
		OmnAlarm << "XML parse: Name must exist in Module" << enderr;
		return false;
	}
	//
	// parse weight, mandatory field
	//
	if (!node->getElementTextValue("Weight", mWeight))
	{
		OmnAlarm << "XML parse: Weight must exist in Module" << enderr;
		return false;
	}
	//
	// parse MinNumCmds, mandatory field
	//
	if (!node->getElementTextValue("MinNumCmds", mMinNumCmds))
	{
		OmnAlarm << "XML parse: MinNumCmds must exist in Module" << enderr;
		return false;
	}
	//
	// parse MaxNumCmds, mandatory field
	//
	if (!node->getElementTextValue("MaxNumCmds", mMaxNumCmds))
	{
		OmnAlarm << "XML parse: MaxNumCmds must exist in Module" << enderr;
		return false;
	}
	//
	// parse Status, mandatory field
	//
	if (!node->getElementTextValue("Status", mStatus))
	{
		OmnAlarm << "XML parse: Status must exist in Module" << enderr;
		return false;
	}

	//
	// parse Command name list
	//
	mCmdNames.clear();
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("Cmds").ToElement();
	if (element)
	{
		std::string cmdName;
		TiXmlElement* child = element->FirstChildElement("CmdName");	
		for (; child!=NULL; child=child->NextSiblingElement("CmdName"))
		{
			if (child->GetText())
			{
				cmdName = child->GetText();
				addCommand(cmdName);
			}
		}
	}
	return true;
}


bool 
AosModule::writeToFile()
{
	std::string configPath;
	if (!AosConfigMgr::getConfigFileName(mProductName, AosConfigMgr::eModule, mName, configPath))
	{
		OmnAlarm << "Error on getting the config file name of the module" << enderr;
		return false;	
	}
	return writeToFile(configPath);
}


bool 
AosModule::readFromFile()
{
	std::string configPath;
	if (!AosConfigMgr::getConfigFileName(mProductName, AosConfigMgr::eModule, mName, configPath))
	{
		OmnAlarm << "Error on getting the config file name of the module" << enderr;
		return false;	
	}
	return readFromFile(configPath);
}


bool 
AosModule::readFromFile(const std::string& configFile)
{
	TiXmlDocument doc(configFile);
	if (!doc.LoadFile())
	{
		OmnAlarm << "There is no module config file in the module directory" << enderr;
		return false;
	}
	TiXmlElement* element = doc.FirstChildElement();
	if (!element)
	{
		OmnAlarm << "There is no module definition in the config file" << enderr;
		return false;
	}
	if (!deserialize(element))
	{
		return false;
	}
	return true;
}


bool 
AosModule::writeToFile(const std::string& configFile)
{
	TiXmlDocument doc(configFile);
	TiXmlElement element("");
	if (!serialize(element))
	{
		return false;
	}
	doc.InsertEndChild(element);
	doc.SaveFile();
	return true;
}


bool 
AosModule::addCommand(const std::string& cmdName)
{
	if (cmdName.empty())
	{
		return false;
	}
	if (mCmdNames.size() >= eMaxNumCmds)
	{
		OmnAlarm << "Too many Commands in a module. The maximum allowed is: " 
			<< eMaxNumCmds << enderr;
		return false;
	}
	std::list<std::string>::iterator iter;
	iter = std::find(mCmdNames.begin(), mCmdNames.end(), cmdName);
	if (iter != mCmdNames.end())
	{
		//
		// exist already
		//
		OmnWarn << "The command is already in the list" << enderr;
		return false;
	}
	mCmdNames.push_back(cmdName);
	return true;
}


bool 
AosModule::delCommand(const std::string& cmdName)
{
	mCmdNames.remove(cmdName);
	return true;
}


bool 
AosModule::isCmdNameExist(const std::string& cmdName)
{
	std::list<std::string>::iterator iter = std::find(mCmdNames.begin(), mCmdNames.end(), cmdName);
	if (iter != mCmdNames.end())
	{
		return true;
	}
	return false;
}


std::string 
AosModule::getName()
{
	return mName;
}


void 
AosModule::setName(const std::string& name)
{
	mName = name;
}


u32 
AosModule::getWeight()
{
	return mWeight;
}


void 
AosModule::setWeight(const u32 weight)
{
	mWeight = weight;
}


u32 
AosModule::getMinNumCmds()
{
	return mMinNumCmds;
}


void 
AosModule::setMinNumCmds(const u32 num)
{
	mMinNumCmds = num;
}


u32 
AosModule::getMaxNumCmds()
{
	return mMaxNumCmds;
}


void 
AosModule::setMaxNumCmds(const u32 num)
{
	mMaxNumCmds = num;
}


bool 
AosModule::getStatus()
{
	return mStatus;
}


void 
AosModule::setStatus(const bool status)
{
	mStatus = status;
}


void 
AosModule::getCommands(std::list<std::string>& cmds)
{
	cmds = mCmdNames;
}


int 
AosModule::getNumCmds()
{
	return mCmdNames.size();
}


u32 
AosModule::getTotalIncorrectCmds()
{
	return mTotalIncorrectCmds;
}


u32 
AosModule::getTotalCorrectCmds()
{
	return mTotalCorrectCmds;
}


u32 
AosModule::getTotalIgnoreCmds()
{
	return mTotalIgnoreCmds;
}


u32 
AosModule::getTotalFailedChecks()
{
	return mTotalFailedChecks;
}


u32 
AosModule::getTotalCmdsRun()
{
	return mTotalCmdsRun;
}


bool 
AosModule::ModuleFactory(const std::string& configFile, AosModulePtr &module)
{
	TiXmlDocument doc(configFile);
	if (!doc.LoadFile())
	{
		OmnAlarm << "There is no module config file in the module directory" << enderr;
		return false;
	}
	TiXmlElement* element = doc.FirstChildElement();
	if (!element)
	{
		OmnAlarm << "There is no module definition in the config file" << enderr;
		return false;
	}
	if (element->ValueStr() == "FuncModule")
	{
		module = OmnNew AosFuncModule();	
	}
	else if (element->ValueStr() == "CliModule")
	{
		module = OmnNew AosCliModule();	
	}
	module->readFromFile(configFile);
	return true;
}
