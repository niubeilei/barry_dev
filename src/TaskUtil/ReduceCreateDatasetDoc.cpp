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
#include "TaskUtil/ReduceCreateDatasetDoc.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/TaskType.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/SeXmlParser.h"

AosCreateDatasetDoc::AosCreateDatasetDoc(
		const OmnString &type,
		const AosXmlTagPtr &dataset_conf,
		const int physical_id,
		list<AosXmlTagPtr> &doc_list)
:
AosReduceTaskCreator(type),
mPhysicalId(physical_id),
mDocList(doc_list),
mDatasetConf(dataset_conf)
{
}


AosCreateDatasetDoc::~AosCreateDatasetDoc()
{
}


bool
AosCreateDatasetDoc::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosCreateDatasetDoc::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mType == AOSACTTYPE_CREATEDATASETDOC, false);
	OmnString docstr;
	docstr << "<sdoc "
		<< AOSTAG_TYPE << "=\"" << AOSTASKNAME_REDUCETASK << "\" "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\">"
		<< "<action "
		<< AOSTAG_TYPE << "=\"" << mType << "\" >";

	OmnString files_str = "<files>";
	list<AosXmlTagPtr>::iterator itr = mDocList.begin();
	while (itr != mDocList.end())
	{
		files_str << (*itr)->toString();
		itr++;
	}
	files_str << "</files>";

	aos_assert_r(mDatasetConf, false);

	OmnString name = mDatasetConf->getAttrStr(AOSTAG_NAME, "");
	mDatasetConf->setAttr("zky_dataset_name", name);
	mDatasetConf->setAttr(AOSTAG_OTYPE, "dataset");

	AosXmlTagPtr datascanner_conf = mDatasetConf->getFirstChild("datascanner");
	aos_assert_r(datascanner_conf, false);

	AosXmlTagPtr dataconnector_conf = datascanner_conf->getFirstChild("dataconnector");
	aos_assert_r(dataconnector_conf, false);

	AosXmlTagPtr files_tag = AosXmlParser::parse(files_str AosMemoryCheckerArgs);
	aos_assert_r(files_tag, false);
	dataconnector_conf->addNode(files_tag);

	docstr << mDatasetConf->toString();
	docstr << "</action></sdoc>";

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	return true;
}


bool 
AosCreateDatasetDoc::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	return true;
}

#endif
