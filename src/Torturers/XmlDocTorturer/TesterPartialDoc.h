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
#ifndef Aos_Torturers_XmlDocTurturer_PartialDocTester_h
#define Aos_Torturers_XmlDocTurturer_PartialDocTester_h

#include "SengTorUtil/SengTester.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosPartialDocTester : virtual public AosSengTester
{
public:
	enum
	{
		eMaxDepth = 4
	};


private:
	//AosRundataPtr		mRundata;
	AosXmlTagPtr		mCompleteDoc;
	AosXmlTagPtr 		mPartilaDoc;
	OmnString			mObjid;

public:
	AosPartialDocTester(const bool regflag);
	AosPartialDocTester();
	~AosPartialDocTester();

	virtual bool test();
	AosSengTesterPtr clone()
			{
				return OmnNew AosPartialDocTester();
			}
	
	bool	QueryDoc();
	bool	modifyPartialdDoc();
	bool	checkResults();
	bool	modifyCompleteDoc();
	bool	checkModifyCompleteResults();
	OmnString 	pickDoc(int loop);
};
#endif

