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
// This class defines a set of ranges:
// 	<valueset type="xxx" datatype="xxx">
// 		<range start="xxx" end="xxx"/>
// 		<range start="xxx" end="xxx"/>
// 		...
// 	</valueset>
//
// Modification History:
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelMultiRanges.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#if 0

AosValueSelMulRanges::AosValueSelMulRanges(const bool reg)
:
AosValueSel(AOSACTOPRID_MULTIRANGES, AosValueSelType::eMultiRanges, reg)
{
}


AosValueSelMulRanges::AosValueSelMulRanges(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_MULTIRANGES, AosValueSelType::eMultiRanges, false)
{
	vector<AosValueRslt> start, end , step;
	if (parse(start, end, step, def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


AosValueSelMulRanges::~AosValueSelMulRanges()
{
}


bool
AosValueSelMulRanges::parse(
		vector<AosValueRslt> &start, 
		vector<AosValueRslt> &end,
		vector<AosValueRslt> &step,
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(rdata, false);
	if (!def)
	{
		rdata->setError() << "Missing valueset def";
		return false;
	}


	AosXmlTagPtr doc = def;
	if(doc->isRootTag()) doc = doc->getFirstChild();
	aos_assert_r(doc, false);
	AosXmlTagPtr child = doc->getFirstChild();
	if (!child)
	{
		rdata->setError() << "Missing ranges";
		return false;
	}

	int guard = eMaxRanges;
	while (child && guard--)
	{
		AosDataType::E type = AosDataType::toEnum(child->getAttrStr(AOSTAG_DATA_TYPE));
		if (!AosDataType::isValid(type))
		{
			rdata->setError() << "Data type incorrect: " 
				<< def->getAttrStr(AOSTAG_DATA_TYPE);
			return false;
		}
		AosValueRslt ss, ee, st;
		aos_assert_r(ss.setValue(type, child->getAttrStr(AOSTAG_START), rdata.getPtrNoLock()), false);
		aos_assert_r(ee.setValue(type, child->getAttrStr(AOSTAG_END), rdata.getPtrNoLock()), false);
		st.setValue(type, child->getAttrStr(AOSTAG_STEP,"1"), rdata.getPtrNoLock());
		start.push_back(ss);
		end.push_back(ee);
		step.push_back(st);

		child = doc->getNextChild();
	}

	rdata->setOk();
	return true;
}


bool 
AosValueSelMulRanges::run(
		AosValueRslt &value, 
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It checks whether the value 'value' is in the range defined by 
	// the smart doc 'sdoc', which is in the form:
	// 	<valueset type="xxx" start="xxx" end="xxx" datatype="xxx"/>
	vector<AosValueRslt> start, end, step;
	aos_assert_r(parse(start, end, step, sdoc, rdata), false);

	rdata->setOk();
	for (u32 i=0; i<start.size(); i++)
	{
		if(value.isRange(start[i], end[i], step[i], rdata.getPtrNoLock())) return true;
		//if (value.isGE(start[i], rdata) && value.isLE(end[i], rdata)) return true;
	}

	return false;
}


OmnString 
AosValueSelMulRanges::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelMulRanges::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelMulRanges(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}


#endif
