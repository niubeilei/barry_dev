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
#include "TaskUtil/ReduceIILBatchOpr.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/TaskType.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/SeXmlParser.h"

AosReduceIILBatchOpr::AosReduceIILBatchOpr(
		const OmnString &type,
		const int physical_id,
		const AosXmlTagPtr &datacol_tag,
		list<AosXmlTagPtr> &info_list)
:
AosReduceTaskCreator(type),
mPhysicalId(physical_id),
mDataColTag(datacol_tag->clone(AosMemoryCheckerArgsBegin)),
mInfoList(info_list)
{
}


AosReduceIILBatchOpr::~AosReduceIILBatchOpr()
{
}


bool
AosReduceIILBatchOpr::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosReduceIILBatchOpr::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	//AosXmlTagPtr iilasm_tag = mDataColTag->getFirstChild("iilasm");
/*
	AosXmlTagPtr iilasm_tag = mDataColTag->getFirstChild("asm");
	aos_assert_r(iilasm_tag, false);

	OmnString iiltype = iilasm_tag->getAttrStr("zky_asmtype");
	if (iiltype == "hitadd" || iiltype == "hitremove")
	{
		aos_assert_r(mType == AOSACTTYPE_CREATEHITIIL, false);
	}
	else
	{
		aos_assert_r(mType == AOSACTTYPE_IILBATCHOPR, false);
	}
*/
	aos_assert_r(mInfoList.size() == 1, false);
	list<AosXmlTagPtr>::iterator itr = mInfoList.begin();
	aos_assert_r(itr != mInfoList.end(), false);

	OmnString nodestr = "<files />";
	AosXmlParser xmlparser;
	AosXmlTagPtr node = xmlparser.parse(nodestr, "" AosMemoryCheckerArgs);
	aos_assert_r(node, false);
	
	AosXmlTagPtr new_node = node->addNode(*itr);
	aos_assert_r(new_node, false);

	new_node = mDataColTag->addNode(node);
	aos_assert_r(new_node, false);

	OmnString docstr;
	docstr << "<sdoc "
		<< AOSTAG_TYPE << "=\"" << AOSTASKNAME_REDUCETASK << "\" "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\">"
		<< mDataColTag->toString()
		<< "</sdoc>";


/*
		<< "<action "
		<< AOSTAG_TYPE << "=\"" << mType << "\">"
		<< mDataColTag->toString()
		<< "<files>"
		<< (*itr)->toString()
		<< "</files>"
		<< "</action></sdoc>";
*/
	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	return true;
}


bool 
AosReduceIILBatchOpr::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	return true;
}

#endif
