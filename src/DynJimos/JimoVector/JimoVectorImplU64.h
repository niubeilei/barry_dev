////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/05/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_JimoVector_JimoVectorImplU64_h
#define Aos_Jimo_JimoVector_JimoVectorImplU64_h

#include "Jimo/JimoVector/JimoVectorImpl.h"
#include <vector>
using namespace std;

class AosJimoVectorImplU64 : public AosJimoVectorImpl
{
private:
	vector<u64>		mVector;

public:
	AosJimoVectorImplU64();
	~AosJimoVectorImplU64();

	virtual bool run(const AosRundataPtr &rdata, 
					const AosMethod::E method,
					const AosXmlTagPtr &worker_doc);
	virtual bool supportInterface(
						const AosRundataPtr &rdata, 
						const int interface_id) const;
};
#endif
