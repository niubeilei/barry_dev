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
// 11/17/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocCreateDocs_h
#define Aos_SmartDoc_SdocCreateDocs_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocCreateDocs : public AosSmartDoc
{
public:
	AosSdocCreateDocs(const bool flag);
	~AosSdocCreateDocs();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocCreateDocs(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	static bool	createEntries(
			const AosXmlTagPtr &entries, 
			const AosXmlTagPtr &sdoc, 
			OmnString &success,
			OmnString &failed,
			const AosRundataPtr &rdata);

	// bool getSeqno(
	// 		const AosXmlTagPtr &sdoc,
	// 		u64 &seqno,
	// 		const AosRundataPtr &rdata);
};
#endif

