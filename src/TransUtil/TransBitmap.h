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
// 12/06/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_TransBitmap_h
#define AOS_TransUtil_TransBitmap_h

#include "aosUtil/Types.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/TransId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"

#include <set>
using namespace std;

class AosTransBitmap : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum E
	{
		eStartPoisonOffset = 0,
		eCreateTimeOffset = eStartPoisonOffset + 4,

		eBitmapSizeOffset = eCreateTimeOffset + 4,
		eBitmapOffset = eBitmapSizeOffset + 4,
		//eEndPoisonOffset = 0,
	
		eTmpPoisonNum = 0x23A0C9D1,
		eMaxLogFileSize = 4 * 1000 * 1000, 
	};

private:
	OmnMutexPtr     	mLock;
	bool				mShowLog;
	//AosBitmapObjPtr		mBitmap;
	set<AosTransId>		mFinishTids;
	//OmnString			mFnames[2];
	//OmnString			mCrtFname;
	//OmnFilePtr          mCrtFile;
	AosLogFilePtr		mFinishLog;

public:
	AosTransBitmap(const OmnString &dirname, const OmnString &fname, const bool show_log);
	~AosTransBitmap();

	void	addFinishTids(vector<AosTransId> &v_tids);
	void	removeFinishTid(const AosTransId &trans_id);
	bool	checkFinish(const AosTransId &trans_id);
	void	saveToFile();

private:
	//bool	recoverBitmap();
	bool	procFinishLogFile();
	//bool	fileIsCorrect(const OmnFilePtr &file);
	//bool	initCrtFile(const OmnString &fname);
	//bool	initBitmap(const OmnFilePtr &file);

};
#endif
