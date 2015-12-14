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
#ifndef Aos_SmartDoc_SdocInlineSearch_h
#define Aos_SmartDoc_SdocInlineSearch_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "MultiLang/LangTermIds.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocInlineSearch : public AosSmartDoc
{

public:
	AosSdocInlineSearch(const bool flag);
	~AosSdocInlineSearch();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocInlineSearch(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	
	bool	docCreated(
				const AosXmlTagPtr &sdoc,
				const OmnString &ctnr,
				vector<OmnString> &keys,   
				const OmnString &seqId,
				const OmnString &splitType,
				const AosRundataPtr &rdata);
	
	bool	docModify(
				const AosXmlTagPtr &sdoc,
				const OmnString &ctnr,
				vector<OmnString> &keys,   
				const OmnString &seqId,
				const OmnString &splitType,
				const AosRundataPtr &rdata);

	bool	docDeleted(
				const AosXmlTagPtr &sdoc,
				const OmnString &ctnr,
				vector<OmnString> &keys,   
				const OmnString &seqId,
				const OmnString &splitType,
				const AosRundataPtr &rdata);
	bool 	static reBuild(
			const OmnString &sdocid, 
			const OmnString &ctnrobjid,
			const AosRundataPtr &rdata);
};
#endif

