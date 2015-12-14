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
#include "SmartDoc/SdocOutputDataXmlFormat.h"

#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "Actions/SdocAction.h"
#include "Conds/Condition.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/AccessRcd.h"
#include "Util/DirDesc.h"
#include "Util/File.h"
#include "ValueSel/ValueSel.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#if 0
const u32 PERMS = 0777;
const u32 FILEMAXLEN = 1024 * 1024 * 1024 + 100;

AosSdocOutputDataXmlFormat::AosSdocOutputDataXmlFormat(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_OUTPUTDATAXMLFORMAT, AosSdocId::eOutputDataXmlFormat, flag)
{
}


AosSdocOutputDataXmlFormat::~AosSdocOutputDataXmlFormat()
{
}


bool
AosSdocOutputDataXmlFormat::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
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
AosSdocOutputDataXmlFormat::runOutputData(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &userdata,
		const AosRundataPtr &rdata)
{
	//<exporter 
	//	dirname="xxx"
	//	file_max_len="xxx"
	//	zky_remove_attrs="xxxx,xxxx,xxx"
	//	zky_isneed_seqno="true";
	//  zky_sep = ""  
	//	prefix="xxx">
	//	<dataset attrs="xxx,xxx,xxx/>
	//</exporter>
	
	OmnString dirname = userdata->getAttrStr("zky_dirname", "");
	aos_assert_r(dirname != "", false);

	bool rslt = OmnApp::createDir(dirname);
	aos_assert_r(rslt, false);

	AosXmlTagPtr conds = userdata->getFirstChild(AOSTAG_CONDITION);
	bool is_out_rccess_record = userdata->getAttrBool("zky_is_out_rccess_record", false);
	OmnString prefix =	userdata->getAttrStr("prefix", ""); 
	aos_assert_r(prefix != "", false);

	u64 file_max_len = userdata->getAttrU64("file_max_len", FILEMAXLEN);
	bool isneed_seqno = userdata->getAttrBool("zky_isneed_seqno", true);
	
	u32 tmp, seqno_num;
	tmp = seqno_num = 1;
	u64 offset = 0;
	OmnString full_name;
	char buff[100];
	sprintf(buff, "%04d", tmp);
	OmnFilePtr file;
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
			if (seqno_num == 1)
			{
				full_name << dirname << prefix << buff;
				file = OmnNew OmnFile(full_name, OmnFile::eCreate AosMemoryCheckerArgs);
			}

			OmnString remove_attrs = userdata->getAttrStr("zky_remove_attrs", ""); 
			OmnString sep = userdata->getAttrStr(AOSTAG_SEPARATOR, ","); 
			if (remove_attrs != "") 
			{ 
				AosStrSplit split(remove_attrs, sep.data()); 
				vector<OmnString> attrs = split.entriesV(); 
				for (u32 i = 0; i < attrs.size(); i++) 
				{ 
					doc->removeAttr(attrs[i]); 
				} 
			} 

			doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
			doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
			rdata->setReceivedDoc(doc, true);
			OmnString str;
			if (isneed_seqno) str << seqno_num << '\n';
			str << doc->toString();
			str << '\n';

			if ((file->getLength() + str.length() + 10) < file_max_len)
			{
				rslt = writeFile(file, offset, str, rdata);
				aos_assert_r(rslt, false);
				seqno_num++;
			}
			else
			{
				++tmp;
				full_name = "";
				sprintf(buff, "%04d", tmp);
				full_name << dirname << prefix << buff;
				offset = 0;
				file = OmnNew OmnFile(full_name, OmnFile::eCreate AosMemoryCheckerArgs);
				rslt = writeFile(file, offset, str, rdata);
				aos_assert_r(rslt, false);
				seqno_num++;
			}

			if (!rslt)
			{
				OmnScreen << "=======================error docid :: " << docid << endl; 
			}

			if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_BREAK)
			{
				break;
			}
		}
	}
	rdata->setWorkingDoc(wdoc, true);
	OmnScreen << "==========SdocOutputDataXmlFormat  run success! created " << tmp << " files " << endl;
	return true;
}


//bool
//AosSdocOutputDataXmlFormat::checkDir(const OmnString &dirname)
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

bool
AosSdocOutputDataXmlFormat::writeFile(
		const OmnFilePtr &file,
		u64 &offset,
		const OmnString &str,
		const AosRundataPtr &rdata)
{
	aos_assert_r(file, false);
	file->put(offset, str.data(), str.length(), rdata);
	offset = offset + str.length();
	return true;
}

#endif
