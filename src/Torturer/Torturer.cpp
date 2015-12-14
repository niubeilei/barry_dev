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
// 01/03/2008: created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#include "Torturer/Torturer.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "ProgramAid/Util.h"
#include "Random/RandomInteger.h"
#include "RVG/ConfigMgr.h"
#include "SemanticData/VarContainer.h"
#include "Torturer/Ptrs.h"
#include "Tester/TestMgr.h"
#include "Torturer/TorUtil.h"
#include "Util/VarList.h"
#include "Util/File.h"
#include "Util1/Time.h"
#include "XmlLib/tinyxml.h"


static std::string	sgAosHome;

bool AosIsLoggingCmd = false;

AosTorturerLogType::E	AosTorturer::mLogType;
bool 					AosTorturer::mTestOnly = false;
AosCorCtrl::E			AosTorturer::mCorrectCtrl = AosCorCtrl::eRandom;
std::string				AosTorturer::mInitFuncs;
std::list<std::string>	AosTorturer::mLogIncludes;
AosVarContainerPtr		AosTorturer::mVariables;
AosTablePtrArray		AosTorturer::mTables;

AosTorturer::AosTorturer(const std::string& torturerName, const std::string& productName)
:
mTorturerName(torturerName),
mProductName(productName),
mIsGood(false),
mNumCmdsToRun(0),
mTorturingTime(eDefaultTorturingTime),
mTotalCmdsCollected(0),
mNumModulesRun(0),
mTotalCmdsRun(0),
mLogFile(NULL)
//mLogType(AosTorturerLogType::eNoLog)
{
}


AosTorturer::AosTorturer()
:
mIsGood(false),
mNumCmdsToRun(0),
mTorturingTime(eDefaultTorturingTime),
mTotalCmdsCollected(0),
mNumModulesRun(0),
mTotalCmdsRun(0),
mLogFile(NULL)
//mLogType(AosTorturerLogType::eNoLog)
{
}


AosTorturer::~AosTorturer()
{
	if (mLogFile)
	{
		std::string str; 
		str += "    std::cout << \"Finished executing all functions. \" << std::endl;\n";
		str += "    return true;\n";
		str += "}\n";
		mLogFile->append(str.c_str());
	}
}


bool
AosTorturer::start()
{
	createModuleSelector();

	if (!mTotalCmdsCollected)
	{
		OmnAlarm << "No commands collected!" << enderr;
		return false;
	}

	int startTime = OmnTime::getSecTick();
	mNumModulesRun = 0;
	mTotalCmdsRun = 0;

	if (mNumCmdsToRun == 0 && mTorturingTime == 0) 
	{
		mTorturingTime = eDefaultTorturingTime;
	}

	while (1)
	{
		// 
		// Randomly select a module and run it
		//
		run();
			
		if (mTorturingTime)
		{
			if (OmnTime::getSecTick() - startTime >= mTorturingTime)
			{
				break;
			}
		}
		else if (mTotalCmdsRun >= mNumCmdsToRun)
		{
			break;
		}
	}

	u32 totalIncorrectCmds = 0;
	u32 totalCorrectCmds = 0;
	u32 totalFailedChecks = 0;
	u32 totalCmds = 0;
	u32 totalIgnoreCmds = 0;
	int moduleNum = mModules.size();
	for (int i=0; i<moduleNum; i++)
	{
		totalIncorrectCmds += mModules[i]->getTotalIncorrectCmds();
		totalCorrectCmds += mModules[i]->getTotalCorrectCmds();
		totalFailedChecks += mModules[i]->getTotalFailedChecks();
		totalCmds += mModules[i]->getTotalCmdsRun();
		totalIgnoreCmds += mModules[i]->getTotalIgnoreCmds();
	}

	cout << "Torturing Time:           " << OmnTime::getSecTick() - startTime << endl;
	cout << "Total Modules Run:        " << mNumModulesRun << endl;
	cout << "Total Commands Ignore:    " << totalIgnoreCmds << endl;
	cout << "Total Commands Run:       " << totalCmds << endl;
	cout << "Total Incorrect Commands: " << totalIncorrectCmds  << endl;
	cout << "Total Correct Commands:   " << totalCorrectCmds  << endl;
	cout << "Total Failed checks:      " << totalFailedChecks << endl;
	cout << "Correct Percent:          " << totalCorrectCmds * 100.0 / totalCmds << endl;
	cout << "Incorrect Percent:        " << totalIncorrectCmds * 100.0 / totalCmds << endl;

	printTables();
	return true;
}


bool
AosTorturer::createModuleSelector()
{
	mModuleSelector.clearIntegerPair();
	mTotalCmdsCollected = 0;
	int moduleNum = mModules.size();
	for (int i=0; i<moduleNum; i++)
	{
		if (mModules[i]->getNumCmds() > 0)
		{
			mTotalCmdsCollected += mModules[i]->getNumCmds();
			mModuleSelector.setIntegerPair(i, i, mModules[i]->getWeight());
		}
	}

	return true;
}


bool 
AosTorturer::run()
{
	static u32 count = 10000;

	// 
	// Randomly select a module to run
	//
	u32 cmdsRun = 0;
	AosValue value;
	if (!mModuleSelector.nextValue(value))
	{
		OmnAlarm << "Module selector error" << enderr;
		return false;
	}
	int index = value.toUint32();
	mModules[index]->runCommands(cmdsRun, mTestMgr);
	mNumModulesRun++;
	mTotalCmdsRun += cmdsRun;

	if (mTotalCmdsRun > count)
	{
		cout << "Commands Run: " << count << endl;
		count += 10000;
	}

	// 
	// Check all the modules
	//
	int moduleNum = mModules.size();
	for (int i=0; i<moduleNum; i++)
	{
		mModules[i]->check();
	}
	return true;
}


OmnTestMgrPtr
AosTorturer::getTestMgr() const
{
	return mTestMgr;
}

/*
bool
AosTorturer::setVar(const std::string &name, const std::string &value)
{
	if (!mVariables)
	{
		OmnAlarm << "mVariables is null: " << name << ":" << value << enderr;
		return false;
	}

	bool ret = mVariables->setValue(name.c_str(), value.c_str(), true);
	return ret;
}


bool
AosTorturer::setVar(const std::string &name, const int value)
{
	std::string str;
	str = AosValue::itoa(value);
	return setVar(name.c_str(), str.c_str());
}
*/

AosValuePtr
AosTorturer::getVar(const std::string &name) const
{
	aos_assert_r(!mVariables, 0);
	return mVariables->getValue(name);
}


AosVarContainerPtr
AosTorturer::getVariables()
{
	return mVariables;
}


OmnFilePtr 
AosTorturer::getLogFile() const
{
	return mLogFile;
}


AosTorturerLogType::E
AosTorturer::getLogType()
{
	return mLogType;
}


std::string			
AosTorturer::getName()
{
	return mTorturerName;
}


void				
AosTorturer::setName(const std::string& name)
{
	mTorturerName = name;
}


std::string			
AosTorturer::getLogFileName()
{
	return mLogFilename;
}


void				
AosTorturer::setLogFileName(const std::string& filename)
{
	mLogFilename = filename;
}


OmnFilePtr
AosTorturer::createNextLogFile()
{
	static int lsFileIndex = 0;

	if (mLogFile)
	{
		mLogFile->closeFile();
		AosCommand::setLogfile(0);
	}

	std::string fn = mLogFilename;
	fn += "_";
   	fn += AosValue::itoa(lsFileIndex);
  	fn += ".cpp";
	mLogFile = OmnNew OmnFile(fn.c_str(), OmnFile::eCreate);
	lsFileIndex++;
	if (!mLogFile->isGood())
	{
		OmnAlarm << "Failed to open the file: " 
			<< fn << enderr;
		return 0;
	}

	AosCommand::setLogfile(mLogFile);
	return mLogFile;
}


int
AosTorturer::getNextFuncIndex()
{
	static int lsFuncIndex = 1;
	return lsFuncIndex++;
}


bool 
AosTorturer::writeToFile()
{
	if (mProductName.empty() || mTorturerName.empty())
	{
		return false;
	}
	std::string configFile;
	if (!AosConfigMgr::getConfigFileName(mProductName, AosConfigMgr::eTorturer, mTorturerName, configFile))
	{
		OmnAlarm << "read config file error" << enderr;
		return false;	
	}
	return writeToFile(configFile);
}


bool 
AosTorturer::readFromFile()
{
	if (mProductName.empty() || mTorturerName.empty())
	{
		return false;
	}
	std::string configFile;
	if (!AosConfigMgr::getConfigFileName(mProductName, AosConfigMgr::eTorturer, mTorturerName, configFile))
	{
		OmnAlarm << "read config file error" << enderr;
		return false;	
	}
	return readFromFile(configFile);
}


bool 
AosTorturer::writeToFile(const std::string &configFile)
{
	// 
	// The configure file is in the format of
	//  <TorturerInfo>
	//      <Name>
	//      <ProductName>
	//      <NumCommands>
	//      <TorturingTime>
	//      <LogType>
	//      <LogFileName>
	//      <CorrectCtrl>
	//  </TorturerInfo>
	// 	<Tables>
	// 		<TableName></TableName>
	// 		<TableName></TableName>
	// 		...
	// 	</Tables>
	// 	<Modules>
	//		<ModuleName></ModuleName>
	//		...
	// 	</Modules>
	// 	<LogIncludes>
	// 		...
	// 	</LogIncludes>
	// 	<InitVars>
	// 		name value
	// 		name value
	// 	</InitVars>
	//	<AdditionalLibs>
	//		<Lib></Lib>
	//		<Lib></Lib>
	//		...
	//	</AdditionalLibs>
	//
	TiXmlDocument doc(configFile);
	TiXmlElement node("TorturerInfo");
	if (!addTorturerInfo(node))
	{
		OmnAlarm << "add TorturerInfo error" << enderr;
		return false;	
	}
	doc.InsertEndChild(node);

	TiXmlElement tables("Tables");
	if (addTablesConfig(node))
	{
		doc.InsertEndChild(tables);
	}

	TiXmlElement modules("Modules");
	if (addTablesConfig(node))
	{
		doc.InsertEndChild(modules);
	}

	TiXmlElement libs("AdditionalLibs");
	if (addLibsConfig(node))
	{
		doc.InsertEndChild(libs);
	}
	doc.SaveFile();
	return true;
}


bool
AosTorturer::addTorturerInfo(TiXmlNode& node)
{
	node.addElement("Name", mTorturerName);
	node.addElement("ProductName", mProductName);
	node.addElement("NumCommands", AosValue::itoa(mNumCmdsToRun));
	node.addElement("TorturingTime", AosValue::itoa(mTorturingTime));
	//
	//      <LogType>
	//      <LogFileName>
	//      <CorrectCtrl>
	//
	return true;
}


bool
AosTorturer::addTablesConfig(TiXmlNode& node)
{
	if (mTables.empty())
	{
		return false;
	}

	int tableNum = mTables.size();
	for (int i=0; i<tableNum; i++)
	{
		node.addElement("TableName", mTables[i]->getName());
	}

	return true;
}


bool
AosTorturer::addModulesConfig(TiXmlNode& node)
{
	if (mModules.empty())
	{
		return false;
	}

	int num = mModules.size();
	for (int i=0; i<num; i++)
	{
		node.addElement("ModuleName", mModules[i]->getName());
	}

	return true;
}


bool
AosTorturer::addLibsConfig(TiXmlNode& node)
{
	if (mAdditionalLibs.empty())
	{
		return false;
	}

	std::list<std::string>::iterator iter;
	for (iter=mAdditionalLibs.begin(); iter!=mAdditionalLibs.end(); iter++)
	{
		node.addElement("Lib", *iter);
	}

	return true;
}


bool 
AosTorturer::readFromFile(const std::string &configFile)
{
	// 
	// The configure file is in the format of
	//  <TorturerInfo>
	//      <Name>
	//      <ProductName>
	//      <NumCommands>
	//      <TorturingTime>
	//      <LogType>
	//      <LogFileName>
	//      <CorrectCtrl>
	//  </TorturerInfo>
	// 	<Tables>
	// 		<TableName></TableName>
	// 		<TableName></TableName>
	// 		...
	// 	</Tables>
	// 	<Modules>
	//		<ModuleName></ModuleName>
	//		...
	// 	</Modules>
	// 	<LogIncludes>
	// 		<Include></Include>
	// 	</LogIncludes>
	// 	<InitVars>
	// 		name value
	// 	</InitVars>
	//	<AdditionalLibs>
	//		<Lib></Lib>
	//		<Lib></Lib>
	//		...
	//	</AdditionalLibs>
	// 	<InitFuncs>
	// 	</InitFuncs>
	// 	<InitIncludes>
	// 		<Include></Include>
	// 		<Include></Include>
	// 	</InitIncludes>
	//
	TiXmlDocument doc(configFile);
	if (!doc.LoadFile())
	{
		OmnAlarm << "The torturer config file doesn't exist or syntax error." 
				<< doc.ErrorDesc()
				<< ":"
				<< doc.ErrorRow()
				<< ":"
				<< doc.ErrorId()
				<< enderr;
		return false;
	}
	TiXmlElement* child = doc.FirstChildElement();
	for (; child!=NULL; child=child->NextSiblingElement())
	{
		std::string tagName = child->ValueStr();
		if (tagName == "TorturerInfo")
		{
			if (!parseTorturerInfo(child))
			{
				OmnAlarm << "Failed to parse TorturerInfo" << enderr;
				return false;
			}
		}
		else if (tagName == "Tables")
		{
			if (!parseTables(child))
			{
				OmnAlarm << "Failed to parse Tables" << enderr;
				return false;
			}
		}
		else if (tagName == "Modules")
		{
			if (!parseModules(child))
			{
				OmnAlarm << "Failed to parse Modules" << enderr;
				return false;
			}
		}
		else if (tagName == "InitVars")
		{
		}
		else if (tagName == "LogIncludes")
		{
			if (!parseLogIncludes(child))
			{
				OmnAlarm << "Failed to parse LogIncludes" << enderr;
				return false;
			}
		}
		else if (tagName == "AdditionalLibs")
		{
			if (!parseAdditionalLibs(child))
			{
				OmnAlarm << "Failed to parse AdditionalLibs" << enderr;
				return false;
			}
		}
		else if (tagName == "InitIncludes")
		{
			if (!parseInitIncludes(child))
			{
				OmnAlarm << "Failed to parse InitIncludes" << enderr;
				return false;
			}
		}
		else if (tagName == "InitFuncs")
		{
			if (!parseInitFuncs(child))
			{
				OmnAlarm << "Failed to parse InitFuncs" << enderr;
				return false;
			}
		}
		else
		{
			OmnAlarm << "Unrecognized tag: " 
				<< child << enderr;
			return false;
		}
	}
	
	return true;
}

bool 
AosTorturer::parseTorturerInfo(TiXmlNode* node)
{
	if (node == NULL)
	{
		return false;
	}
	//
	// parse name, mandatory field
	//
	if (!node->getElementTextValue("Name", mTorturerName))
	{
		OmnAlarm << "XML parse: Name must exist in torturer config file" << enderr;
		return false;
	}
	//
	// parse productName, mandatory field
	//
	if (!node->getElementTextValue("ProductName", mProductName))
	{
		OmnAlarm << "XML parse: ProductName must exist in torturer config file" << enderr;
		return false;
	}
	//
	// parse NumCommands, mandatory field
	//
	if (!node->getElementTextValue("NumCommands", mNumCmdsToRun))
	{
		OmnAlarm << "XML parse: NumCommands must exist in torturer config file" << enderr;
		return false;
	}
	//
	// parse TorturingTime, mandatory field
	//
	if (!node->getElementTextValue("TorturingTime", mTorturingTime))
	{
		OmnAlarm << "XML parse: TorturingTime must exist in torturer config file" << enderr;
		return false;
	}

	if (mNumCmdsToRun <= 0 && mTorturingTime <= 0)
	{
		// 
		// The default running one hour is assumed.
		//
		mNumCmdsToRun = 0;
		mTorturingTime = eDefaultTorturingTime;
	}
	else if (mTorturingTime > 0)
	{
		mNumCmdsToRun = 0;
	}
	else
	{
		mTorturingTime = 0;
	}
	//
	// parse LogType, mandatory field
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("LogType").ToElement();
	if (element)
	{
		const char* typeStr = element->GetText();
		if (typeStr)
		{
			mLogType = AosTorturerLogType::str2Enum(typeStr);
		}
	}
	else
	{
		OmnAlarm << "XML parse: LogType must exist in torturerInfo config file" << enderr;
		return false;
	}
	//
	// parse LogFileName, optional field
	//
	node->getElementTextValue("LogFileName", mLogFilename);
	//
	// parse CorrectCtrl, mandatory field
	//
	element = docHandle.FirstChild("CorrectCtrl").ToElement();
	if (element)
	{
		const char* str = element->GetText();
		if (str)
		{
			mCorrectCtrl = AosCorCtrl::str2Enum(str);
		}
	}
	else
	{
		OmnAlarm << "XML parse: CorrectCtrl must exist in torturerInfo config file" << enderr;
		return false;
	}

	return true;
}


bool 
AosTorturer::parseTables(TiXmlNode* node)
{
	//
	// 	<Tables>
	// 		<TableName></TableName>
	// 		<TableName></TableName>
	// 		...
	// 	</Tables>
	//
	if (node == NULL)
	{
		return false;
	}
	std::string configDir;
	if (!AosConfigMgr::getConfigDir(mProductName, AosConfigMgr::eTable, configDir))
	{
		return false;
	}

	TiXmlElement* child = node->FirstChildElement("TableName");
	for (; child!=NULL; child=child->NextSiblingElement("TableName"))
	{
		if (child->GetText())
		{
			std::string configFile = configDir;
			std::string tableName = child->GetText();	
			configFile += tableName;
			configFile += CONFIG_FILE_SUFFIX;
			AosTablePtr table;
			if (AosTable::createInstance(configFile, table))
			{
				addTable(table);
			}
		}
	}

	return true;
}


bool 
AosTorturer::parseModules(TiXmlNode* node)
{
	//
	// 	<Modules>
	// 		<ModuleName></ModuleName>
	// 		<ModuleName></ModuleName>
	// 		...
	// 	</Modules>
	//
	if (node == NULL)
	{
		return false;
	}
	std::string configDir;
	if (!AosConfigMgr::getConfigDir(mProductName, AosConfigMgr::eModule, configDir))
	{
		return false;
	}

	TiXmlElement* child = node->FirstChildElement("ModuleName");
	for (; child!=NULL; child=child->NextSiblingElement("ModuleName"))
	{
		if (child->GetText())
		{
			std::string configFile = configDir;
			std::string moduleName= child->GetText();	
			configFile += moduleName;
			configFile += CONFIG_FILE_SUFFIX;
			AosModulePtr module;
		   	if (AosModule::ModuleFactory(configFile, module))
			{
				addModule(module);
			}
		}
	}

	return true;
}


bool
AosTorturer::parseAdditionalLibs(TiXmlNode* node)
{
	//
	//	<AdditionalLibs>
	//		<Lib></Lib>
	//		<Lib></Lib>
	//		...
	//	</AdditionalLibs>
	//
	if (node == NULL)
	{
		return false;
	}
	TiXmlElement* child = node->FirstChildElement("Lib");
	for (; child!=NULL; child=child->NextSiblingElement("Lib"))
	{
		if (child->GetText())
		{
			mAdditionalLibs.push_back(child->GetText());
		}
	}

	return true;
}


bool
AosTorturer::parseInitIncludes(TiXmlNode* node)
{
	//
	// 	<InitIncludes>
	// 		<Include></Include>
	// 		<Include></Include>
	// 	</InitIncludes>
	//
	if (node == NULL)
	{
		return false;
	}
	TiXmlElement* child = node->FirstChildElement("Include");
	for (; child!=NULL; child=child->NextSiblingElement("Include"))
	{
		if (child->GetText())
		{
			std::string str = child->GetText();
			if (!str.empty())
			{
				if (str[0] != '#')
				{
					str = "#include \"";
					str += child->GetText();
					str += "\"";
				}
				mInitIncludes.push_back(str);
			}
		}
	}

	return true;
}


bool
AosTorturer::parseLogIncludes(TiXmlNode* node)
{
	//
	// 	<LogIncludes>
	// 		<Include></Include>
	// 		<Include></Include>
	// 	</LogIncludes>
	//
	if (node == NULL)
	{
		return false;
	}
	TiXmlElement* child = node->FirstChildElement("Include");
	for (; child!=NULL; child=child->NextSiblingElement("Include"))
	{
		if (child->GetText())
		{
			std::string str = child->GetText();
			if (!str.empty())
			{
				if (str[0] != '#')
				{
					str = "#include \"";
					str += child->GetText();
					str += "\"";
				}
				mLogIncludes.push_back(str);
			}
		}
	}

	return true;
}


bool
AosTorturer::parseInitFuncs(TiXmlNode* node)
{
	// 
	// <InitAPI>
	//     <Includes> 
	//     		...
	//     </Includes>
	//     <API>
	//     		...
	//     </API>
	// </InitAPI>
	//
/*	OmnXmlItemPtr item = def->getItem("Includes");

	if (item)
	{
		mInitIncludes = item->getStr();
		mInitIncludes.removeLeadingWhiteSpace();
	}

	item = def->getItem("API");
	if (!item)
	{
		OmnAlarm << "Missing API tag: " << def->toString() << enderr;
		return false;
	}

	OmnString str = item->getStr();
	while (str != "")
	{
		OmnString line = str.getLine(true);
		line.removeWhiteSpaces();
		mInitAPIs << "    " << line << "\n";
	}
*/	return true;
}


bool 
AosTorturer::addModule(const AosModulePtr& module)
{
	if (mModules.size() >= eMaxModules)
	{
		OmnAlarm << "Too many modules: " << eMaxModules << enderr;
		return false;
	}

	AosModulePtrArray::iterator iter;
	iter = std::find(mModules.begin(), mModules.end(), module);
	if (iter != mModules.end())
	{
		//
		// exist already
		//
		OmnWarn << "The module is already in the list" << enderr;
		return false;
	}
	mModules.push_back(module);

	return true;
}


bool 
AosTorturer::delModule(const AosModulePtr& module)
{
	mModules.erase(std::remove(mModules.begin(), mModules.end(), module), mModules.end());
	return true;
}


bool 
AosTorturer::init()
{
	if (!readFromFile())
	{
		return false;
	}

	mVariables = OmnNew AosVarContainer("CommandContainer");

	if (mTorturingTime > 0)
	{
		mNumCmdsToRun = 0;
	}

	if (mNumCmdsToRun == 0 && mTorturingTime == 0)
	{
		mTorturingTime = eDefaultTorturingTime;
	}

	return true;
}


bool
AosTorturer::printTables()
{
	int size = mTables.size();
	for (int i=0; i<size; i++)
	{
		std::cout << "Table: " << mTables[i]->getName() << std::endl;
		std::cout << mTables[i]->toString() << std::endl;
	}

	return true;
}


void 	
AosTorturer::getTables(std::vector<AosTablePtr> &tables)
{
	tables = mTables;
}


bool 
AosTorturer::getTable(const std::string &tableName, AosTablePtr& table)
{
	int size = mTables.size();
	for (int i=0; i<size; i++)
	{
		if (mTables[i]->getName() == tableName)
		{
			table = mTables[i];
			return true;
		}
	}
	return false;
}


bool 
AosTorturer::addTable(const AosTablePtr& table)
{
	if (!table)
	{
		return false;
	}
	AosTablePtrArray::iterator iter;
	iter = std::find(mTables.begin(), mTables.end(), table);
	if (iter != mTables.end())
	{
		//
		// exist already
		//
		OmnWarn << "The table is already in the list" << enderr;
		return false;
	}
	mTables.push_back(table);
	return true;
}


bool 
AosTorturer::delTable(const AosTablePtr& table)
{
	mTables.erase(std::remove(mTables.begin(), mTables.end(), table), mTables.end());
	return true;
}


AosCorCtrl::E
AosTorturer::getCorrectCtrlFlag()
{
	return mCorrectCtrl;
}


void 
AosTorturer::setCorrectCtrlFlag(AosCorCtrl::E flag)
{
	mCorrectCtrl = flag;
}


void
AosTorturer::getCommandNames(std::list<std::string>& cmdNames)
{
	std::list<std::string> cmds;
	int size = mModules.size();
	for (int i=0; i<size; i++)
	{
		mModules[i]->getCommands(cmds);
		cmdNames.insert(cmdNames.end(), cmds.begin(), cmds.end());
	}
	cmdNames.unique();
}


void 
AosTorturer::getLogIncludes(std::list<std::string>& includes)
{
	includes = mLogIncludes;
}


std::string 
AosTorturer::getInitFuncs()
{
	return mInitFuncs;
}


bool 
AosTorturer::createInstance(const std::string &configFile, AosTorturerPtr &torturer)
{
	torturer = OmnNew AosTorturer();
	return torturer->readFromFile(configFile);
}
