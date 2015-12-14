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
#ifndef Aos_SEUtil_XmlTorturer_XmlWordParserTester_h
#define Aos_SEUtil_XmlTorturer_XmlWordParserTester_h

#include "SEUtil/XmlTag.h"
#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/TestPkg.h"
#include <map>
#include <vector>
#include <deque>

using namespace std;


class AosXmlWordParserTester : public OmnTestPkg
{

public:
	AosXmlWordParserTester();
	~AosXmlWordParserTester() {};

	virtual bool		start();
	AosXmlTagPtr 		createTag(vector<OmnString> wordlist, vector<OmnString> &disable_attr);

private:
	bool		basicTest();
};
#endif
