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
// 08/19/2011	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocCopyDocs_h
#define Aos_SmartDoc_SdocCopyDocs_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocCopyDocs : public AosSmartDoc
{

	enum 
	{
		eMaxNumDocs = 500
	};

public:
	AosSdocCopyDocs(const bool flag);
	~AosSdocCopyDocs();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocCopyDocs(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	static bool copyDocs(
			const AosXmlTagPtr &precord,
			const OmnString &prefix,
			const bool is_public,
			const bool cid_required,
			const AosRundataPtr &rdata);
};
#endif

