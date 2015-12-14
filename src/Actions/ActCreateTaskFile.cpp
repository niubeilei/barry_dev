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
// 2014/08/20	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActCreateTaskFile.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosActCreateTaskFile::AosActCreateTaskFile(const bool flag)
:
AosSdocAction(AOSACTTYPE_CREATETASKFILE, AosActionType::eCreateTaskFile, flag)
{
}


AosActCreateTaskFile::~AosActCreateTaskFile()
{
}


bool
AosActCreateTaskFile::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	//	<action type=\"createtaskfile\">
	//		<files zky_file_defname=\"statname2_0_all_files\" >
	//			<task_data__n zky_physicalid=\"0\" zky_datacolid=\"statname2_0_all_0\" zky_storage_fileid=\"9223372036854775910\" zky_level=\"0\" type=\"output_iil\" zky_from_task_docid=\"8129\" zky_length=\"4148265\" zky_startpos=\"0\"//>
	//			...
	//		</files>
	//	</action>

	if(!task)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mTask = task;
	aos_assert_r(sdoc, false);

	AosXmlTagPtr files_conf = sdoc->getFirstChild("files");
	aos_assert_r(files_conf, false);

	OmnString def_name = files_conf->getAttrStr("zky_file_defname");
	aos_assert_r(def_name != "", false);
	files_conf->setAttr(AOSTAG_OBJID, def_name);
	files_conf->setAttr(AOSTAG_PUBLIC_DOC, "true");
	files_conf->setAttr(AOSTAG_CTNR_PUBLIC, "true");

	OmnString docstr = files_conf->toString();
	AosXmlTagPtr doc = AosCreateDoc(docstr, true, rdata);
	aos_assert_r(doc, false);

	OmnScreen << "AosActCreateTaskFile AosActCreateTaskFile;name:" << def_name
		<< ";docid:" << doc->getAttrU64(AOSTAG_DOCID, 0) << ";"<< endl;
	
	actionFinished(rdata);
	return true;
}


AosActionObjPtr
AosActCreateTaskFile::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActCreateTaskFile(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

