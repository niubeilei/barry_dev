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
// 11/03/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartReply_SmartReplyMgr_h
#define Aos_SmartReply_SmartReplyMgr_h	

#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <vector>
using namespace std;

OmnDefineSingletonClass(AosSmartReplyMgrSingleton,
		 				AosSmartReplyMgr,
		  				AosSmartReplyMgrSelf,
						OmnSingletonObjId::eSmartReplyMgr,
			 		    "SmartReplyMgr");


class AosSmartReplyMgr: public OmnRCObject 
{
	OmnDefineRCObject;	
	enum
	{
		eFileSize = 1000000,

		eMaxPatternSize = 100,
		eMaxAnswerSize = 300,
		
		ePatternLenOffset = 0,
		ePatternOffset = ePatternLenOffset + 4,
		eAnswerLenOffset = ePatternOffset + eMaxPatternSize,
		eAnswerOffset = eAnswerLenOffset + 4,
		
		eEntrySize = eAnswerOffset + eMaxAnswerSize
	};

private:
	u32					mCrtPatternNum;
	OmnFilePtr  		mPatternFile;
	vector<OmnString>	mPatterns;
	vector<OmnString>	mAnswers;
		
public:
	AosSmartReplyMgr();
	~AosSmartReplyMgr();

	// Singleton class interface
	static AosSmartReplyMgr *     getSelf();
	virtual bool        start(){return true;};
	virtual bool        stop(){return true;};
	virtual bool        config(const AosXmlTagPtr &def){return true; };

	void		addPatterns(const OmnString &pattern, const OmnString &answer);
	OmnString	getReply(const OmnString &question);
};
#endif

