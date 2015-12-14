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
#include "ValueSel/ValueSelFull.h"

#include "ValueSel/ValueRslt.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEUtil/ValueDefs.h"
#include "Util/OmnNew.h"


AosValueSelFull::AosValueSelFull(const bool reg)
	:
AosValueSel(AOSACTOPRID_FULL, AosValueSelType::eFull, reg)
{
}


AosValueSelFull::AosValueSelFull(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_FULL, AosValueSelType::eFull, false)
{
	vector<AosValueRslt> vv;
	if (parse(vv, def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


AosValueSelFull::~AosValueSelFull()
{
}


bool
AosValueSelFull::parse(
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

/*	AosValueRslt::Type type = AosValueRslt::toFull(def->getAttrStr(AOSTAG_DATATYPE));
	if (!AosValueRslt::isValid(type))
	{
		rdata->setError() << "Data type incorrect: " 
			<< def->getAttrStr(AOSTAG_DATATYPE);
		return false;
	}

	OmnString enumtype = def->getAttrStr(AOSTAG_ENUMTYPE);
	if (enumtype == AOSVALUE_ENUM_TYPE1)
	{
		OmnString vv = def->getNodeText();
		if (vv == "")
		{
			rdata->setError() << "Missing values!";
			return false;
		}

		OmnStrParser parser(vv, ", ");
		parser.reset();
		while (parser.hasMore())
		{
			OmnString word = parser.nextWord();
			AosValueRslt vv;
			aos_assert_r(vv.setValue(type, word, rdata), false);
			values.push_back(vv);
		}
		rdata->setOk();
		return true;
	}

	if (enumtype == AOSVALUE_ENUM_TYPE2)
	{
		AosXmlTagPtr child = def->getFirstChild();
		if (!child)
		{
			rdata->setError() << "Missing enum values";
			return false;
		}

		AosValueRslt vv;
		while (child)
		{
			aos_assert_r(vv.setValue(type, child->getNodeText(), rdata), false);
			values.push_back(vv);
			child = def->getNextChild();
		}
		rdata->setOk();
		return true;
	}
*/
//	rdata->setError() << "Invalid enum type: " << enumtype;
	return false;
}


bool 
AosValueSelFull::run(
		AosValueRslt &value, 
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It checks whether the value 'value' is in the range defined by 
	// the smart doc 'sdoc', which is in the form:
	// 	<valueset type="xxx" zky_set_allnone ="all/none" datatype="xxx"/>

	if(!sdoc)
	{
		rdata->setError() << "Missing sdoc";
		return false;
	}

	OmnString type = sdoc->getAttrStr(AOSTAG_SET_ALLNONE);
	if(type == "none")
	{
		if(value.isNull())
		{
			rdata->setOk();
			return true;
		}
	}
	else if(type == "all")
	{
		if(!value.isNull())
		{
			rdata->setOk();
			return true;
		}
	}

	rdata->setOk();
	return false;
}


OmnString 
AosValueSelFull::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
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


AosValueSelObjPtr
AosValueSelFull::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelFull(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

