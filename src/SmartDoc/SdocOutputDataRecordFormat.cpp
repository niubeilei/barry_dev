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
#include "SmartDoc/SdocOutputDataRecordFormat.h"

#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "Actions/SdocAction.h"
#include "Conds/Condition.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
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
const u32 FILEMAXLEN = 1024 * 1024 * 1024 + 100;

AosSdocOutputDataRecordFormat::AosSdocOutputDataRecordFormat(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_OUTPUTDATARECORDFORMAT, AosSdocId::eOutputDataRecordFormat, flag)
{
}


AosSdocOutputDataRecordFormat::~AosSdocOutputDataRecordFormat()
{
}


bool
AosSdocOutputDataRecordFormat::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
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
AosSdocOutputDataRecordFormat::runOutputData(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &userdata,
		const AosRundataPtr &rdata)
{
	//<exporter 
	//	dirname="xxx"
	//	file_max_len="xxx"
	//	zky_remove_attrs="xxxx,xxxx,xxx"
	//  zky_sep = ""  
	//  zky_isneed_attr=""
	//	prefix="xxx">
	//	<dataset attrs="xxx,xxx,xxx/>
	//  <outputfile>
	// 		file_format="xml_format|record_format"
	// 	</outputfile>
	//</exporter>
	
	OmnString dirname = userdata->getAttrStr("zky_dirname", "");
	aos_assert_r(dirname != "", false);

	bool rslt = OmnApp::createDir(dirname);
	aos_assert_r(rslt, false)

	OmnString prefix =	userdata->getAttrStr("prefix", ""); 
	aos_assert_r(prefix != "", false);

	u64 file_max_len = userdata->getAttrU64("file_max_len", FILEMAXLEN);
	AosXmlTagPtr conds = userdata->getFirstChild(AOSTAG_CONDITION);
	bool isneed_attr = userdata->getAttrBool("zky_isneed_attr", true);
	AosXmlTagPtr wdoc = rdata->getWorkingDoc();
	
	u32 tmp = 1;
	u64 offset = 0;
	OmnString full_name;
	char buff[100];
	OmnFilePtr file;

	while (valueRslt.hasMore())
	{
		u64 docid;
		if (!valueRslt.getU64Value(docid, rdata.getPtr())) return false;
		if (docid != 0)
		{
			AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
			if (!doc) continue;
			if (conds)
			{
				rdata->setWorkingDoc(doc, true);                     
				rslt = AosCondition::evalCondStatic(conds, rdata);
				if (!rslt) continue;
			}   
			if (!file)
			{
				sprintf(buff, "%04d", tmp);
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
		
			OmnString str;
			AosXmlTagPtr dataset = userdata->getFirstChild("dataset");
			aos_assert_r(dataset, false);
			OmnString attrs_str = dataset->getAttrStr("attrs", "");
			vector<OmnString> attrs;
			AosStrSplit split(attrs_str, ","); 
			attrs = split.entriesV();					

			if (offset == 0 && isneed_attr)
			{
				for(u32 i = 0; i < attrs.size(); i++)
				{
					buff[0] = '\0';
					sprintf(buff, "%100s", attrs[i].data());
					str << buff;
				}
				str << '\n';
				rslt = writeFile(file, offset, str, rdata);
				aos_assert_r(rslt, false);
			}
			else
			{
				for(u32 i = 0; i < attrs.size(); i++)
				{
					OmnString value = doc->getAttrStr(attrs[i], "");
					buff[0] = '\0';
					sprintf(buff, "%100s", value.data()); 
					str << buff;
				}
				str << "\n";
				if ((file->getLength() + str.length() + 10) < file_max_len)
				{
					if (isneed_attr)
					{
						rslt = writeFile(file, offset, str, rdata);
						aos_assert_r(rslt, false);
					}
				}
				else
				{
					++tmp;
					full_name = "";
					OmnString title_str;
					sprintf(buff, "%04d", tmp);
					full_name << dirname << prefix << buff;
					offset = 0;
					file = OmnNew OmnFile(full_name, OmnFile::eCreate AosMemoryCheckerArgs);

					for(u32 i = 0; i < attrs.size(); i++)
					{
						buff[0] = '\0';
						sprintf(buff, "%100s", attrs[i].data());
						title_str << buff;
					}
					title_str << '\n';
					
					if (isneed_attr)
					{
						rslt = writeFile(file, offset, title_str, rdata);
						aos_assert_r(rslt, false);
					}
					rslt = writeFile(file, offset, str, rdata);
					aos_assert_r(rslt, false);
				}
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
	OmnScreen << "==========SdocOutputDataRecordFormat  run success! created " << tmp << " files " << endl;
	return true;
}


//bool
//AosSdocOutputDataRecordFormat::checkDir(const OmnString &dirname)
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
AosSdocOutputDataRecordFormat::writeFile(
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
