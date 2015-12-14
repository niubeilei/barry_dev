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
#ifndef Aos_SmartDoc_SdocCondActions_h
#define Aos_SmartDoc_SdocCondActions_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "Util/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

#define AOSSUMTYPE_YEAR    "year"
#define AOSSUMTYPE_MONTH   "month"
#define AOSSUMTYPE_DAILY   "daily"
#define AOSSUMTYPE_HOURLY  "hourly"

class AosSdocCondActions : public AosSmartDoc
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
	AosSdocCondActions(const bool flag);
	~AosSdocCondActions();

	virtual AosSmartDocObjPtr	clone() {return OmnNew AosSdocCondActions(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};


#endif
