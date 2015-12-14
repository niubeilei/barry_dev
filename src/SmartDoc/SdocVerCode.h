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
// 04/14/2011   Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocVerCode_h
#define Aos_SmartDoc_SdocVerCode_h

#if 0

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>

using namespace boost::gregorian;
using namespace boost::posix_time;

class AosSdocVerCode : public AosSmartDoc
{

	enum
	{
		eDefaultNumVerCodes = 1,
		eDefaultNumHours = 0
	};

public:
	AosSdocVerCode(const bool flag);
	~AosSdocVerCode();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocVerCode(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool	sendEmailMethod(
				const OmnString &id, 
				const AosXmlTagPtr &request, 
				const AosRundataPtr &rdata);
	bool	sendShortMethod();
};

#endif
#endif

