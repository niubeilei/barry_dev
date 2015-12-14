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
// 2015/03/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoSample_h
#define Aos_JimoAPI_JimoSample_h

#include "JimoCall/Ptrs.h"
#include "Util/String.h"

using namespace std;

class AosRundata;

namespace Jimo
{
class JimoSample
{
public:
	enum FuncName
	{
		eInvalidFuncName,

		eHelloJimo,
		eHelloJimoWithMessage,

		eMaxFuncName
	};

	bool	helloJimo(AosRundata *rdata);
	bool	helloJimo( 	AosRundata *rdata,
						const OmnString &message);
};
};
#endif
