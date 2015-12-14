////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 12/15/2011	Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogSvr_PhyLogSvr_h
#define AOS_SeLogSvr_PhyLogSvr_h

#include "LogTrans/Ptrs.h"
#include "SeLogSvr/Ptrs.h"
#include "LogTrans/LogTrans.h"
#include "SEInterfaces/PhyLogSvrObj.h"
#include "SeLogUtil/LogOpr.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThrdShellProc.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransProc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlUtil.h"
#include "Util/RCObject.h"


OmnDefineSingletonClass(AosPhyLogSvrSingleton,
						AosPhyLogSvr,
						AosPhyLogSvrSelf,
						OmnSingletonObjId::ePhyLogSvr,
						"AosPhyLogSvr");

class AosPhyLogSvr : virtual public AosTransProc,
					 virtual public AosPhyLogSvrObj	
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxLogPerContainer = 10
	};


private:
	static bool		mIsChecking;

	
	class procThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		AosLogTransPtr	mTrans;
		
	public:
		procThrd(const AosLogTransPtr &trans)
		:
		OmnThrdShellProc("LogprocThrd"),
		mTrans(trans)
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(); 
	};
	

public:
	//AosPhyLogSvr(const AosXmlTagPtr &config);
	AosPhyLogSvr();
	~AosPhyLogSvr();

	// Singleton add by Ketty 2013/03/11
	// Singleton class interface
	static AosPhyLogSvr*   getSelf();
	virtual bool        start();
	virtual bool        stop() {return true;};
	virtual bool        config(const AosXmlTagPtr &def);

	// TransProc interface
	//virtual bool proc(const AosIILTransPtr &trans, const AosRundataPtr &rdata){ return false; };
	//virtual bool proc(const AosBigTransPtr &trans, 
	//				const AosBuffPtr &content,
	//				const AosRundataPtr &rdata);

	static  int  			getMaxLogsPerContainer() 
							{
								return eMaxLogPerContainer;
							}
	static AosMdlLogSvrPtr	getMdlLogSvr(const int idx);
	static bool				isChecking(){return mIsChecking;}

private:
	bool					init(const AosXmlTagPtr &config);

	//bool					retrieveLog(
	//						const AosXmlTagPtr &trans_doc,
	//						const AosRundataPtr &rdata);
	
	//bool					retrieveLogs(
	//						const AosXmlTagPtr &trans_doc,
	//						const AosRundataPtr &rdata);
	 
	//void					sendResp(
	//						//const AosBigTransPtr &trans, 
	//						const AosRundataPtr &rdata);
	
public:
	// Ketty 2013/03/20
	bool		addLog(
					const AosLogOpr::E opr,
					const u64 ctnrdocid,
					const OmnString &pctr_objid,
					const OmnString logname,
					const AosXmlTagPtr &loginfo, 
					u64 &logid,
					const AosRundataPtr &rdata);
	
	OmnString 	retrieveLogs(
					const AosXmlTagPtr &req,
					const AosRundataPtr &rdata);

	AosXmlTagPtr	retrieveLog(
					const u64 &logid,
					const AosRundataPtr &rdata);

	bool		addTrans(const AosLogTransPtr &trans);

};
#endif
