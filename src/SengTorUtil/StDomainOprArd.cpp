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
#include "SengTorUtil/StDomainOprArd.h"

#include "Random/RandomUtil.h"
#include "Random/CommonValues.h"
#include "SengTorUtil/StAccess.h"
#include "XmlUtil/SeXmlParser.h"


AosXmlTagPtr 
AosStDomainOprArd::createDomainOprArd(
		const OmnString &objid,
		const AosSengTestThrdPtr &thread)
{
	// 	<opr_ard ...>
	// 		<AOSTAG_OPERATIONS>
	// 			<oprcode AOSTAG_OVERRIDE="true|false">
	// 				<access type="xxx">xxx</access>
	// 				<access type="xxx">xxx</access>
	// 				...
	// 			</oprcode>
	// 			...
	// 		</AOSTAG_OPERATIONS>
	// 	</opr_ard>
	OmnString docstr = "<oprard ";
	docstr << AOSTAG_OBJID << "=\"" << objid 
		<< "\" " << AOSTAG_OTYPE << "=\"" << AOSOTYPE_DOMAIN_OPRARD 
		<< "\"><" << AOSTAG_OPERATIONS << ">";

	vector<OmnString> opr_codes;
	aos_assert_r(AosCommonValues::pickUniqueOprCodes(opr_codes), 0);
	for (u32 i=0; i<opr_codes.size(); i++)
	{
		docstr << "<" << opr_codes[i] << " " << AOSTAG_OVERRIDE;
		if (OmnRandom::percent(50))
		{
			docstr << "=\"true\">";
		}
		else
		{
			docstr << "=\"false\">";
		}

		int nn = OmnRandom::intByRange(
						1, 3, 80, 
						4, 7, 20,
						8, 20, 3);
		for (int kk=0; kk<nn; kk++)
		{
			OmnString ss = AosStAccess::randAccessXmlStr("access", thread);
			if (ss != "")
			{
				docstr << ss;
			}
		}
		docstr << "</" << opr_codes[i] << ">";
	}
	docstr << "</" << AOSTAG_OPERATIONS << "></oprard>";
	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	return doc;
}


AosXmlTagPtr
AosStDomainOprArd::modifyDomainOprArd(
		const AosXmlTagPtr &doc,
		const AosSengTestThrdPtr &thread)
{
	return createDomainOprArd(doc->getAttrStr(AOSTAG_OBJID), thread);
}


bool 
AosStDomainOprArd::checkAccess(
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

	bool rslt;
	AosXmlTagPtr record = oprtag->getFirstChild();
	while (record)
	{
		rslt = AosStAccess::checkAccessByOprStatic(
				record, local_doc, granted, denied, thread);
		aos_assert_r(rslt, false);
		if (granted || denied) return true;
		record = oprtag->getNextChild();
	}

	if (oprtag->getAttrBool(AOSTAG_OVERRIDE, false))
	{
		denied = true;
	}
	return true;
}

