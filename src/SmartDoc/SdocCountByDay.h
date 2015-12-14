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
// 05/27/2011	Created by Wynn
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocCountByDay_h
#define Aos_SmartDoc_SdocCountByDay_h

#include "SmartDoc/SmartDoc.h"
#include "Util/String.h"
#include <vector>
using namespace std;


class AosSdocCountByDay : public AosSmartDoc
{
public:
	AosSdocCountByDay(const bool flag);
	~AosSdocCountByDay();

	// Smartdoc Interface
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	virtual AosSmartDocObjPtr clone(){return OmnNew AosSdocCountByDay(false);}
private:
	u64 getNow();
};
#endif
