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
#include "TaskUtil/ReduceMergeFile.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/TaskType.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/SeXmlParser.h"

AosReduceMergeFile::AosReduceMergeFile(
		const OmnString &type,
		const int physical_id,
		const AosXmlTagPtr &datacol_tag,
		list<AosXmlTagPtr> &info_list)
:
AosReduceTaskCreator(type),
mPhysicalId(physical_id),
mDataColTag(datacol_tag),
mInfoList(info_list)
{
}


AosReduceMergeFile::~AosReduceMergeFile()
{
}


bool
AosReduceMergeFile::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosReduceMergeFile::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr tag = *(mInfoList.begin());
	aos_assert_r(tag, false);
	int level = tag->getAttrInt(AOSTAG_LEVEL, -1);
	aos_assert_r(mType == AOSACTTYPE_MERGEFILE, false);

	OmnString docstr;
	docstr << "<sdoc "
		<< AOSTAG_TYPE << "=\"" << AOSTASKNAME_REDUCETASK << "\" "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\">"
		<< "<dataproc "
		<< AOSTAG_TYPE << "=\"" << mType << "\" "
		<< AOSTAG_LEVEL << "=\"" << level << "\">"
		<< mDataColTag->toString()
		<< "<files>";
	list<AosXmlTagPtr>::iterator itr = mInfoList.begin();
	AosXmlTagPtr output_tag;
	while(itr != mInfoList.end())
	{
		output_tag = *itr;
		docstr << output_tag->toString();
		itr++;
	}
	docstr << "</files>";
	docstr << "</dataproc></sdoc>";

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	return true;
}


bool 
AosReduceMergeFile::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	return true;
}

#endif
