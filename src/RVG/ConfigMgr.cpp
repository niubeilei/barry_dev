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
// Monday, January 14, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#include "RVG/ConfigMgr.h"
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "Alarm/Alarm.h"


AosConfigMgr::AosConfigMgr()
{
}


AosConfigMgr::~AosConfigMgr()
{
}


bool 
AosConfigMgr::getPlatformDir(OmnString & dir)
{
	const char* env = getenv(PLATFORM_DIR_ENV);
	if (env)
	{
		dir = env;
	}
	if (dir.empty())
	{
		OmnAlarm << "The environment variable "
				<< PLATFORM_DIR_ENV
				<<" is not defined." 
				<< "Please define the environment variable to the torturer platform directory."
				<< enderr;
		return false;
	}
	return true;
}


bool 
AosConfigMgr::getConfigRootDir(OmnString &dir)
{
   	if (!getPlatformDir(dir))
	{
		return false;
	}
	dir += CONFIG_DATA_DIRECTORY;
	if (!checkDir(dir))
	{
		return false;
	}

	return true;
}

//
// check the product direcotry whether it is exist or not. if not exist, create it and its sub directories,
// else do nothing and return true. 
// The directory path is : 
//		PLATFORM_DIR_ENV/ConfigData/<productName>
//										--Table
//										--RVG
//										--Command
//										--Module
//										--Torturer
//
bool 
AosConfigMgr::checkProductDir(const OmnString &dir)
{
	//
	// check whether the product directory exist or not. 
	// if not exist, create it.
	//
	if (!checkDir(dir))
	{
		return false;
	}
	else
	{
		OmnString torturer, module, cmd, table, rvg;
		torturer = dir + TORTURER_SUB_DIR;
		module = dir + MODULE_SUB_DIR;
		cmd = dir + COMMAND_SUB_DIR;
		table = dir + TABLE_SUB_DIR;
		rvg = dir + RVG_SUB_DIR;
		if (!checkDir(torturer))
		{
			return false;
		}
		if (!checkDir(module))
		{
			return false;
		}
		if (!checkDir(cmd))
		{
			return false;
		}
		if (!checkDir(table))
		{
			return false;
		}
		if (!checkDir(rvg))
		{
			return false;
		}
	}

	return true;
}


//
// Get the product directory
// The directory path is : 
//		PLATFORM_DIR_ENV/ConfigData/<productName>
//
bool 
AosConfigMgr::getProductDir(const OmnString &productName, OmnString &productDir)
{
   	if (!getPlatformDir(productDir))
	{
		return false;
	}
	productDir += CONFIG_DATA_DIRECTORY;
	productDir += productName + "/";
	return true;
}


//
// Get the configuration directory of different ConfigDataType
// The directory path is : 
//		PLATFORM_DIR_ENV/ConfigData/<productName>/RVG/	or
//		PLATFORM_DIR_ENV/ConfigData/<productName>/Table/		or
//		PLATFORM_DIR_ENV/ConfigData/<productName>/Command/	or
//		PLATFORM_DIR_ENV/ConfigData/<productName>/Module/	or
//		PLATFORM_DIR_ENV/ConfigData/<productName>/Torturer/
//
bool 
AosConfigMgr::getConfigDir(const OmnString &productName, const AosConfigMgr::ConfigDataType configDataType, OmnString &configDir)
{
	OmnString productDir;
	if (!getProductDir(productName, productDir))
	{
		return false;
	}
	if (!checkProductDir(productDir))
	{
		return false;	
	}

	configDir = productDir;

	switch (configDataType)
	{
		case AosConfigMgr::eTable:
			configDir += TABLE_SUB_DIR;
			break;	
		case AosConfigMgr::eRVG:
			configDir += RVG_SUB_DIR;
			break;	
		case AosConfigMgr::eCommand:
			configDir += COMMAND_SUB_DIR;
			break;	
		case AosConfigMgr::eModule:
			configDir += MODULE_SUB_DIR;
			break;	
		case AosConfigMgr::eTorturer:
			configDir += TORTURER_SUB_DIR;
			break;	
		default:
			OmnAlarm << "Unknow config data type" << enderr;
			return false;
	}
	return true;
}


//
// Get the config file name
// The directory path is : 
//		PLATFORM_DIR_ENV/ConfigData/<productName>/<ConfigType>/objectName.xml
//
bool 
AosConfigMgr::getConfigFileName(const OmnString &productName, 
					const AosConfigMgr::ConfigDataType configDataType, 
					const OmnString &objectName, 
					OmnString &configFileName)
{
	OmnString dir;
	if (!getConfigDir(productName, configDataType, dir))
	{
		return false;
	}
	configFileName = dir;
	configFileName += objectName + CONFIG_FILE_SUFFIX;
	return true;
}


bool	
AosConfigMgr::getDestGenCodeDir(const OmnString &productName, const OmnString &torturerName, OmnString &dir)
{
	OmnString platformDir;
	if (!getPlatformDir(platformDir))
	{
		return false;
	}
	if (productName.empty())
	{
		OmnAlarm << "The product name is empty" << enderr;
		return false;
	}
	if (torturerName.empty())
	{
		OmnAlarm << "The torturer name is empty" << enderr;
		return false;
	}

	dir = platformDir;
	dir += "/GenCode/";
	dir += productName;
	if (!checkDir(dir))
	{
		return false;
	}

	dir += "/" + torturerName;
	if (!checkDir(dir))
	{
		return false;
	}

	return true;
}


//
// check whether the directory exist or not. 
// if not exist, create it.
//
bool 
AosConfigMgr::checkDir(const OmnString &dir)
{
	if (access(dir.c_str(), F_OK) != 0)
	{
		perror("access");
		if (mkdir(dir.c_str(), 0777))
		{
			if (errno != EEXIST)
			{
				perror("mkdir");
				OmnAlarm << "mkdir error" << enderr;
				return false;	
			}
		}
	}
	return true;
}


//
// get directory name list
//
bool 
AosConfigMgr::getDirNameList(const OmnString &parentDir,
								std::list<OmnString> &dirNameList)
{
	OmnNotImplementedYet;
	/*
	 * Need to replace the std::string portion, Chen Ding, 12/16/2008
	struct dirent ** nameList;
	int n;
	n = scandir(parentDir.c_str(), &nameList, 0, versionsort);
	if (n >= 0)
	{
		while (n--)
		{
			if (std::string(nameList[n]->d_name)==std::string(".") ||
					std::string(nameList[n]->d_name)==std::string(".."))
			{
				free(nameList[n]);
				continue;
			}

			if (nameList[n]->d_type == DT_DIR)
			{
				dirNameList.push_back(nameList[n]->d_name);
			}
			free(nameList[n]);
		}
		free(nameList);
	}
	*/
	return true;
}


//
// get filename list, the suffix of the file must be CONFIG_FILE_SUFFIX
//
bool 
AosConfigMgr::getConfigFileNameList(const OmnString &parentDir,
								std::list<OmnString> &filenameList)
{
	OmnNotImplementedYet;
	/*
	 * Need to replace the std::string portion, Chen Ding, 12/16/2008
	struct dirent ** nameList;
	int n;
	n = scandir(parentDir.c_str(), &nameList, 0, versionsort);
	if (n >= 0)
	{
		while (n--)
		{
			if (std::string(nameList[n]->d_name)==std::string(".") ||
					std::string(nameList[n]->d_name)==std::string(".."))
			{
				free(nameList[n]);
				continue;
			}

			if (nameList[n]->d_type != DT_DIR)
			{
				//
				// check the file suffix whether it is .xml 
				//
				std::string name = nameList[n]->d_name;
				int len = name.length();
				int stdLen = strlen(CONFIG_FILE_SUFFIX);
				if (len <= stdLen)
				{
					free(nameList[n]);
					continue;
				}
				std::string suffix = name.substr(len-stdLen);
				if (suffix == CONFIG_FILE_SUFFIX)
				{
					//name = name.substr(0, len-stdLen);
					filenameList.push_back(name);
				}
			}
			free(nameList[n]);
		}
		free(nameList);
	}
	*/
	return true;
}

