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
//
// Modification History:
// 01/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/StUserOprArd.h"

#include "Random/RandomUtil.h"
#include "Random/CommonValues.h"
#include "SengTorUtil/StAccess.h"
#include "SEUtil/ValueDefs.h"
#include "XmlUtil/SeXmlParser.h"


AosXmlTagPtr 
AosStUserOprArd::createUserOprArd(
		const OmnString &objid,
		const AosSengTestThrdPtr &thread)
{
	// 	<opr_ard ...>
	// 		<AOSTAG_OPERATIONS>
	// 			<oprcode AOSTAG_ACCESS=AOSVALUE_GRANT|AOSVALUE_DENY>
	// 			<oprcode AOSTAG_ACCESS=AOSVALUE_GRANT|AOSVALUE_DENY>
	// 			...
	// 		</AOSTAG_OPERATIONS>
	// 	</opr_ard>
	OmnString docstr = "<oprard ";
	docstr << AOSTAG_OBJID << "=\"" << objid 
		<< "\" " << AOSTAG_OTYPE << "=\"" << AOSOTYPE_USER_OPRARD 
		<< "\"><" << AOSTAG_OPERATIONS << ">";

	vector<OmnString> opr_codes;
	aos_assert_r(AosCommonValues::pickUniqueOprCodes(opr_codes), 0);
	for (u32 i=0; i<opr_codes.size(); i++)
	{
		docstr << "<" << opr_codes[i] << " " << AOSTAG_ACCESS;
		if (OmnRandom::percent(50))
		{
			docstr << "=\"" << AOSVALUE_GRANT << "\"/>";
		}
		else
		{
			docstr << "=\"" << AOSVALUE_DENY << "\"/>";
		}
	}
	docstr << "</" << AOSTAG_OPERATIONS << "></oprard>";
	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	return doc;
}


AosXmlTagPtr
AosStUserOprArd::modifyUserOprArd(
		const AosXmlTagPtr &doc,
		const AosSengTestThrdPtr &thread)
{
	return createUserOprArd(doc->getAttrStr(AOSTAG_OBJID), thread);
}


bool 
AosStUserOprArd::checkAccess(
		const AosXmlTagPtr &domain_oprard,
		const AosStDocPtr &local_doc,
		const OmnString &operation,
		bool &granted,
		bool &denied, 
		const AosSengTestThrdPtr &thread)
{
	granted = false;
	denied = false;
	aos_assert_r(domain_oprard, false);
	aos_assert_r(operation != "", false);
	AosXmlTagPtr operations_tag = domain_oprard->getFirstChild(AOSTAG_OPERATIONS);
	if (!operations_tag) return true;
	AosXmlTagPtr oprtag = operations_tag->getFirstChild(operation);
	if (!oprtag) return true;

	OmnString vv = oprtag->getAttrStr(AOSTAG_ACCESS);
	if (vv == AOSVALUE_GRANT)
	{
		granted = true;
		return true;
	}

	if (vv == AOSVALUE_DENY)
	{
		denied = true;
		return true;
	}
	return true;
}

