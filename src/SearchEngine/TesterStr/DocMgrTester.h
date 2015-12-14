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
#ifndef Aos_SearchEngine_TesterStr_DocMgrTester_h
#define Aos_SearchEngine_TesterStr_DocMgrTester_h

#include "XmlUtil/XmlTag.h"
#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/TestPkg.h"
#include <map>

using namespace std;


class AosDocMgrTester : public OmnTestPkg
{
public:
	enum
	{
		eMaxDocs = 1000,
		eMaxObjids = 100000
	};

	enum Operation
	{
		eGetDocByDocid,
		eGetDocByObjid
	};

private:
	struct docLink*					mDocHead;
	std::map<u64, struct docLink*>	mDocMap;
	AosXmlTagPtr					mDocs[eMaxDocs];
	int								mNumDocs;
	OmnString						mObjids[eMaxObjids];
	int								mNumObjids;
	int 							mErrors;

public:
	AosDocMgrTester();
	~AosDocMgrTester() {};

	virtual bool		start();

private:
	bool		basicTest();
	bool		verifyLastDoc();
	bool		verifyAndAddDoc(u64 docid, const AosXmlTagPtr &doc, OmnString odjid);
	bool		getDocByDocId();
	bool		getDocByObjId();
	bool		addNode(struct docLink* node, AosXmlTagPtr doc, u64 docid, bool flag);
	OmnString	pickObjid()
				{
					if(mNumObjids == 0)
						return "vpd_about";
					else
					{
						int i = rand() % mNumObjids;
						return mObjids[i];
					}
				};
	Operation	pickOperation()
				{
					if(rand()%2 == 0)
						return eGetDocByDocid;
					else
                       	return eGetDocByObjid;
				};
};
#endif
