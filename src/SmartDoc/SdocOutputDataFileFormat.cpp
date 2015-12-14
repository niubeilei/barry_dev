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
// 04/13/2011	Created by Andy 
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocOutputDataFileFormat.h"

#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "Actions/SdocAction.h"
#include "Conds/Condition.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/AccessRcd.h"
#include "XmlUtil/XmlTag.h"
#include "Util/DirDesc.h"
#include "Util/File.h"
#include "ValueSel/ValueSel.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#if 0
const u32 PERMS = 0777;

AosSdocOutputDataFileFormat::AosSdocOutputDataFileFormat(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_OUTPUTDATAFILEFORMAT, AosSdocId::eOutputDataFileFormat, flag)
{
}


AosSdocOutputDataFileFormat::~AosSdocOutputDataFileFormat()
{
}


bool
AosSdocOutputDataFileFormat::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if (sdoc->getAttrStr(AOSTAG_OPERATION) == "simulate")
	{
		rdata->setSdocReturnStaus(AOSTAG_STATUS_BREAK);
	}

	AosXmlTagPtr userdata = sdoc->getFirstChild("zky_userdata");
	aos_assert_r(userdata, false);

	// Query all needed doc 
	AosValueRslt valueRslt;
	bool rslt = AosValueSel::getValueStatic(valueRslt, sdoc, AOSTAG_VALUEDEF, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eFailedGetValue);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	switch(valueRslt.getType())
	{
		case AosDataType::eQueryRslt:
			return runOutputData(valueRslt, userdata, rdata);

		default:
			OmnAlarm << "Undefined type" << enderr;
			return false;
	}
	
	OmnShouldNeverComeHere;
	return false;
}


bool
AosSdocOutputDataFileFormat::runOutputData(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &userdata,
		const AosRundataPtr &rdata)
{
	//  <zky_userdata 
	//  	zky_dirname="xxx" 
	//  	zky_max_file_number="xxx"
	//  	zky_remove_attrs="xxxx,xxxx,xxx",
	//  	zky_is_out_rccess_record="true||false"
	//  	zky_sep = ""  
	//  	<conds>
	//  		<cond>
	//  		..............
	//  		</cond>
	//  	</conds>
	//  </zky_userdata>
	u32 tmp = 0; 
	u32 dir_num = 0;
	u32 max_file_number = userdata->getAttrU32("zky_max_file_number", 1000); 
	bool is_out_rccess_record = userdata->getAttrBool("zky_is_out_rccess_record", false);
	OmnString dirname = userdata->getAttrStr("zky_dirname", "");
	aos_assert_r(dirname != "", false);

	AosXmlTagPtr conds = userdata->getFirstChild(AOSTAG_CONDITION);
	bool rslt = OmnApp::createDir(dirname);
	//bool rslt = checkDir(dirname);
	aos_assert_r(rslt, false);

	AosXmlTagPtr wdoc = rdata->getWorkingDoc();
	AosAccessRcdPtr accessRcd;	

	while (valueRslt.hasMore())
	{
		u64 docid;
		if (!valueRslt.getU64Value(docid, rdata.getPtr())) return false;
		if (docid != 0)
		{
			AosXmlTagPtr doc;
			if (is_out_rccess_record)
			{
				// Chen Ding, 2013/12/15
				// accessRcd = AosGetAccessRecord(docid, false, rdata);
				accessRcd = AosGetAccessRecord(rdata, docid, false);
				if (!accessRcd) continue;
				doc = accessRcd->getDoc();
				aos_assert_r(doc, false);

				u64	owndocid = doc->getAttrU64(AOSTAG_OWNER_DOCID, 0);
				aos_assert_r(owndocid > 0, false);

				AosXmlTagPtr newdoc = AosDocClientObj::getDocClient()->getDocByDocid(owndocid, rdata);
				aos_assert_r(newdoc, false);

				OmnString objid = newdoc->getAttrStr(AOSTAG_OBJID, "");
				aos_assert_r(objid != "", false);
				doc->setAttr(AOSTAG_OBJID, objid);
			}
			else doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);

			if (!doc) continue;
			if (conds)
			{
				rdata->setWorkingDoc(doc, true);
				rslt = AosCondition::evalCondStatic(conds, rdata);
				if (!rslt) continue;
			}

			if((tmp % max_file_number) == 0) 
			{

				rslt = creatDir(userdata, dirname, dir_num, rdata);
				aos_assert_r(rslt ,false);
			}

			doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
			doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
			rslt = writeFile(userdata, rdata); 
			if (!rslt)
			{
				OmnScreen << "=======================error docid :: " << docid << endl; 
			}

			++tmp;
			if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_BREAK)
			{
				break;
			}
		}
	}
	rdata->setWorkingDoc(wdoc, true);
	OmnScreen << "==========SdocOutputDataFileFormat  run success! created " << tmp << " files " << endl;
	return true;
}


bool 
AosSdocOutputDataFileFormat::creatDir(
		const AosXmlTagPtr &userdata,
		const OmnString dirname,
		u32 &dir_num,
		const AosRundataPtr &rdata)
{
	OmnString new_dir; 
	new_dir << dirname << "/" << dir_num; 
	struct stat st = {0};

	if (stat(new_dir.data(), &st) == -1)
	{
		OmnApp::createDir(new_dir); 
	}
	userdata->setAttr("zky_dirname", new_dir.data());
	++dir_num;
	return true;	
}

bool
AosSdocOutputDataFileFormat::writeFile(
		const AosXmlTagPtr &userdata,
		const AosRundataPtr &rdata)
{
	//<zky_userdata 
	//      zky_dirname="xxx" 
	//      zky_max_file_number="xxx"
	//      zky_remove_attrs="xxxx,xxxx,xxx",
	//      zky_sep = ""  
	// </zky_userdata>

	OmnString dirname = userdata->getAttrStr("zky_dirname", ""); 
	aos_assert_r(dirname != "", false); 

	OmnString remove_attrs = userdata->getAttrStr("zky_remove_attrs", ""); 
	OmnString sep = userdata->getAttrStr(AOSTAG_SEPARATOR, ","); 

	AosXmlTagPtr doc = rdata->getReceivedDoc(); 
	aos_assert_r(doc, false); 

	if (remove_attrs != "") 
	{ 
		AosStrSplit split(remove_attrs, sep.data()); 
		vector<OmnString> attrs = split.entriesV(); 
		for (u32 i = 0; i < attrs.size(); i++) 
		{ 
			doc->removeAttr(attrs[i]); 
		} 
	} 

	OmnString str_doc = doc->toString(); 
	aos_assert_r(str_doc != "", false);
	OmnString objid =  doc->getAttrStr(AOSTAG_OBJID, ""); 
	aos_assert_r(objid != "", false); 
	int idx = objid.indexOf(0, '/');
	if (idx != -1) return false;

	OmnString full_name; 
	full_name << dirname << "/" <<  objid << ".xml"; 

	OmnFile file(full_name, OmnFile::eCreate AosMemoryCheckerArgs); 
	aos_assert_r(file.getFile() != NULL, false);
	file.put(0, str_doc.data(), str_doc.length(), rdata); 
	return true; 
}


//bool
//AosSdocOutputDataFileFormat::checkDir(const OmnString &dirname)
//{
//	struct stat st = {0}; 
//	if (stat(dirname.data(), &st) == -1) 
//	{ 
//		OmnString tmpDirName; 
//		vector<OmnString> dirnames; 
//		AosStrSplit split(dirname, "/");     
//		dirnames  = split.entriesV(); 
//		for (u32 i = 0; i < dirnames.size(); i++) 
//		{ 
//			tmpDirName << "/" << dirnames[i]; 
//			if (stat(tmpDirName.data(), &st) == -1) 
//			{ 
//				int rslt = mkdir(tmpDirName.data(), PERMS);  
//				aos_assert_r(rslt != -1, false);
//			} 
//		} 
//	} 
//	return true;
//}                                                             

#endif
