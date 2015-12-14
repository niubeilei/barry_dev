////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TransMgr.h
// Description:
//	It maintains a list of all transactions for an SPNR.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TransMgr_TransMgr_h
#define Omn_TransMgr_TransMgr_h

#include "aosUtil/Types.h"
#include "Debug/Rslt.h"
#include "Message/Ptrs.h"
#include "NMS/HouseKpObj.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "TransMgr/Ptrs.h"
#include "TransMgr/ForeignTransId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashSPtr.h"
#include "Util/HashSPtrB.h"
#include "UtilComm/Ptrs.h"
#include "Util/ValList.h"
#include "Util/HashAddrPort.h"
#include "Util1/TimerObj.h"
#include "XmlParser/Ptrs.h"


OmnDefineSingletonClass(OmnTransMgrSingleton,
						OmnTransMgr,
						OmnTransMgrSelf,
                		OmnSingletonObjId::eTransMgr, 
						"TransMgr");


class OmnTransMgr : public OmnHouseKpObj
{	
private:
	enum
	{
		eTableSize = 8192,
		eTransCleaningTimer = 15,
		
		eTransCheckTimer = 30,		//trans check timer
		eMinTimerSec = 1,			//min timer in second
	};

	OmnHashSPtr<OmnTransPtr, eTableSize>	mTransTable;
	OmnHashSPtrB<OmnTransPtr, OmnForeignTransId, eTableSize>	mForeignTrans;

	OmnMutexPtr		mLock;
	unsigned int	mCleanThreshold;	// It tells which (old) trans need to be
										// cleaned in finished table.
										
public:
	OmnTransMgr();
	virtual ~OmnTransMgr();

	// 
	// Housekp Interface
	//
	virtual void		procHouseKeeping(const int64_t &tick);
	virtual OmnString	getName() const {return "TransMgr";}

	//
	// member functions
	//
	static OmnTransMgr *	getSelf();
	bool					start();
	bool					stop();
	bool					config(const AosXmlTagPtr &configItem);

	OmnRslt					cleanTrans();

	//
	// sets
	// 
	void	setCleanTimeThreshold(const long t) { mCleanThreshold = t; }

	//
	// gets
	//
	long	getCleanTimeThreshold() const { return mCleanThreshold; }

	bool				addTrans(const OmnTransPtr &trans);
	OmnTransPtr			getTrans(const OmnRespPtr &resp, const bool removeFlag);
	OmnTransPtr			getTrans(const int64_t &transId, const bool removeFlag);
	OmnTransPtr			getTrans(const OmnTransPtr &trans);
	OmnTransPtr			getTrans(const OmnIpAddr &addr, 
								 const int port,
								 const int64_t &transId, 
								 const bool removeFlag);

private:
};
#endif

