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
// 2010/10/31	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Torturers_XmlDocTurturer_SaveObjTester_h
#define Aos_Torturers_XmlDocTurturer_SaveObjTester_h

#include "Torturers/XmlDocTorturer/SengTester.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosSaveObjTester : virtual public AosSengTester
{
private:
	// Weights

	// Working Data

public:
	AosSaveObjTester(const bool regflag);
	~AosSaveObjTester();

	virtual bool test();

private:
};
#endif

