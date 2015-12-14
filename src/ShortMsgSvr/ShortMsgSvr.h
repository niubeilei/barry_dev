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
// 05/19/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ShortMsgSvr_ShortMsgSvr_h
#define Aos_ShortMsgSvr_ShortMsgSvr_h

#include "TransUtil/Ptrs.h"
#include "Rundata/Rundata.h"
#include "ShortMsgSvr/Ptrs.h"
#include "ShortMsgUtil/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "TransServer/TransProc.h"
#include <map>
#include <vector>

OmnDefineSingletonClass(AosShortMsgSvrSingleton,
						AosShortMsgSvr,
						AosShortMsgSvrSelf,
						OmnSingletonObjId::eShortMsgSvr,
						"AosShortMsgSvr");

class AosShortMsgSvr : public OmnRCObject 
{
	OmnDefineRCObject;

	typedef map<OmnString, AosShmHandlerPtr>     		 	HandlerMap;
	typedef map<OmnString, AosShmHandlerPtr>::iterator	 	HandlerMapItr;

private:
	enum
	{
		eMaxHandlers = 100
	};

	static HandlerMap	 	mSenderHandlerMap;
	static HandlerMap	 	mModemHandlerMap;
	bool   			       	mIsSimulate;
	AosGsmMgrPtr			mGsmMgr;

public:
	AosShortMsgSvr();
	~AosShortMsgSvr();
	
    //
    // Singleton class interface
    //
    static AosShortMsgSvr *    		getSelf();
    virtual bool      				start();
    virtual bool        			stop();
	virtual bool					config(const AosXmlTagPtr &def);
	bool							addSenderHandler(
									const OmnString &sender,
									const AosShmHandlerPtr &handler)
	{
		return addHandler(sender, handler, mSenderHandlerMap);
	}
	bool							addModemHandler(
									const OmnString &modem_cellnum,
									const AosShmHandlerPtr &handler)
	{
		return addHandler(modem_cellnum, handler, mModemHandlerMap);
	}
	static bool 					addHandler(
									const OmnString &sender, 
									const AosShmHandlerPtr &req, 
									HandlerMap &map);
	
	bool            proc(//const AosBigTransPtr &trans,
			            const AosXmlTagPtr &data,
						const AosRundataPtr &rdata);

	void 							sendResponse(
									//const AosTrans1Ptr &trans,
									const bool &rslt,
									const AosRundataPtr &rdata);
	bool 							shortMsgReceived(
									const OmnString &msg, 
									const OmnString &sender, 
									const OmnString &local_phnum,
									const AosRundataPtr &rdata);
	bool            				processHandlers(
									const OmnString &sender, 
									const OmnString &modem_cellnum, 
									const OmnString &msg, 
									AosShmHandlerPtr &handler, 
									bool &stop_flag,
									const bool remove_sender,
									const AosRundataPtr &rdata);
	AosGsmModemPtr					getGsmModem(const OmnString &key);
	bool  			   				getSimulate(){return mIsSimulate;};
	void							closeGsmMgr();
private:
	AosShmHandlerPtr	  			getHandler(
						  			const OmnString &msg,
									const AosRundataPtr &rdata);
};
#endif
