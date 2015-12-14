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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelRundata.h"

#include "Actions/ActUtil.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#if 0
AosValueSelRundata::AosValueSelRundata(const bool reg)
:
AosValueSel(AOSACTOPRID_RUNDATA, AosValueSelType::eRundata, reg)
{
}


AosValueSelRundata::AosValueSelRundata(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_RUNDATA, AosValueSelType::eRundata, false)
{
}


AosValueSelRundata::~AosValueSelRundata()
{
}


bool
AosValueSelRundata::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// The XML format is:
	// 	<valuesel AOSTAG_VALUE_TYPE=AOSACTOPRID_RUNDATA
	// 		AOSTAG_ARGNAME="xxx" 
	// 		AOSTAG_DEFAULT="true|false">default</valuesel>
	rdata->setArg1(AOSARG_VALUE_SELECTED, "false");
	aos_assert_r(sdoc, false);
	aos_assert_r(rdata, false);
	valueRslt.reset();
	
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorU(rdata, "valsel_attr_001") << enderr;
		return false;
	}
	
	OmnString argname = sdoc->getAttrStr(AOSTAG_ARGNAME);
	if (argname != "")
	{
		OmnString vv = rdata->getArg1(argname);
		if (vv != "")
		{
			rdata->setArg1(AOSARG_VALUE_SELECTED, "true");
			return valueRslt.setStr(vv);
		}
	}


	// Chen Ding, 05/30/2012
	if (argname == "_rdcharstr")
	{
		int fieldid = sdoc->getAttrInt(AOSTAG_FIELD_IDX, -1);
		aos_assert_r(fieldid >=0, false);
		
		int len;
		char *charstr = rdata->getCharPtr(fieldid, len);
		aos_assert_r(charstr, false);

		bool copy_memory = sdoc->getAttrBool(AOSTAG_COPY_MEMORY, true);
		if (copy_memory)
		{
			OmnString ss(charstr, len);
			valueRslt.setStr(ss);
			return true;
		}
		OmnString str(charstr, len, false);
		valueRslt.setStr(str);
		return true;
	}

	if (argname == AOSTAG_CLOUDID)
	{
		OmnString cid = rdata->getCid();
		if (cid == "")
		{
			rdata->setError() << "Missing cloudid";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
			valueRslt.setStr(cid);
		}
		valueRslt.setStr(cid);
		return true;
	}

	if (sdoc->getAttrBool(AOSTAG_DEFAULT, true))
	{
		rdata->setArg1(AOSARG_VALUE_SELECTED, "true");
		valueRslt.setStr(sdoc->getNodeText());
		return true;
	}

	return true;
}


OmnString 
AosValueSelRundata::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelRundata::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelRundata(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}






#endif
