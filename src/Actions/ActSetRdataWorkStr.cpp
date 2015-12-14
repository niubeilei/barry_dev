////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 01/06/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Actions/ActSetRdataWorkStr.h"

#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/LangDictObj.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/ValueDefs.h"
#include "SEBase/SeUtil.h"
#include "Security/SessionMgr.h"
#include "Security/Ptrs.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/DynArray.h"
#include "Util/OmnNew.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include <vector>
using namespace std;

const int sgMaxAttrs = 30;


AosActSetRdataWorkStr::AosActSetRdataWorkStr(const bool flag)
:
AosSdocAction(AOSACTTYPE_SETRDATA_WORKSTR, AosActionType::eSetRdataWorkStr, flag)
{
}


AosActSetRdataWorkStr::~AosActSetRdataWorkStr()
{
}


bool
AosActSetRdataWorkStr::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		AosSetError(rdata, AOSLT_MISSING_SDOC);
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// Retrieve the value
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, rdata);
	if (!rslt)
	{
		AosSetError(rdata, "failed_ret_value001:") << AOSTERM("smart_doc", rdata)
			<< ": " << sdoc->getAttrStr(AOSTAG_OBJID);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString vv = value.getStr();
	rdata->setWorkingStr(vv);
	return true;
}


AosActionObjPtr
AosActSetRdataWorkStr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSetRdataWorkStr(false);
	}

	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

#endif
