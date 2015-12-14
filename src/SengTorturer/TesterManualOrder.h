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
// 05/25/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_TesterManualOrderTester_h
#define AosSengTester_TesterManualOrderTester_h

#include "SengTorturer/SengTester.h"
#include "Thread/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <map>
using namespace std;

class AosManualOrderTester : virtual public AosSengTester
{
	enum E
	{
		eManualTryWeight = 10,
		eAppendWeight = 30,
		eSwapWeight = 50,
		eMoveWeight = 80
	};

private:
	map<u64, u64> 		mValue;
	map<u64, u64> 		mDocid;
	OmnString			mCtrName;
	OmnString			mOrdName;

public:
	AosManualOrderTester(const bool);
	AosManualOrderTester();
	~AosManualOrderTester();

	virtual bool test();
	
	virtual AosSengTesterPtr clone()
			{
				return OmnNew AosManualOrderTester();
			}

	bool	append();
	bool	swap();
	bool    move();
	bool    del();
	void	showRslt();
	void	getNext(u64 &v, bool &flag);
};
#endif

