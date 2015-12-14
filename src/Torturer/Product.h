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

#ifndef Aos_Torturer_Product_h
#define Aos_Torturer_Product_h

#include "Util/RCObject.h"
#include "RVG/RVG.h"
#include "Torturer/Command.h"
#include "Torturer/Module.h"
#include "Torturer/Torturer.h"
#include "Util/RCObject.h"

class AosProduct : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	std::string 		mName;

public:
	//
	// This torturer pool is used for editing the torutrer
	// configuration setting from GUI, it is not runtime torturer pool
	//
	AosTorturerPtrArray mTorturers;

	//
	// Module pool for configuration not runtime
	//
	AosModulePtrArray	mModules;

	//
	// command pool for configuration not runtime
	//
	AosCommandPtrArray	mCommands;

	//
	// RVG pool for configuration not runtime
	//
	AosRVGPtrArray		mRVGs;

	//
	// This table pool is used for editing the tables from GUI,
	// it is not runtime table pool.
	//
	AosTablePtrArray	mTables;

public:
	AosProduct();
	AosProduct(const std::string &name);
	~AosProduct();

	bool init();

	std::string getName();
	void setName(const std::string& name);

private:
	bool initRVGs();
	bool initTables();
	bool initCommands();
	bool initModules();
	bool initTorturers();

};


typedef std::vector<AosProductPtr> AosProductPtrArray;


class AosProductMgr : virtual public OmnRCObject
{
public:
	static AosProductPtrArray 	mProducts;
	static AosProductPtr		mGlobalData;

public:
	AosProductMgr();
	~AosProductMgr();

	//
	// Initialize all the product configuration data, then GUI 
	// can work based on these data. 
	//
	static bool init();
	static bool getProduct(const std::string& productName, AosProductPtr& product);

};

#endif
