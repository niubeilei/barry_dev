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
// 08/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "ValueSel/ValueSelector.h"

#include "ValueSel/ValueSel.h"


AosValueSelector::AosValueSelector()
{
}


AosValueSelector::AosValueSelector(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	parse(def, rdata);
}


AosValueSelector::~AosValueSelector()
{
}


bool
AosValueSelector::parse(const AosXmlTagPtr &data, const AosRundataPtr &rdata)
{
	// 'def' is an XML of the following form:
	// 	<data_selector>
	// 		<entry .../>
	// 		<entry .../>
	// 		...
	// 		<entry .../>
	// 	</data_selector>
	// This function parses the XML doc and sets the values into an array. 
	aos_assert_rr(data, rdata, false);
	AosXmlTagPtr entry = data->getFirstChild();
	mSelectors.clear();
	while (entry)
	{
		AosValueSelPtr selector = AosValueSel::getValueSelector(entry, rdata);
		if (!selector)
		{
			OmnAlarm << "Failed parsing the selector: " << entry->toString()
				<< ". Error: " << rdata->getErrmsg() << enderr;
		}
		else
		{
			mSelectors.push_back(selector);
		}
		entry = data->getNextChild();
	}
	return true;
}


/*
OmnString 
AosValueSelector::getValue(const AosXmlTagPtr &obj, const AosRundataPtr &rdata)
{
	// This function concatenate values from mSelectors.
	AosValueRslt value;
	for (u32 i=0; i<mSelectors.size(); i++)
	{
		mSelectors[i]->getValue(value, obj, rdata);
	}

	return value.getStrValue(rdata);
}
*/


bool 
AosValueSelector::setSelectByConstant(const OmnString &value)
{
	// mSelectors.clear();
	// mSelectors.push_back(AosValueSel::getConstant(value));
	return true;
}


bool 
AosValueSelector::setSelectByAttr(const OmnString &aname)
{
	// mSelectors.clear();
	// mSelectors.push_back(AosValueSel::getAttrSelector(aname));
	return true;
}


bool
AosValueSelector::setConfig(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	return parse(config, rdata);
}
#endif
