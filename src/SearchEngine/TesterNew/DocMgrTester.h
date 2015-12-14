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
// 2010/10/23	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SearchEngine_TesterNew_DocMgrTester_h
#define Aos_SearchEngine_TesterNew_DocMgrTester_h

#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/TestPkg.h"
#include "XmlUtil/XmlTag.h"
#include <map>

using namespace std;


class AosDocMgrTester : public OmnTestPkg
{
	/*
public:
	enum
	{
		eMaxDocs = 1000,
		eMaxObjids = 100000
	};

	enum Operation
	{
		eGetDocByDocid,
		eGetDocByObjid,

		eMaxOperations
	};

private:
	AosXmlTagPtr		mDocs[eMaxDocs+1];
	u64					mDocids[eMaxDocs+1];
	u64					mNumDocs;
	int 				mErrors;
	vector<OmnString>	mObjids;

public:
	AosDocMgrTester();
	~AosDocMgrTester() {};

	virtual bool		start();

private:
	bool		basicTest();
	bool		verifyDocs();
	bool		verifyAndAddDoc(
					const u64 &docid, 
					const AosXmlTagPtr &doc, 
					const OmnString &odjid);
	bool		getDocByDocId();
	bool		getDocByObjId();
	bool		addNode(struct docLink* node, AosXmlTagPtr doc, u64 docid, bool flag);
	OmnString	pickObjid()
				{
					if(mObjids.size() == 0)
						return "vpd_about";
					else
					{
						int i = rand() % mObjids.size();
						return mObjids[i];
					}
				};

	Operation	pickOperation()
				{
					return (rand() % eMaxOperations);
				};
				*/
};
#endif
