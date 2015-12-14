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
// Wednesday, January 16, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#include "Torturer/Product.h"
#include "RVG/ConfigMgr.h"
#include "Util/OmnNew.h"
#include "Util/RCObjImp.h"


AosProduct::AosProduct()
{
}


AosProduct::AosProduct(const std::string &name)
:
mName(name)
{
}


AosProduct::~AosProduct()
{
}


//
// initialize all configuration data
//
bool 
AosProduct::init()
{
	if (!initTables())
	{
		return false;
	}
	if (!initRVGs())
	{
		return false;
	}
	if (!initCommands())
	{
		return false;
	}
	if (!initModules())
	{
		return false;
	}
	if (!initTorturers())
	{
		return false;
	}

	return true;
}


bool
AosProduct::initTables()
{
	std::string dir;
	//
	// initialize the Table data
	//
	if (!AosConfigMgr::getConfigDir(mName, AosConfigMgr::eTable, dir))
	{
		OmnAlarm << "Get Table directory error" << enderr;
		return false;
	}
	std::list<std::string> nameList;
	AosConfigMgr::getConfigFileNameList(dir, nameList);
	std::list<std::string>::iterator iter;
	for (iter=nameList.begin(); iter!=nameList.end(); iter++)
	{
		std::string tmp = dir + *iter; 
		AosTablePtr table;
	   	if (AosTable::createInstance(tmp, table))
		{
			mTables.push_back(table);
		}
		else
		{
			OmnAlarm << "Create table instance error" << enderr;
			return false;
		}
	}
	return true;
}


bool
AosProduct::initRVGs()
{
	std::string dir;
	//
	// initialize the RVG data
	//
	if (!AosConfigMgr::getConfigDir(mName, AosConfigMgr::eRVG, dir))
	{
		OmnAlarm << "Get RVG directory error" << enderr;
		return false;
	}
	std::list<std::string> nameList;
	AosConfigMgr::getConfigFileNameList(dir, nameList);
	std::list<std::string>::iterator iter;
	for (iter=nameList.begin(); iter!=nameList.end(); iter++)
	{
		std::string tmp = dir + *iter; 
		AosRVGPtr rvg = AosRVG::RVGFactory(tmp);
		if (rvg)
		{
			mRVGs.push_back(rvg);
		}
		else
		{
			OmnAlarm << "Create RVG instance error" << enderr;
			return false;
		}
	}
	return true;
}


bool
AosProduct::initCommands()
{
	std::string dir;
	//
	// initialize the Command data
	//
	if (!AosConfigMgr::getConfigDir(mName, AosConfigMgr::eCommand, dir))
	{
		OmnAlarm << "Get command directory error" << enderr;
		return false;
	}
	std::list<std::string> nameList;
	AosConfigMgr::getConfigFileNameList(dir, nameList);
	std::list<std::string>::iterator iter;
	for (iter=nameList.begin(); iter!=nameList.end(); iter++)
	{
		std::string tmp = dir + *iter; 
		AosCommandPtr cmd;
	    if (AosCommand::commandFactory(tmp, cmd))
		{
			mCommands.push_back(cmd);
		}
		else
		{
			OmnAlarm << "Create Command instance error" << enderr;
			return false;
		}
	}
	return true;
}


bool
AosProduct::initModules()
{
	std::string dir;
	//
	// initialize the Module data
	//
	if (!AosConfigMgr::getConfigDir(mName, AosConfigMgr::eModule, dir))
	{
		OmnAlarm << "Get module directory error" << enderr;
		return false;
	}
	std::list<std::string> nameList;
	AosConfigMgr::getConfigFileNameList(dir, nameList);
	std::list<std::string>::iterator iter;
	for (iter=nameList.begin(); iter!=nameList.end(); iter++)
	{
		std::string tmp = dir + *iter; 
		AosModulePtr module;
	    if (AosModule::ModuleFactory(tmp, module))
		{
			mModules.push_back(module);
		}
		else
		{
			OmnAlarm << "Create module instance error" << enderr;
			return false;
		}
	}
	return true;
}


bool
AosProduct::initTorturers()
{
	std::string dir;
	//
	// initialize the torturer 
	//
	if (!AosConfigMgr::getConfigDir(mName, AosConfigMgr::eTorturer, dir))
	{
		OmnAlarm << "Get Torturer directory error" << enderr;
		return false;
	}
	std::list<std::string> nameList;
	AosConfigMgr::getConfigFileNameList(dir, nameList);
	std::list<std::string>::iterator iter;
	for (iter=nameList.begin(); iter!=nameList.end(); iter++)
	{
		std::string tmp = dir + *iter; 
		AosTorturerPtr torturer;
	    if (AosTorturer::createInstance(tmp, torturer))
		{
			mTorturers.push_back(torturer);
		}
		else
		{
			OmnAlarm << "Create Torturer instance error" << enderr;
			return false;
		}
	}
	return true;
}


std::string 
AosProduct::getName()
{
	return mName;
}


void 
AosProduct::setName(const std::string& name)
{
	mName = name;
}



AosProductPtrArray AosProductMgr::mProducts;

AosProductPtr AosProductMgr::mGlobalData;

AosProductMgr::AosProductMgr()
{
}


AosProductMgr::~AosProductMgr()
{
}


//
// Initialize all the product configuration data, then GUI 
// can work based on these data. 
//
bool 
AosProductMgr::init()
{
	//
	// Get the name list of the products from directory /ConfigData/
	// then initialize each product
	//
	std::string configDir;
	if (!AosConfigMgr::getConfigRootDir(configDir))
	{
		OmnAlarm << "Get the config data root directory error" << enderr;
		return false;
	}
	std::list<std::string> dirList;
	AosConfigMgr::getDirNameList(configDir, dirList);
	std::list<std::string>::iterator iter;
	for (iter=dirList.begin(); iter!=dirList.end(); iter++)
	{
		//
		// Get one of the product directories
		// create a product object and init it
		//
		if (*iter == "CVS")
		{
			continue;
		}

		AosProductPtr ptr = OmnNew AosProduct(*iter);
		if (ptr->init())
		{
			if (*iter == std::string(GLOBAL_DATA_DIR_NAME))
			{
				mGlobalData = ptr;
			}
			else
			{
				mProducts.push_back(ptr);
			}
		}
		else
		{
			OmnAlarm << "The initialization of the product failed, product name: " 
				<< *iter
				<< enderr;
		}
	}

	return true;
}


bool 
AosProductMgr::getProduct(const std::string& productName, AosProductPtr& product)
{
	if (productName == GLOBAL_DATA_DIR_NAME)
	{
		product = mGlobalData;
		return true;
	}
	int productNum = mProducts.size();
	for (int i=0; i<productNum; i++)
	{
		if (productName == mProducts[i]->getName())
		{
			product = mProducts[i];
			return true;
		}
	}
	return false;
}

