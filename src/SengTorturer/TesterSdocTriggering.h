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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_SdocTriggeringTester_h
#define AosSengTester_SdocTriggeringTester_h

#include "SengTorturer/SengTester.h"
#include "SengTorturer/XmlTagRand.h"
#include "SengTorturer/StUtil.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosSdocTriggeringTester : virtual public AosSengTester
{
public:
	enum
	{
		eMaxContainers = 10,
		eMinCtnrLen = 2,
		eMaxCtnrLen = 30,
		eMaxDepth = 4
	};

	enum Event
	{
		eEVENT_CTNR_ADDMEMBER,
	//	eEVENT_CTNR_ADDATTRMEMBER,
		eEVENT_CTNR_DELMEMBER,
	//	eEVENT_CTNR_DELATTRMEMBER,
		eEVENT_CTNR_MODMEMBER,
	//	eEVENT_CTNR_MODATTRMEMBER,
		eEVENT_CTNR_READMEMBER,
		eEVENT_MAX
	};

private:
	OmnMutexPtr			mLock;
	AosXmlTagPtr 		mCtnr;
	AosXmlTagPtr 		mOldCtnr;
	int					mType;
	OmnString 			mSobjid;
	bool 				mFlag;
	static bool				mSaveSdocFlag;
	vector<OmnString>	mSdocObjids;
	static u64				mNum;

public:
	AosSdocTriggeringTester(const bool regflag);
	AosSdocTriggeringTester();
	~AosSdocTriggeringTester();

	virtual bool test();
	AosSengTesterPtr clone()
			{
				return OmnNew AosSdocTriggeringTester();
			}
	
private:
	bool testRead(const AosRundataPtr &rdata);
	bool testModify(const AosRundataPtr &rdata);
	bool testCreate(const AosRundataPtr &rdata);
	bool testDelete(const AosRundataPtr &rdata);
	bool generateSmartdoc();
	bool ModifyCtnrEvent(const AosRundataPtr &rdata);
	bool checkResults();

	OmnString toEnum(int type)
	{
		switch (type)
		{

			case eEVENT_CTNR_ADDMEMBER: 
				 return "event_ctnr_addmember";
			case eEVENT_CTNR_DELMEMBER:
				 return "event_ctnr_delmember";
			case eEVENT_CTNR_MODMEMBER: 
				 return "event_ctnr_modmember";
			case eEVENT_CTNR_READMEMBER: 
				 return "event_ctnr_readmember";
			case 4:
				 return "event_ctnr_modattrmemeber";
			default:
				 return "";
		}

		return "";
	}
	OmnString pickDoc();
	bool	retrieveCtnrDoc(const OmnString &objid);
	OmnString pickSdocObjid();
	bool 	pickContainer();
	bool	generateSmartdoc(const OmnString &fname);
	u64		getNum();
	bool	modifyConfingDoc(const AosRundataPtr &rdata);
	bool	clear();
	bool	testModifyAttr(const AosRundataPtr &rdata);
	bool	modifysdoc_modifyattr(AosXmlTagPtr &doc);

		
};
#endif
