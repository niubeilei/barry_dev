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
// 04/22/2011   Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocWriteDoc_h
#define Aos_SmartDoc_SdocWriteDoc_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"
#include <vector>

class AosSdocWriteDoc : public AosSmartDoc
{
	enum
	{
		eDefaultNumWriteDocs = 1,
		eDefaultNumHours = 0
	};

private:

public:
	AosSdocWriteDoc(const bool flag);
	~AosSdocWriteDoc();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocWriteDoc(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool	createDoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool	callActions(const AosXmlTagPtr &action, const AosRundataPtr &rdata);
};

#endif

