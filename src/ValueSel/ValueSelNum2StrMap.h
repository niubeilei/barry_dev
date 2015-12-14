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
// 11/18/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueSel_ValueSelNum2StrMap_h
#define Aos_ValueSel_ValueSelNum2StrMap_h

#include "ValueSel/ValueSel.h"

class AosValueSelNum2StrMap : public AosValueSel
{
private:
	enum
	{
		eMaxValues = 50,
		eMaxListedValues = 500
	};

public:
	AosValueSelNum2StrMap(const bool reg);
	AosValueSelNum2StrMap(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosValueSelNum2StrMap();

	virtual bool run(
					AosValueRslt    &valueRslt,
					const AosXmlTagPtr &item,
					const AosRundataPtr &rdata);
	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
					
	virtual OmnString getXmlStr(
					const OmnString &tagname, 
					const int level,
					const AosRandomRulePtr &rule, 
					const AosRundataPtr &rdata);
	static OmnString getXmlStr(
					const OmnString &tagname, 
					const int level,
					const OmnString &valuesel,
					const OmnString &values,
					const AosRundataPtr &rdata);
private:
	bool getValueByValueSelectors(
					const OmnString &value_to_map,
					const AosXmlTagPtr &maptag,
					const AosDataType::E datatype,
					AosValueRslt &valueRslt,
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
	bool getValueByValueListing(
					const OmnString &value_to_map,
					const AosXmlTagPtr &maptag,
					const AosDataType::E datatype,
					AosValueRslt &valueRslt,
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);

	bool getValueByRanges(
					const OmnString &value_to_map,
					const AosXmlTagPtr &maptag,
					const AosDataType::E datatype,
					AosValueRslt &valueRslt,
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
};
#endif

