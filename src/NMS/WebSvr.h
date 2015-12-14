////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: WebSvr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_WebSvr_h
#define Omn_NMS_WebSvr_h

#include "NMS/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"



OmnDefineSingletonClass(OmnWebSvrSingleton,
						OmnWebSvr,
						OmnWebSvrSelf,
						OmnSingletonObjId::eWebSvr, 
						"WebSvr");

class OmnWebSvr : public virtual OmnRCObject,
				  public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eScanDbFreq = 5,
		eEpAdded = 10,
		eEpDeleted,
		eEpModified,
		eAlgAdded,
		eAlgDeleted,
		eAlgModified,
		
		eStartCapture = 31,
		eStopCapture 
	};

	OmnTcpServerPtr			mServer;
	OmnThreadPtr			mThread;
	int						mScanDbFreq;
	OmnCapProcPtr			mCapProc;

public:
	OmnWebSvr();
	~OmnWebSvr();

	static OmnWebSvr *	getSelf();
	bool		start();
	bool		stop();
	bool		config(const OmnXmlParserPtr &parser);

	bool		registerCapProc(const OmnCapProcPtr &req);
	
    virtual OmnString       getSysObjName() const {return "OmnWebSvr";}
	virtual OmnSingletonObjId::E  getSysObjId() const 
				{return OmnSingletonObjId::eWebSvr;}

	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;

private:
//	bool		procMsg(const OmnConnBuffPtr &, const OmnTcpClientPtr &);
//	bool		procAction(const OmnSoWebActionPtr &action);
public:

};

#endif
