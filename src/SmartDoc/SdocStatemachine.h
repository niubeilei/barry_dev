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
// 2011/05/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SDocStatemachine_h
#define Aos_SmartDoc_SDocStatemachine_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "XmlUtil/XmlTag.h"
#include <vector>

class AosSdocStatemachine : public AosSmartDoc
{
private:

public:
	AosSdocStatemachine(const bool flag);
	~AosSdocStatemachine();

	virtual AosSmartDocObjPtr	clone() {return OmnNew AosSdocStatemachine(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	AosAccessRcdPtr getAccessRecord(
				        	const AosXmlTagPtr &sdoc,
						    const AosRundataPtr &rdata);
	OmnString checkEvent(
				        	const AosXmlTagPtr &sdoc,
						    AosXmlTagPtr &event_def,
							const AosRundataPtr &rdata);
	bool createErrorLog( 	const OmnString log_container,
						    const OmnString &logname,
							const OmnString &fromstate,
							const OmnString &tostate,
							const OmnString &event,
							const OmnString &errmsg,
							const u64 &docid,
							const AosXmlTagPtr &sdoc,
							const AosRundataPtr &rdata);
	bool createActionLogs(
				        	const OmnString &fromstate,
						    const OmnString &tostate,
							const OmnString &event,
							const OmnString &logcontainer, 
							const OmnString &logname,
							const u64 &docid,
							const vector<AosXmlTagPtr> &logs, 
							const AosRundataPtr &rdata);
};
#endif
