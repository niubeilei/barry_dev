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
// 10/22/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogUtil_LogId_h
#define AOS_SeLogUtil_LogId_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeErrors.h"
#include "Util/String.h"
#include "Debug/Debug.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlUtil.h"



class AosLogId
{
public:
	enum
	{
		eSeqnoBitmap = 0x3fffff,
		eOffsetBitmap = 0xffffff,
		eModuleIdBitmap = 0x3ff,
		eLogFileReservedSize = 10000
	};

private:
	AosLogId();
	~AosLogId();

public:

	static inline u64 composeLogid(
			const u32 seqno, 
			const u32 offset, 
			const u32 module_id)
	{
		// Logid is constructed as:
		//    Lower 10 bits:  ModuleId
		//    Next  24 bites: Offset
		//    Next  22 bites: Seqno
		//    Next  8  bits:  Reserve
		// u64 logid = (((u64)seqno&eSeqnoBitmap) << 34) +
		//            (((u64)offset&eOffsetBitmap) << 10) +
		//            (module_id&eModuleIdBitmap);
		u64 logid = (((u64)seqno) << 34) +
		            (((u64)offset) << 10) +
		            module_id;
		AosDocType::setDocidType(AosDocType::eLogDoc, logid);
		return logid;
	}

	static inline void decomposeLogid(
			const u64 &logid,
			u32 &seqno,
			u32 &offset,
			u32 &moduleid)
	{
		moduleid = logid & eModuleIdBitmap;
		offset = (logid >> 10) & eOffsetBitmap;
		seqno  = (logid >> 34) & eSeqnoBitmap;
	}

	static inline u64 getMaxLogFileSize() {return eOffsetBitmap - eLogFileReservedSize;}
};
#endif
