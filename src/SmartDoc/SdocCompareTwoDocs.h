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
// 01/05/2014	Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocCompareTwoDocs_h
#define Aos_SmartDoc_SdocCompareTwoDocs_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "MultiLang/LangTermIds.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocCompareTwoDocs : public AosSmartDoc
{

public:
	AosSdocCompareTwoDocs(const bool flag);
	~AosSdocCompareTwoDocs();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocCompareTwoDocs(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

};
#endif

