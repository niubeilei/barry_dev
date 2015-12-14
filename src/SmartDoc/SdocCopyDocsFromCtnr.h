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
// 07/22/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocCopyDocsFromCtnr_h
#define Aos_SmartDoc_SdocCopyDocsFromCtnr_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocCopyDocsFromCtnr : public AosSmartDoc
{

	enum 
	{
		eMaxNumDocs = 500
	};

public:
	AosSdocCopyDocsFromCtnr(const bool flag);
	~AosSdocCopyDocsFromCtnr();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocCopyDocsFromCtnr(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	static bool copyDocs(
					const OmnString &orig_objid, 
					const OmnString &target_objid, 
					const bool is_public,
					const bool cid_required,
					const AosRundataPtr &rdata);
};
#endif

