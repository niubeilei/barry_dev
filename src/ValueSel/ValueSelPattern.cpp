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
// 05/18/2011	Created by Tom
///////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelPattern.h"

#include "ValueSel/ValueRslt.h"
#include "ValueSel/ValueSelAttr.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEUtil/ValueDefs.h"
#include "Util/OmnNew.h"
#include <boost/regex.hpp>
#if 0

AosValueSelPattern::AosValueSelPattern(const bool reg)
	:
AosValueSel(AOSACTOPRID_PATTERN, AosValueSelType::ePattern, reg)
{
}


AosValueSelPattern::AosValueSelPattern(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_PATTERN, AosValueSelType::ePattern, false)
{
	OmnString vv;
	if (!parse(vv, def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


AosValueSelPattern::~AosValueSelPattern()
{
}


bool
AosValueSelPattern::parse(
		OmnString &pattern, 
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(rdata, false);
	if (!def)
	{
		rdata->setError() << "Missing valueset def";
		return false;
	}
/*
	AosValueRslt::Type type = AosValueRslt::toEnum(def->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosValueRslt::isValid(type))
	{
		rdata->setError() << "Data type incorrect: " 
			<< def->getAttrStr(AOSTAG_DATA_TYPE);
		return false;
	}
*/
	pattern = def->getNodeText();
	if (pattern == "")
	{
	//	rdata->setError() << "Missing values!";
		return false;
	}

	rdata->setOk();
	return true;
}


bool 
AosValueSelPattern::run(
		AosValueRslt &value, 
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It checks whether the value 'value' is in the range defined by 
	// the smart doc 'sdoc', which is in the form:
	// 	<valueset type="xxx" start="xxx" end="xxx" datatype="xxx"/>
	OmnString pattern; 
	aos_assert_r(parse(pattern, sdoc, rdata), false);

	rdata->setOk();
	
	bool rslt;
	OmnString str = value.getValueStr("", rslt);
	if (!rslt)
	{
		AosSetErrorUser(rdata, "valuesel_pattern_001") << value.getType() << enderr;
	}
	else
	{
		try
		{
			std::string temp = pattern.data();
	    	boost::regex reg(temp);
	    	std::string s = str.data();
	    	if (boost::regex_match(s, reg))
			{
				rdata->setOk();
	    		return true;
			}
		}
		catch(const boost::bad_expression& e)
		{
			rdata->setError()<<" RegExp  error";
		}
	}

	return false;
}


OmnString 
AosValueSelPattern::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelPattern::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelPattern(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}






#endif
