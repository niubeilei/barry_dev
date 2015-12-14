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
// 	<valueset type="xxx" datatype="xxx">xxx,xxx,...</valueset>
// or:
// 	<valueset type="xxx" datatype="xxx">
// 		<value>xxx</value>
// 		<value>xxx</value>
// 		...
// 	</valueset>
//
// Modification History:
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelEnum.h"

#include "ValueSel/ValueSelAttr.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEUtil/ValueDefs.h"
#include "Util/OmnNew.h"
#if 0

AosValueSelEnum::AosValueSelEnum(const bool reg)
:
AosValueSel(AOSACTOPRID_ENUM, AosValueSelType::eEnum, reg)
{
}


AosValueSelEnum::AosValueSelEnum(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_ENUM, AosValueSelType::eEnum, false)
{
	// vector<AosValueRslt> vv;
	// if (!parse(vv, def, rdata))
	// {
	// 	OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
	// 	throw e;
	// }
}


AosValueSelEnum::~AosValueSelEnum()
{
}


bool
AosValueSelEnum::parse(
		vector<AosValueRslt> &values, 
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	values.clear();
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

	OmnString enumtype = def->getAttrStr(AOSTAG_ENUMTYPE);
	if (enumtype == AOSVALUE_ENUM_TYPE1)
	{
		// The values are listed in the tag body, separated by a 
		// separator, which is defaulted to ",".
		OmnString vv = def->getNodeText();
		if (vv == "") return true;

		OmnString sep = def->getAttrStr(AOSTAG_SEPARATOR, ",");
		if (sep == "") sep = ",";
		OmnStrParser1 parser(vv, sep);
		parser.reset();
		while (parser.hasMore())
		{
			OmnString word = parser.nextWord();
			AosValueRslt vv;
			vv.setStr(word);
			values.push_back(vv);
		}
		rdata->setOk();
		return true;
	}

	if (enumtype == AOSVALUE_ENUM_TYPE2)
	{
		AosXmlTagPtr doc = def;
		if(doc->isRootTag()) doc = doc->getFirstChild();
		AosXmlTagPtr child = doc->getFirstChild();
		if (!child)
		{
			rdata->setError() << "Missing enum values";
			return false;
		}

		AosValueRslt vv;
		while (child)
		{
			//aos_assert_r(vv.setValue(type, child->getNodeText(), rdata), false);
			aos_assert_r(AosValueSel::getValueStatic(vv, child, rdata),false);
			values.push_back(vv);
			child = doc->getNextChild();
		}
		rdata->setOk();
		return true;
	}

	rdata->setError() << "Invalid enum type: " << enumtype;
	return false;
}


bool 
AosValueSelEnum::run(
		AosValueRslt &value, 
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It checks whether the value 'value' is in the range defined by 
	// the smart doc 'sdoc', which is in the form:
	vector<AosValueRslt> values; 
	aos_assert_r(parse(values, sdoc, rdata), false);

	rdata->setOk();
	for (u32 i=0; i<values.size(); i++)
	{
		if (value.isEQ(values[i], rdata.getPtrNoLock())) return true;
	}
	return false;
}


OmnString 
AosValueSelEnum::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelEnum::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelEnum(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}
#endif
