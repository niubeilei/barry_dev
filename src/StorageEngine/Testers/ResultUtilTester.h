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
// 09/26/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_Testers_ResultUtilTester_h
#define Aos_StorageEngine_Testers_ResultUtilTester_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "QueryRslt/Ptrs.h"
#include "QueryRslt/QueryBitmap.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Opr.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "DocClient/DocidShufflerMgr.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"


class AosResultUtilTester  
{
public:
	static bool sendStart(
			const OmnString &scanner_id, 
			const AosXmlTagPtr &fnames,
			const AosRundataPtr &rdata)
	{
		bool rslt = AosDocidShufflerMgr::getSelf()->sendStart(scanner_id,  1000000, fnames, rdata);
		return rslt;
	}


	static  bool sendData(
			const OmnString &scanner_id,
			const vector<AosQueryBitmapObjPtr> &bitmaps,
			const AosDocClientCallerPtr &caller,
			const AosRundataPtr &rdata)
	{
		int num_thrds = 1;
OmnScreen << "=================== scannerid: " << scanner_id << " , size: "<< bitmaps.size() << endl; 
		for (u32 i = 0; i < bitmaps.size(); i++)
		{
			bool rslt = AosDocidShufflerMgr::getSelf()->shuffle(scanner_id, bitmaps[i], num_thrds, caller, rdata);
			aos_assert_r(rslt, false);
		}
		return true;
	}


	static bool	sendFinished(
			const OmnString &scanner_id, 
			const AosRundataPtr &rdata)
	{
		bool rslt = AosDocidShufflerMgr::getSelf()->sendFinished(scanner_id, rdata);
		return rslt;
	}

};
#endif
