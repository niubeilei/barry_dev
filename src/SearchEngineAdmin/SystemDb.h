////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEARCHENGINEADMIN_SystemDb_h
#define AOS_SEARCHENGINEADMIN_SystemDb_h

#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Ptrs.h"
#include "IdGen/U64IdGen.h"
#include "SEClient/Ptrs.h"
//#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "UtilComm/Ptrs.h"
#include "Util/HashUtil.h"


class AosSystemDb
{

private:
	
	OmnMutexPtr		mLock;
	int				mRemotePort;
	u32				mTransId;
	OmnString		mSiteid;
	u64				mLastDocid;
	OmnString sgSystemContainer;
	OmnString sgImgPath;
	OmnString sgSystemImageDir;
	int sgGroupSize;
	int sgSleepLength;
	vector<OmnString> ctnrVect;
	OmnString fileStr;

public:
	AosSystemDb();
	~AosSystemDb();
    
	bool      	start();
    bool        stop();
    OmnRslt     config(const OmnXmlParserPtr &def);

	bool 		copySystemDb(const u32 siteid);

	bool		createSystemDb(
					const u64 &startDocid,
					const u64 &maxDocid,
					const OmnString &ctnrStr,
					const u32 siteid);
	
	bool		processImages(AosXmlTagPtr &doc);

	bool		isSystemContainer(const OmnString &ctnr);

};
#endif

