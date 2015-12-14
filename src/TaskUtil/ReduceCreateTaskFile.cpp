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
// 2014/08/20 Created By Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "TaskUtil/ReduceCreateTaskFile.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/TaskType.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/SeXmlParser.h"

AosReduceCreateTaskFile::AosReduceCreateTaskFile(
		const OmnString &type,
		const int physical_id,
		const AosXmlTagPtr &dataproc_tag,
		list<AosXmlTagPtr> &doc_list)
:
AosReduceTaskCreator(type),
mPhysicalId(physical_id),
mDocList(doc_list)
{
	mFileDefName = dataproc_tag->getAttrStr("zky_file_defname", "");
	aos_assert(mFileDefName != "");
}


AosReduceCreateTaskFile::~AosReduceCreateTaskFile()
{
}


bool
AosReduceCreateTaskFile::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosReduceCreateTaskFile::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<sdoc "
		<< AOSTAG_TYPE << "=\"" << AOSTASKNAME_REDUCETASK << "\" "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\">"
		<< "<action "
		<< AOSTAG_TYPE << "=\"" << mType << "\" >";
	
	OmnString files_str = "<files ";
	files_str << "zky_file_defname=\"" << mFileDefName << "\" >";

	list<AosXmlTagPtr>::iterator itr = mDocList.begin();
	while (itr != mDocList.end())
	{
		files_str << (*itr)->toString();
		itr++;
	}
	files_str << "</files>";

	docstr << files_str << "</action></sdoc>";

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	return true;
}


bool 
AosReduceCreateTaskFile::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	return true;
}

#endif
