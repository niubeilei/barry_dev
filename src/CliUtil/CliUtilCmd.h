////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCmd.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliUtil_CliUtilCmd_h
#define Omn_CliUtil_CliUtilCmd_h

#include "CliUtil/CliUtil.h"

#include "aos/KernelEnum.h"
#include "CliUtil/Ptrs.h"
#include "KernelInterface/CliLevel.h"
#include "KernelInterface/CliCmd.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "Util/OmnNew.h"

class OmnStrParser;
class OmnCliUtilCmd : public OmnRCObject
{
	OmnDefineRCObject;
	
protected:
	OmnString				mPrefix;
	OmnString				mUsage;
	OmnVList<OmnCliParmPtr>	mParms;
	int						mBufsize;
	OmnString				mOprId;
	OmnString				mModId;
	AosCliLvl::CliLevel		mLevel;
	static bool				mLoadConfig;	// Indicate loading config

public:
	OmnCliUtilCmd(const OmnString &def);
	OmnCliUtilCmd();
	virtual ~OmnCliUtilCmd() ;

	OmnString		getPrefix() const {return mPrefix;}
	//
	// mod by lxx for module cli 03/13/2006
	//
	virtual bool	run(const OmnString &cmd,const int parmPos, OmnString &rslt);
	virtual bool	runUserLand(const OmnString &parms, OmnString &rslt);
	virtual bool	checkDef(OmnString &rslt);

	static void 	setLoadConfig(const bool b) {mLoadConfig = b;}
	static bool		isLoadConfig() {return mLoadConfig;}
	
	OmnString getOprId()const {return mOprId;}
	OmnString getModId()const {return mModId;}

	bool		runAppPreparing(const OmnString &parms, 
				  char **data, 
				  unsigned int *totalLen,
				  OmnString &rslt);
				  
	AosCliLvl::CliLevel		getLevel()const;
	
	aosCliFunc mFunc;
	
private:

#ifdef AOS_OLD_CLI
	virtual bool	saveConfig(const OmnString &parms, OmnString &rslt);
	virtual bool	loadConfig(const OmnString &parms, OmnString &rslt);
	bool	clearConfig(OmnString &rslt);
#endif

	bool	appendIntToStr(OmnString &rslt, int ret);
	
	bool 			retrieveInMemLog(const OmnString &parms, OmnString &rslt);
	
	int sendToUserLand(char *data, unsigned int size);
	bool aosUserLandApi_convert(char *data,
						 unsigned int datalen,
						 struct aosUserLandApiParms *parms);
};

#ifndef OmnDeclareCliCmd
#define OmnDeclareCliCmd(x, str) \
class x : public OmnCliUtilCmd \
{ \
public: \
	x() {mCmdStr = (str);} \
	virtual ~x() {} \
	virtual bool	run(const OmnString &cmd,const int parmPos, OmnString &rslt); \
};
#endif

#endif

