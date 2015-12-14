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
// 2011/06/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransClient_Tester_SqlClientTester_h
#define Aos_TransClient_Tester_SqlClientTester_h 

#include "XmlUtil/XmlTag.h"
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include <map>
#include <vector>
#include <deque>

using namespace std;


class AosImportClientTester : public OmnTestPkg
{

public:
	enum 
	{
		eNodeName = 'n',
		eNodeText = 't'
	};
	AosImportClientTester();
	~AosImportClientTester() {};

	virtual bool		start();

private:
	bool basicTest();
};
#endif
