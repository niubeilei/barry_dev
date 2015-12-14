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
// The configuration directory structure is like this:
// /ConfigData/GlobalData
//		--Table
//		--RVG
//		--Command
// /ConfigData/<ProductName1>
//		--Table
//		--RVG
//		--Command
//		--Module
//		--Torturer
// /ConfigData/<ProductName2>
//		--Table
//		--RVG
//		--Command
//		--Module
//		--Torturer
// ...
//   
//
// Modification History:
// Monday, January 14, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_RVG_ConfigMgr_h
#define Aos_RVG_ConfigMgr_h

#include "Util/RCObject.h"
#include <string>
#include <list>

//
// XML configuration directory definition
//
#define PLATFORM_DIR_ENV "TORTURER_PLATFORM"
#define CONFIG_FILE_SUFFIX ".xml"
#define CONFIG_DATA_DIRECTORY "/ConfigData/"
#define GLOBAL_CONFIG_DATA_DIRECTORY "/ConfigData/GlobalData/"
#define GLOBAL_DATA_DIR_NAME "GlobalData"
#define COMMAND_SUB_DIR "Command/"
#define PARM_SUB_DIR "Parameter/"
#define RVG_SUB_DIR "RVG/"
#define TABLE_SUB_DIR "Table/"
#define MODULE_SUB_DIR "Module/"
#define TORTURER_SUB_DIR "Torturer/"

class AosConfigMgr : virtual public OmnRCObject
{
public:
	enum ConfigDataType
	{
		eTable = 0,
		eRVG,
		eCommand,
		eModule,
		eTorturer
	};

public:
	AosConfigMgr();
	~AosConfigMgr();

	static bool getPlatformDir(OmnString &dir);

	static bool getConfigRootDir(OmnString &dir);

	static bool getProductDir(const OmnString &productName, 
								OmnString &productDir);

	static bool getConfigDir(const OmnString &productName, 
								const AosConfigMgr::ConfigDataType configDataType, 
								OmnString &configDir);

	static bool getConfigFileName(const OmnString &productName, 
									const AosConfigMgr::ConfigDataType configDataType, 
									const OmnString &objectName, 
									OmnString &configFileName);
	static bool	getDestGenCodeDir(const OmnString &productName, 
									const OmnString &torturerName, 
									OmnString &dir);

	static bool getDirNameList(const OmnString &parentDir,
								std::list<OmnString> &dirNameList);

	static bool getConfigFileNameList(const OmnString &parentDir,
								std::list<OmnString> &filenameList);
 
private:
	static bool checkProductDir(const OmnString &dir);
	static bool checkDir(const OmnString &dir);


};

#endif

