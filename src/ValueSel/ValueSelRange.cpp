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
// This class defines a range [start, end]. Both start and end are 
// 	<valueset type="xxx" start="xxx" end="xxx" datatype="xxx"/>
//
// Modification History:
// 05/10/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelRange.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#if 0


AosValueSelRange::AosValueSelRange(const bool reg)
	:
AosValueSel(AOSACTOPRID_RANGE, AosValueSelType::eRange, reg)
{
}


AosValueSelRange::AosValueSelRange(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_RANGE, AosValueSelType::eRange, false)
{
	AosValueRslt start, end, step;
	if (parse(start, end, step, def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


AosValueSelRange::~AosValueSelRange()
{
}


bool
AosValueSelRange::parse(
		AosValueRslt &start, 
		AosValueRslt &end,
		AosValueRslt &step,
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	// 'def' is the definition of this class:
	// 	<valueset type="xxx" start="xxx" end="xxx" datatype="xxx"/>
	aos_assert_r(rdata, false);
	if (!def)
	{
		rdata->setError() << "Missing valueset def";
		return false;
	}

	AosDataType::E type = AosDataType::toEnum(def->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(type))
	{
		rdata->setError() << "Data type incorrect: " 
			<< def->getAttrStr(AOSTAG_DATA_TYPE);
		return false;
	}

	//if (!start.setValue(type, def->getAttrStr(AOSTAG_START), rdata.getPtrNoLock())) return false;
	if (!start.setStr(def->getAttrStr(AOSTAG_START))) return false;
	if (!end.setValue(type, def->getAttrStr(AOSTAG_END), rdata.getPtrNoLock())) return false;
	step.setValue(type, def->getAttrStr(AOSTAG_STEP,"1"), rdata.getPtrNoLock());
	if (!start.isLE(end, rdata.getPtrNoLock()))
	{
		rdata->setError() << "Invalid range: " << start.toString() 
			<< ":" << end.toString();
		return false;
	}
	rdata->setOk();
	return true;
}


bool 
AosValueSelRange::run(
		AosValueRslt &value, 
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It checks whether the value 'value' is in the range defined by 
	// the smart doc 'sdoc', which is in the form:
	// 	<valueset type="xxx" start="xxx" end="xxx" datatype="xxx"/>
	AosValueRslt start, end, step;
	if (!parse(start, end, step, sdoc, rdata))
	{
		return false;
	}
	
//	return value.isGE(start, rdata) && value.isLE(end, rdata);
	return value.isRange(start, end, step, rdata.getPtrNoLock());
}


OmnString 
AosValueSelRange::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	mRule = rule;

	OmnString xml = "<";
	xml << tagname << " ";

	XmlGenRuleValueIter itr;
	for (itr = rule->mAttrs_r.begin(); itr != rule->mAttrs_r.end(); ++itr)
	{
		xml << itr->first <<"=\"" << (itr->second).getRandomValue() << "\" ";
	}
	for (itr = rule->mAttrs_o.begin(); itr != rule->mAttrs_o.end(); ++itr)
	{
		if (rand()%10 < 8)
			xml << itr->first <<"=\"" << (itr->second).getRandomValue() << "\" ";
	}
	
	xml << "/>";
	return  xml;
}


OmnString 
AosValueSelRange::getXmlStr(
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	return "";
}


AosValueSelObjPtr
AosValueSelRange::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelRange(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

#endif
