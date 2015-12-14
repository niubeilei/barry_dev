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
// 04/26/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocActionsOnDocs_h
#define Aos_SmartDoc_SdocActionsOnDocs_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocActionsOnDocs : public AosSmartDoc
{

public:
	AosSdocActionsOnDocs(const bool flag);
	~AosSdocActionsOnDocs();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocActionsOnDocs(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	bool doActions(
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &actions, 
		const AosXmlTagPtr &entries,
		const AosRundataPtr &rdata);
};
#endif

