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
// Modification History:
// 2014/01/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IndexMgr_ParalIIL_h
#define Aos_IndexMgr_ParalIIL_h

#include "IndexMgr/ParalLevel.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosParalIIL : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eSecondsInOneDay = 60*60*24,
		eDftWaitSec = 60*60*4 	// four hours
	};

	OmnMutexPtr				mLock;
	OmnString				mName;
	OmnString				mIILName;
	OmnString				mTimeFieldname;
	vector<AosParalLevel>	mLevels;
	int						mWaitSec;

public:
	AosParalIIL();
	~AosParalIIL();

	bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &defs);

	virtual bool getIILName(
						const AosRundataPtr &rdata, 
						const OmnString &base_name,
						const int epoch_day, 
						OmnString &iilname, 
						int &level,
						int &period,
						bool &need_create);

	virtual bool getIILNames(
						const AosRundataPtr &rdata, 
						const AosQueryReqObjPtr &query_req, 
						vector<OmnString> &iilnames);

	virtual bool removePeriod(const AosRundataPtr &rdata, 
						const int level,
						const int period);
};
#endif

