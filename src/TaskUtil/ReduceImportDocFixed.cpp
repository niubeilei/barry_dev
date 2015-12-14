////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2014/01/08 Created By Linda  
////////////////////////////////////////////////////////////////////////////
#if 0
#include "TaskUtil/ReduceImportDocFixed.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/TaskType.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/SeXmlParser.h"

AosReduceImportDocFixed::AosReduceImportDocFixed(
		const OmnString &type,
		const int physical_id,
		list<AosXmlTagPtr> &doc_list)
:
AosReduceTaskCreator(type),
mPhysicalId(physical_id),
mDocList(doc_list)
{
}


AosReduceImportDocFixed::~AosReduceImportDocFixed()
{
}


bool
AosReduceImportDocFixed::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosReduceImportDocFixed::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mType == AOSACTTYPE_IMPORTDOC_FIXED, false);

	OmnString docstr;
	docstr << "<sdoc "
		<< AOSTAG_TYPE << "=\"" << AOSTASKNAME_REDUCETASK << "\" "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\">"
		<< "<dataproc "
		<< AOSTAG_TYPE << "=\"" << mType << "\">"
		<< "<files>";

	list<AosXmlTagPtr>::iterator itr = mDocList.begin();
	while (itr != mDocList.end())
	{
		docstr << (*itr)->toString();
		itr++;
	}
	docstr << "</files></dataproc></sdoc>";

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	return true;
}


bool 
AosReduceImportDocFixed::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	return true;
}

#endif
