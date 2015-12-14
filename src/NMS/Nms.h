////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Nms.h
// Description:
//	This class is used by HASS App to implement network management 
//  functions for a HASS application. If one wants to support network
//  management, it should generate an instance of this class (or its
//  derived class) and give this to OmnHassApp.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_Nms_h
#define Omn_NMS_Nms_h

//#include "Config/ConfigEntity.h"
#include "Debug/Rslt.h"
#include "Heartbeat/Ptrs.h"
#include "NMS/Ptrs.h"
#include "NMS/AppType.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"

class OmnHouseKpObj;
class OmnSmChgState;

OmnDefineSingletonClass(OmnNmsSingleton, 
						OmnNms,
						OmnNmsSelf,
						OmnSingletonObjId::eNms, 
						"Nms");


class OmnNms 
{
protected:
	// 
	// Chen Ding, 05/07/2003
	//
	enum
	{
		eDefaultProgLogFreq = 30
	};

	OmnHouseKpPtr			mHouseKp;
	OmnNmsServerPtr			mNmsServer;
	OmnNmsClientPtr			mNmsClient;

	static OmnString		mProgName;
	static OmnString		mStartTime;
	static OmnString		mVersion;
	static int				mBuildNo;
	static OmnString		mBuildTime;
	static OmnString		mPatch; 		// Chen Ding, 09/08/2003
	static OmnString		mProcId;
	static OmnString		mFailReason;
	static OmnString		mMachine;
	static OmnAppType::E	mAppType;
	static OmnString		mInstId;
	static bool				mIsPrimary;
	static OmnString		mLocation;

	// OmnSoProgLog	mProgLog;
	int				mLastLogTick;
	int				mProgLogFreq;

	//
	// Do not use the two
	//
	OmnNms(const OmnNms &rhs);
	OmnNms & operator = (const OmnNms &rhs);

public:
	OmnNms();
	virtual ~OmnNms();

	static bool	isPrimary() {return mIsPrimary;}
	static void	setMode(const bool b) {mIsPrimary = b;}

	static OmnNms *		getSelf();
	virtual bool		start();
	virtual bool		stop();
	virtual OmnRslt		config(const OmnXmlParserPtr &def);

	void		addHouseKeepObj(OmnHouseKpObj *obj);
	OmnString	getObjTotals() const;
	void		stopProg();
	bool		stopProg(const OmnString &name, OmnString &err);
	bool		isProgDefined(const OmnString &name);
	OmnTcpClientPtr	getNmsConn(const OmnString &name);
	bool		checkProgStatus(const OmnString &name, OmnString &rslt);
	OmnNmsClientPtr	getNmsClient() const;
	void		storeAppInfo() const;

	void		runNmsServer();
	
	OmnString	getProgName() const {return mProgName;}
	OmnString	getProcId() const {return mProcId;}
	OmnString	getStartTime() const {return mStartTime;}
	OmnString	getFailReason() const {return mFailReason;}
	OmnString	getMachine() const {return mMachine;}
	OmnIpAddr	getNmsAddr() const;
	int			getNmsPort() const;

	void		setNmsSvrObj(const OmnNmsSvrObjPtr &obj);

	void		setStartTime(const OmnString &s) {mStartTime = s;}

	// 
	// Chen Ding, 09/08/2003, 2003-0248
	//
	static void	setVersionInfo();
	static void	setVersion(const OmnString &s) {mVersion = s;}
	static OmnString	getVersion() {return mVersion;}

	static void	setBuildNo(const int &s) {mBuildNo = s;}
	static int	getBuildNo() {return mBuildNo;}

	static void	setBuildTime(const OmnString &s) {mBuildTime = s;}
	static OmnString	getBuildTime() {return mBuildTime;}

	static OmnString	getPatch() {return mPatch;}

	static void	setAppType(const OmnAppType::E t) {mAppType = t;}

	// 
	// Chen Ding, 06/15/2003, Ticket: 2003-0047
	//
	static void	setInstId(const OmnString &id) {mInstId=id;}
	static OmnString	getInstId() {return mInstId;}

	static bool	isAlg() {return mAppType == OmnAppType::eAlg;}
	static bool isMediaRouter() 
				{return mAppType == OmnAppType::eMediaRouter;}
	static bool	isCentralDb() {return mAppType == OmnAppType::eCentralDb;}

	virtual OmnString	getSysObjName() const {return "OmnNms";}
	virtual OmnSingletonObjId::E getSysObjId() const
			{return OmnSingletonObjId::eNms;}

	// 
	// Chen Ding, 05/07/2003
	//
	void	procStateHb(const OmnSmChgState &msg);
	static bool setAppInfo(int argc, char **argv);

private:
	OmnString	getAlgConfig(const OmnString &remoteAddr) const;

};
#endif
