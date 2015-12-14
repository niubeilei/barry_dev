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
// 07/21/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/29
/*
#ifndef AOS_DocTrans_DocTrans_h
#define AOS_DocTrans_DocTrans_h

#include "aosUtil/Types.h"
#include "DocTrans/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "TransBasic/Trans.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

#include <vector>
using namespace std;

class AosDocTrans : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		//temp metadata offset
		eTmpStatus = 4,
		eTmpFileSize = 5,
		eTmpEntryId = 9,
		eTmpBlockNum = 17,
		eTmpNumTrans = 21,
		eTmpTrans = 25,
		eTmpPoisonNum = 0x23A0C9D1,

		eMaxFileSize = 50000000 
	};

private:
	AosDocTransProcPtr		mProc;
	OmnString				mFilename;
	OmnFilePtr				mFile;
	OmnMutexPtr				mLock;
	u32						mFileSize;
	int						mBlockNum;

public:
	AosDocTrans(
			const OmnString &fname,
			const AosDocTransProcPtr &proc);
	~AosDocTrans();

	bool			isGood();

	OmnString		getFilename(){return mFilename;}

	bool			initTmpFile(
						const u64 &entryId,
						const vector<u64> &trans);
	bool			addBlock(
						AosBuff &headerBuff, 
						AosBuffPtr &bodyBuff);
	bool			initTmpFileFinish();
	bool			finishTrans(
						vector<u64> &trans,
						const AosRundataPtr &rdata);

};

#endif

*/
