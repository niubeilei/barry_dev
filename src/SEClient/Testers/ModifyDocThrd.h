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
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEClient_Testers_ModifyDocThrd_h
#define Aos_SEClient_Testers_ModifyDocThrd_h

#include "SearchEngine/Ptrs.h"
#include "SEClient/Ptrs.h"
#include "SEClient/Testers/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"



class AosModifyDocThrd : public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxMods = 10,
		eDftWeight = 100,
		eDepth = 5
	};

private:
	AosSearchEngTesterPtr	mTorturer;
	int						mTotal;
	int						mWeight;
	int						mNumModified;
	bool					mIsCreating;
	OmnThreadPtr			mThread;
	AosSEClientPtr			mConn;
	OmnIpAddr				mRaddr;
	int						mRport;
	OmnString				mDocid;
	OmnString				mSiteid;
	OmnString				mAttrTname;
	OmnString				mCrtOpr;
	OmnString				mCrtValue;

public:
	AosModifyDocThrd(
			const AosSearchEngTesterPtr &torturer,
			const int total, 
			const int weight, 
			const OmnIpAddr &remoteaddr,
			const int remoteport, 
			const OmnString &attrtname);
	~AosModifyDocThrd();

	// OmnThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state,
						const OmnThreadPtr &thread);
  	virtual bool	stop();
  	virtual bool	signal(const int threadLogicId);
   	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	isCreating() const {return mIsCreating;}

private:
	bool 	modifyDoc();
	bool 	addMetadata(const AosXmlTagPtr &doc);
	bool 	addAttr(const AosXmlTagPtr &crttag);
	bool 	deleteAttr(const AosXmlTagPtr &crttag);
	bool 	modifyAttr(const AosXmlTagPtr &crttag);
	bool 	addTextWords(const AosXmlTagPtr &crttag);
	bool 	removeTextWords(const AosXmlTagPtr &crttag);
	bool 	modifyTexts(const AosXmlTagPtr &crttag);
	bool 	addTexts(const AosXmlTagPtr &crttag);
	bool 	removeTexts(const AosXmlTagPtr &crttag);
	bool 	addTag(const AosXmlTagPtr &crttag);
	bool 	removeTag(const AosXmlTagPtr &crttag);
	AosXmlTagPtr selectSubtag(const AosXmlTagPtr &child);
};
#endif

