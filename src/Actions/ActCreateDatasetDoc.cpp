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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2013/04/26	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActCreateDatasetDoc.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"


AosActCreateDatasetDoc::AosActCreateDatasetDoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_CREATEDATASETDOC, AosActionType::eCreateDatasetDoc, flag)
{
}


AosActCreateDatasetDoc::~AosActCreateDatasetDoc()
{
}


bool
AosActCreateDatasetDoc::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if(!task)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mTask = task;
	aos_assert_r(sdoc, false);

	OmnTagFuncInfo << "sdoc is: " << sdoc->toString() << endl;

//	AosXmlTagPtr filestag = sdoc->getFirstChild("files");
//	if(filestag)
//	{
//		OmnString objid = sdoc->getAttrStr("zky_file_objid", "");
//		aos_assert_r(objid != "", false);
//
//		AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
//		if (!doc)
//		{
//			OmnString docstr;
//			docstr << "<data "
//				<< AOSTAG_PUBLIC_DOC << "=\"" << "true" << "\" "
//				<< AOSTAG_CTNR_PUBLIC << "=\"" << "true" << "\" "
//				<< AOSTAG_OBJID << "=\"" << objid << "\" >"
//				<< filestag->toString()
//				<< "</data>";
//			doc = AosCreateDoc(docstr, true, rdata);
//			aos_assert_r(doc, 0);
//		}
//		else
//		{
//			doc = doc->clone(AosMemoryCheckerArgsBegin);
//			doc->addNode(filestag);
//			bool rslt = AosModifyDoc(doc, rdata);
//			aos_assert_r(rslt, false);
//		}
//	}
//	else
//	{
		AosXmlTagPtr dataset_conf = sdoc->getFirstChild("dataset");
		aos_assert_r(dataset_conf, false);

		OmnString name = dataset_conf->getAttrStr("zky_dataset_name");
		OmnString otype = dataset_conf->getAttrStr(AOSTAG_OTYPE);

		vector<OmnString> key_names;
		key_names.push_back("zky_dataset_name");
		key_names.push_back(AOSTAG_OTYPE);

		vector<OmnString> key_values;
		key_values.push_back(name);
		key_values.push_back(otype);

		AosXmlTagPtr doc = AosGetDocByKeys(rdata, "", key_names, key_values);
		if (doc)
		{
			u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
			aos_assert_r(docid, false);
			AosDeleteDocByDocid(docid, rdata);
			OmnScreen << "KKKKKKKKKKK delete doc docid:" << docid << endl;

			OmnTagFuncInfo << "doc is: " << doc->toString() << endl;
		}

		OmnString docstr = dataset_conf->toString();
		doc = AosCreateDoc(docstr, true, rdata);
		aos_assert_r(doc, 0);
OmnScreen << "AosActCreateDatasetDoc AosActCreateDatasetDoc;name:" << name << ";docid:" << doc->getAttrU64(AOSTAG_DOCID, 0) << ";"<< endl;
OmnTagFuncInfo << "The doc is: " << doc->toString() << endl;
//	}
	actionFinished(rdata);
	return true;
}


AosActionObjPtr
AosActCreateDatasetDoc::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActCreateDatasetDoc(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

