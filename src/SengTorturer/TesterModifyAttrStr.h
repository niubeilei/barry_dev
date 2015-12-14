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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_ModifyAttrStrTester_h
#define AosSengTester_ModifyAttrStrTester_h

#include "Rundata/Ptrs.h"
#include "SengTorturer/SengTester.h"
#include "SengTorturer/XmlTagRand.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosModifyAttrStrTester : virtual public AosSengTester
{
public:

private:
	AosXmlTagPtr 		mRawDoc;
	u64					mLocalDocid;
	u64					mServerDocid;
	AosXmlTagPtr		mServerDoc;
	OmnString			mAttrName;
	OmnString			mAttrValue;
	bool				mCanModify;
	OmnString 			mObjid;
	bool				mSpecialAttr;

	

public:
	AosModifyAttrStrTester(const bool regflag);
	AosModifyAttrStrTester();
	~AosModifyAttrStrTester();

	virtual bool test();
	AosSengTesterPtr clone()
			{
				return OmnNew AosModifyAttrStrTester();
			}
private:
	bool	pickDoc();
	bool	modifyDoc(const AosRundataPtr &rdata);
	bool	canModify();
	bool	randPickModifyAttr();
	bool	sendModfiyAttr();
	bool	checkModify();
	bool	modifyDocToMemory();
};
#endif

