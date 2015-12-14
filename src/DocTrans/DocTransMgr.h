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
#ifndef AOS_DocTrans_DocTransMgr_h
#define AOS_DocTrans_DocTransMgr_h

#include "DocTrans/DocTrans.h"

#include <vector>
using namespace std;

class AosDocTransMgr : virtual public OmnRCObject
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
	u32							mInstId;
	OmnString					mDirName;
	OmnString					mTempFname;
	AosDocTransProcPtr			mProc;
	OmnMutexPtr					mLock;
	vector<AosDocTransPtr>		mDocVector;

public:
	AosDocTransMgr(
			const AosXmlTagPtr &config,
			const AosDocTransProcPtr &proc);
	~AosDocTransMgr();

	AosDocTransPtr 	getDocTrans();
	void			returnDocTrans(const AosDocTransPtr &doc);

private:
	OmnString		getFileName();

	void			recover();
	void			recoverEachFile(const OmnString &fileName);
	bool			recoverHeaderBody(const AosBuffPtr &buff);
};

#endif

*/
