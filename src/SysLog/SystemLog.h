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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
//	All documents are stored in 'mDocFilename' + seqno. There is 
//	a Document Index, that is stored in files 'mDocFilename' + 
//		'Idx_' + seqno
//
// Modification History:
// 12/12/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SystemLog_SystemLog_h
#define AOS_SystemLog_SystemLog_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosSystemLog : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum EntryType
	{
		eInvalid,

		eCreateDoc,
		eDeleteDoc,
		eModifyDoc,

		eMax
	};

private:
	bool		mSysLogOn;
	AosBuffPtr	mBuff;

public:
	AosSystemLog();
	~AosSystemLog();

private:
	OmnString createFilekey(
			const OmnString &snapshot_name, 
			const u64 &dfmid)
	{
		OmnString filekey = AOSFILEKEY_SNAPSHOT_FILENAME;
		filekey << "_" << dfmid << "_" << snapshot_name;
		return filekey;
	}

};
#endif
