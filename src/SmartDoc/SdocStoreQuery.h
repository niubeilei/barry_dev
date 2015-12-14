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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocStoreQuery_h
#define Aos_SmartDoc_SdocStoreQuery_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"

class AosSdocStoreQuery : public AosSmartDoc
{

public:
	AosSdocStoreQuery(const bool flag);
	~AosSdocStoreQuery();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocStoreQuery(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool addQuery(const AosXmlTagPtr &query, const AosRundataPtr &rdata);

};

#endif

