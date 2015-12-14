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
// 2013/05/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActImportDataFileFormat.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataCalcDyn/DataCalcNames.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/StrParser.h"
#include "Util/ValueRslt.h"
#include "Util/DirDesc.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "time.h"

const u64 MAXSIZE = 1024 * 1024 * 1024;

AosActImportDataFileFormat::AosActImportDataFileFormat(const bool flag)
:
AosSdocAction(AOSACTTYPE_IMPUTDATAFILEFORMAT, AosActionType::eImputDataFileFormat, flag)
{
}


AosActImportDataFileFormat::~AosActImportDataFileFormat()
{
}

bool
AosActImportDataFileFormat::getFileName(
		vector<OmnString> &file_names,
		const OmnString &path,
		const int rec_level,
		const int crt_level,
		const AosRundataPtr &rdata)
{
	DIR * pDir = opendir(path.data());
	aos_assert_r(pDir, false);

	bool rslt = true;
	int ret = 0;
	int new_level = crt_level + 1;
	struct stat st;
	struct dirent *ent;
	OmnString childPath;

	while ((ent = readdir(pDir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		{
			continue;
		}

		childPath = path;
		childPath << "/" << ent->d_name;

		ret = stat(childPath.data(), &st);
		if (ret < 0) 
		{
			continue;
		}

		if (S_ISREG(st.st_mode))
		{
			if (st.st_size > 0)
			{
				aos_assert_r(childPath != "", false);
				file_names.push_back(childPath);			
			}
			continue;
		}

		if (S_ISDIR(st.st_mode))
		{
			if(rec_level == -1 || rec_level >= new_level)
			{
				rslt = getFileName(file_names, childPath, rec_level, new_level, rdata);
				aos_assert_r(rslt, false);
			}
			continue;
		}
	}

	closedir(pDir);
	return true;
}


bool
AosActImportDataFileFormat::run(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// This function creates one doc for each file. 'def' is in the form:
	// <action dirname = "xxxx"  zky_format="xxxx" zky_isneed_seqno = "xxxx">
	// </action>
	OmnScreen << "start time: " << OmnGetSecond() << endl; 

	bool isDir = def->getAttrBool("zky_is_dir", false);

	OmnString dir_name = def->getAttrStr("dirname", "");
	aos_assert_rr(dir_name != "", rdata, false);

	OmnString format = def->getAttrStr("zky_format", "");
	aos_assert_r(format != "", false);

	bool is_access_record = def->getAttrBool("zky_is_access_record", false);

	vector<OmnString> file_names;
	bool rslt = false;
	if (isDir)
	{
		rslt = getFileName(file_names, dir_name, -1, -1, rdata);
		aos_assert_r(rslt, false);
	}
	else
	{
		file_names.push_back(dir_name);
	}

	if (format == "file") 
	{
		rslt = readFileOfFileFrmat(file_names, is_access_record, rdata);
		aos_assert_r(rslt, false);
	}
	if (format == "xml")
	{
		bool isneed_seqno = def->getAttrBool("zky_isneed_seqno", false);
		rslt = readFileOfXmlFrmat(file_names, isneed_seqno, is_access_record, rdata);
		aos_assert_r(rslt, false);
	}
	if (format == "record")
	{
		OmnString ctnr = def->getAttrStr(AOSTAG_PUB_CONTAINER, "");
		OmnString sep = def->getAttrStr(AOSTAG_SEPARATOR, "|");
		OmnString logpath = def->getAttrStr("zky_logpath", "");
		bool is_need_attr = def->getAttrBool("zky_isneed_attr", true);
		rslt = readFileOfRecordFrmat(file_names, is_need_attr, is_access_record, ctnr, sep, logpath, rdata);
		aos_assert_r(rslt, false);
	}
	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActImportDataFileFormat::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActImportDataFileFormat(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActImportDataFileFormat::writeAccessDoc(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc, false);
	return false;
//	OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
//	u64 docid = clinet->getDocidByObjid(objid, rdata);
//	aos_assert_r(docid > 0, false); 
//
//	doc->removeAttr(AOSTAG_DOCID);
//	doc->removeAttr(AOSTAG_OBJID);
//
//	doc->setAttr(AOSTAG_OWNER_DOCID, docid);
//	return AosDocClientObj::getDocClient()->writeAccessDoc(docid, doc, rdata);
}


bool
AosActImportDataFileFormat::readFileOfFileFrmat(
		vector<OmnString> &file_names,
		const bool is_access_record,
		const AosRundataPtr &rdata)
{
	for (u32 i = 0; i < file_names.size(); i++)
	{
		OmnFilePtr file = OmnNew OmnFile(file_names[i], OmnFile::eReadOnly AosMemoryCheckerArgs);

		aos_assert_r(OmnFile::getFileLengthStatic(file_names[i]) < MAXSIZE, false);
		//aos_assert_r(file->getLength() < MAXSIZE, false);

		OmnString str;
		bool rslt = file->readToString(str);
		aos_assert_r(rslt, false);

		aos_assert_r(str != "", false);
		AosXmlTagPtr doc = AosXmlParser::parse(str AosMemoryCheckerArgs); 

		if (!doc) continue;
		if (is_access_record) writeAccessDoc(doc, rdata);
		else AosXmlTagPtr newdoc = AosCreateDoc(doc, rdata);
		//aos_assert_r(newdoc, false);
	}
	return true;
}


bool
AosActImportDataFileFormat::readFileOfRecordFrmat(
		vector<OmnString> &file_names,
		const bool is_need_attr,
		const bool is_access_record,
		const OmnString &ctnr,
		const OmnString &sep,
		const OmnString &logpath,
		const AosRundataPtr &rdata)
{
	aos_assert_r(is_need_attr, false);
	AosStrSplit split;


	u32 offset = 0;
	OmnFilePtr logfile;
	if (logpath != "")
	{
		logfile = OmnNew OmnFile(logpath, OmnFile::eReadWrite AosMemoryCheckerArgs);
		aos_assert_r(logfile && logfile->isGood(), false);
		logfile->resetFile();
	}

	for (u32 i = 0; i < file_names.size(); i++)
	{
		OmnFilePtr file = OmnNew OmnFile(file_names[i], OmnFile::eReadOnly AosMemoryCheckerArgs);
		aos_assert_r(OmnFile::getFileLengthStatic(file_names[i]) < MAXSIZE, false);

		OmnString str;
		bool rslt = file->readToString(str);
		aos_assert_r(rslt, false);
		aos_assert_r(str != "", false);

		vector<OmnString> allRowList; 
		OmnString linestr = str.getLine(true, 1);
		while (linestr != "")
		{
			u32 len = linestr.length();
			while(linestr[len - 1] == '\n' || linestr[len - 1] == '\r')
			{
				linestr[len -1] = '\0';
				len = len -1;
			}

			allRowList.push_back(linestr);
			linestr = str.getLine(true, 1);
		}

		vector<OmnString> attrNameList = split.entriesV(); 
		vector<OmnString> attrValueList;

		split.splitStr(allRowList[0].data(), sep.data());
		attrNameList = split.entriesV(); 
			
		for(u32 k = 0; k < attrNameList.size(); k++)
			attrNameList[k].normalizeWhiteSpace(true, true);

		OmnString lac;
		OmnString cellid;
		for (u32 i = 1; i < allRowList.size(); i++)
		{
			attrValueList.clear();
			split.splitStr(allRowList[i].data(),sep.data());
			attrValueList = split.entriesV();
			aos_assert_r(attrNameList.size() == attrValueList.size(), false);

			OmnString xml_str;
			OmnString temstr;
			xml_str << "<doc "
					<< AOSTAG_CTNR_PUBLIC << "=\"" << "true" << "\" "
					<< AOSTAG_PUBLIC_DOC << "=\"" << "true" << "\" ";

			for(u32 k = 0; k < attrNameList.size(); k++)
			{
				temstr = attrNameList[k];
				temstr.toLower();
				attrValueList[k].normalizeWhiteSpace(true, true);  // trim
				if (temstr == "lac") lac = attrValueList[k];
				if (temstr == "cell_id") cellid = attrValueList[k];
				xml_str << attrNameList[k] << "=\"" << attrValueList[k] << "\" " ;
			}

			if (lac != "" && cellid != "") 
				xml_str << "lac_cellid=\"" << lac << "_" << cellid << "\" " ;

			lac = cellid = "";
			xml_str << " />" ;

			AosXmlTagPtr doc = AosStr2Xml(rdata.getPtr(), xml_str AosMemoryCheckerArgs);	
			aos_assert_r(doc, false);
			doc->setAttr(AOSTAG_PARENTC, ctnr);
			AosXmlTagPtr newdoc;
			if (is_access_record) writeAccessDoc(doc, rdata);
			else newdoc = AosCreateDoc(doc, rdata);

			if (newdoc && logfile && logpath != "")
			{
				OmnString docid = newdoc->getAttrStr("zky_docid", "");
				docid << ", ";
				u32 docidlen = docid.length();
				logfile->put(offset, docid.data(), docidlen, rdata);
				offset += docidlen;
			}
		}
	}

	OmnScreen << "end time: " << OmnGetSecond()<< endl; 
	return true;
}


bool
AosActImportDataFileFormat::readFileOfXmlFrmat(
		vector<OmnString> &file_names,
		const bool isneed_seqno,
		const bool is_access_record,
		const AosRundataPtr &rdata)
{
	for (u32 i = 0; i < file_names.size(); i++)
	{
		OmnFilePtr file = OmnNew OmnFile(file_names[i], OmnFile::eReadOnly AosMemoryCheckerArgs);

		aos_assert_r(OmnFile::getFileLengthStatic(file_names[i]) < MAXSIZE, false);
	//	aos_assert_r(file->getLength() < MAXSIZE, false);

		OmnString str;
		bool rstl = file->readToString(str);
		aos_assert_r(rstl, false);

		AosStrSplit split(str, "\n"); 
		vector<OmnString> docs= split.entriesV(); 

		for (u32 j = 0; j < docs.size(); j++)
		{
			if (isneed_seqno) 
			{
				j++;
				if (j == docs.size()) continue;
			}
			OmnString docstr = docs[j];
			AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs); 
			if (!doc) continue;
			if (is_access_record) writeAccessDoc(doc, rdata);
			else AosXmlTagPtr newdoc = AosCreateDoc(doc, rdata);
			//aos_assert_r(newdoc, false);
		}
	}
	return true;
}

