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
// 2009/10/16	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SearchEngine_Testers_XmlParserTester_h
#define Aos_SearchEngine_Testers_XmlParserTester_h

#include "Tester/TestPkg.h"


class AosXmlParserTester : public OmnTestPkg
{
private:
	enum
	{
		eMaxAttrs = 1000
	};

	struct TagInfo
	{
		OmnString	attrnames[eMaxAttrs];
		OmnString	attrvalues[eMaxAttrs];
	};

public:
	AosXmlParserTester() {mName = "Tester";}
	~AosXmlParserTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};


#endif

