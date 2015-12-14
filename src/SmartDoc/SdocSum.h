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
// 2011/04/18	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocSum_h
#define Aos_SmartDoc_SdocSum_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "UtilTime/TimeUtil.h"
#include "UtilTime/TimeInfo.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

#define AOSSUMTYPE_YEAR    "year"
#define AOSSUMTYPE_MONTH   "month"
#define AOSSUMTYPE_DAILY   "daily"
#define AOSSUMTYPE_HOURLY  "hourly"

class AosSdocSum : public AosSmartDoc
{
	enum SumType
	{
		eYearly,
		eMonthly,
		eDaily,
		eHourly,

		eInvalid
	};

public:
	AosSdocSum(const bool flag);
	~AosSdocSum();

	virtual AosSmartDocObjPtr	clone() {return OmnNew AosSdocSum(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	bool	retrieveKey(
		const SumType sumtype, 
		const AosXmlTagPtr &sdoc, 
		const AosXmlTagPtr &created_doc, 
		OmnString &key,
		const AosRundataPtr &rdata);

	bool	retrieveValue(
		const AosXmlTagPtr &sdoc, 
		const AosXmlTagPtr &created_doc, 
		u64  &value, 
		const AosRundataPtr &rdata);
	
	bool	getTime(
		AosTime &timeinfo,
		const AosXmlTagPtr &sdoc, 
		const AosXmlTagPtr &created_doc, 
		const AosRundataPtr &rdata);

	SumType	getSumtype(
		const AosXmlTagPtr &sdoc);

	u64		getIILID(
		const AosRundataPtr &rata,
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &ctnr);

	bool	saveIIL(
		AosXmlTagPtr &ctnr,
		const AosXmlTagPtr &sdoc,
		u64	 iilid,
		const AosRundataPtr &rdata);

private:

};


#endif
