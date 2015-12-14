////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 09/20/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataScanner_Testers_NormalDoc_h 
#define Aos_DataScanner_Testers_NormalDoc_h 

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "QueryRslt/Ptrs.h"
#include "BitmapMgr/Bitmap.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Opr.h"

#include <vector>

class AosNormalDoc : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:

	u64 			mStartDocid;
	u64 			mCrtDocid;

public:
	AosNormalDoc();

	~AosNormalDoc();

	bool createData(const AosRundataPtr &rdata);
	u64 getStartDocid(){return mStartDocid;}
};
#endif

