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
// This data proc uses one data element to convert/process one data field.
//
//
// Modification History:
// 04/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcUnicomDocument.h"
#include "API/AosApi.h"
#include "Thread/ThreadMgr.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "SEUtil/IILName.h"
#include "SEInterfaces/DocidMgrObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "WordParser/WordParser.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/SdocDocument.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"

#if 0
const bool IS_PUBLIC = true;
AosDocProcUtil::AosSeWordHash   AosDataProcUnicomDocument::mWords[eMaxThrdIds];
AosDocProcUtil::AosSeAttrHash   AosDataProcUnicomDocument::mAttrs[eMaxThrdIds];

AosDataProcUnicomDocument::AosDataProcUnicomDocument(const bool flag)
:
AosDataProc(AOSDATAPROC_UNICOMDOCUMENT, AosDataProcId::eUnicomDocument, flag),
mLock(OmnNew OmnMutex()),
mDocNum(0)
{
}

	
AosDataProcUnicomDocument::AosDataProcUnicomDocument(const AosDataProcUnicomDocument &proc)
:
AosDataProc(proc),
mLock(OmnNew OmnMutex()),
mDocNum(0)
{
}

AosDataProcUnicomDocument::~AosDataProcUnicomDocument()
{
}


bool
AosDataProcUnicomDocument::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);

	AosXmlTagPtr input_tag = def->getFirstChild("input");
	aos_assert_r(input_tag, false);

	OmnString input_name_field = input_tag->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	aos_assert_r(input_name_field != "", false);
	
	mInput.init(input_name_field, true);
	
	bool rslt;
	rslt = getOutPuts(def, rdata);
	aos_assert_r(rslt, false);

	mAttrMap.clear();

	mUuicomTitlePctrs = def->getAttrStr("unicom_title_ctnr", "");
	aos_assert_r(mUuicomTitlePctrs != "", false)
	
	mUuicomFilePctrs = def->getAttrStr("unicom_file_ctnr", "");
	aos_assert_r(mUuicomFilePctrs != "", false)

	mDocumentCtnr = def->getAttrStr("zky_document_ctnr", "");
	aos_assert_r(mDocumentCtnr != "", false);

	AosXmlTagPtr attrs = def->getFirstChild("attrs");
	if (!attrs) return true;

	AosXmlTagPtr attr = attrs->getFirstChild(true); 
	while (attr)
	{
		AosValueRslt value;
		AosXmlTagPtr key_doc = attr->getFirstChild("xpath");
		AosXmlTagPtr value_doc = attr->getFirstChild("value");

		rslt = AosValueSel::getValueStatic(value, key_doc, rdata);
		if (!rslt || value.getTypeStr() != AOSDATATYPE_STRING)
		{
			rdata->setError() << "Failed to get the cash attribute name!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;                                                                   
		}
		OmnString key = value.getStr();
		if ( key == "")
		{
			rdata->setError() << "cash attribute name is null!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		rslt = AosValueSel::getValueStatic(value, value_doc, rdata);
		if (!rslt || value.getTypeStr() != AOSDATATYPE_STRING)
		{
			rdata->setError() << "Failed to get the cash attribute name!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;                                                                   
		}
		OmnString strValue = value.getStr();
		if ( strValue == "")
		{
			rdata->setError() << "cash attribute name is null!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		mAttrMap[key] = strValue;
		attr = attrs->getNextChild();
	}
	 
	for (int i=0; i<eMaxThrdIds; i++)
	{
		mWordNorm[i] = OmnNew AosWordNorm("", "wordNorm");
		mWordParser[i] = OmnNew AosWordParser();
	}
	
	return true;
}

bool
AosDataProcUnicomDocument::getOutPuts(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnString attr_name, attr_path, opr, iiltype, output_name_field, docid_field_name;
	
	//output_doc
	AosXmlTagPtr output_doc = def->getFirstChild("output_doc");
	aos_assert_r(output_doc, false);

	output_name_field = output_doc->getAttrStr("zky_output_field_name", "");
	aos_assert_r(output_name_field != "", false);
	mOutputDoc.init(output_name_field, false);

	//output_hit
	AosXmlTagPtr output_hit = def->getFirstChild("output_hit");
	aos_assert_r(output_hit, false);

	output_name_field = output_hit->getAttrStr("zky_output_field_name", "");
	aos_assert_r(output_name_field != "", false);

	docid_field_name = output_hit->getAttrStr("zky_output_docid_name", "");

	mOutputHitInfo.mOutput.init(output_name_field, false);
	mOutputHitInfo.mDocidFieldName = docid_field_name;

	//output_unknow_attrs
	AosXmlTagPtr output_attr = def->getFirstChild("output_attr");
	aos_assert_r(output_attr, false);
	
	output_name_field = output_attr->getAttrStr("zky_output_field_name", "");
	aos_assert_r(output_name_field != "", false);

	docid_field_name = output_attr->getAttrStr("zky_output_docid_name", "");
	OmnString value_field_name = output_attr->getAttrStr("zky_output_value_name", "");
	
	mOutputAttrInfo.mOutput.init(output_name_field, false);
	mOutputAttrInfo.mDocidFieldName = docid_field_name;
	mOutputAttrInfo.mValueFieldName = value_field_name;

	//output_attrs
	AosXmlTagPtr output_keys = def->getFirstChild("output_keys");
	aos_assert_r(output_keys, false);

	AosXmlTagPtr tag = output_keys->getFirstChild("field");
	while(tag)
	{
		attr_name = tag->getAttrStr("zky_attrname", "");
		aos_assert_r(attr_name != "", false);

		attr_path = tag->getAttrStr("zky_attrpath", "");
		aos_assert_r(attr_path != "", false);
	
		output_name_field = tag->getAttrStr("zky_output_field_name", "");
		aos_assert_r(output_name_field != "", false);

		opr = tag->getAttrStr("zky_opr", "");
		iiltype = tag->getAttrStr("zky_iiltype", "");

		docid_field_name = tag->getAttrStr("zky_output_docid_name", "");

		OutputInfo output_info;
		output_info.mAttrName = attr_name;
		output_info.mAttrPath = attr_path;
		output_info.mOpr = opr;
		output_info.mIILType = iiltype;
		output_info.mOutput.init(output_name_field, false);
		output_info.mDocidFieldName = docid_field_name;
		mOutputKeys.push_back(output_info);
		tag = output_keys->getNextChild();
	}

	return true;
}

bool
AosDataProcUnicomDocument::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	rf_infos.push_back(&(mInput));
	rf_infos.push_back(&(mOutputDoc));
	rf_infos.push_back(&(mOutputHitInfo.mOutput));
	rf_infos.push_back(&(mOutputAttrInfo.mOutput));
	for(u32 i=0; i<mOutputKeys.size(); i++)
	{
		rf_infos.push_back(&(mOutputKeys[i].mOutput));
	}
	return true;
}

AosDataProcStatus::E
AosDataProcUnicomDocument::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	mDocNum = 0;
OmnScreen << " ###### DataProcUnicomDocument start procData " << endl;
	AosDataRecordObj * from_record = getRecord(mInput, input_records, output_records);
	aos_assert_r(from_record, AosDataProcStatus::eError);	
	
	AosDataRecordObj * doc_record = getRecord(mOutputDoc, input_records, output_records);
	aos_assert_r(doc_record, AosDataProcStatus::eError);	

	AosValueRslt input_rslt;
	bool rslt = from_record->getFieldValue(                                  
			    mInput.mFieldIdx, input_rslt, true, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	int len = 0;
	char *data = const_cast <char *>(input_rslt.getCharStr(len));
	aos_assert_r(len, AosDataProcStatus::eError);

	OmnScreen << "VVVVVVVVV len:" << len << ";"<< endl;
	AosBuffPtr buff = OmnNew AosBuff(data, len, len AosMemoryCheckerArgs);
	
	int numbuffs = buff->getInt(0);

	u32 buff_len = buff->getU32(0);
	AosBuffPtr unicomtitle_buff = buff->getBuff(buff_len, false AosMemoryCheckerArgs); 
	aos_assert_r(unicomtitle_buff, AosDataProcStatus::eError);

	AosXmlTagPtr unicomtitle_doc = AosXmlParser::parse(unicomtitle_buff->data(), buff_len AosMemoryCheckerArgs);
	aos_assert_r(unicomtitle_doc, AosDataProcStatus::eError);

	bool zip_error = unicomtitle_doc->getAttrBool("zip_error", false);
	if (zip_error)
	{
OmnScreen << " ###### zip_error return " << endl; 
		return AosDataProcStatus::eContinue;
	}

	buff_len = buff->getU32(0);
	AosBuffPtr zip_buff = buff->getBuff(buff_len, true AosMemoryCheckerArgs);
	aos_assert_r(zip_buff, AosDataProcStatus::eError);


	OmnString fname = unicomtitle_doc->getAttrStr("unicom_file_name", "");
	aos_assert_r(fname != "", AosDataProcStatus::eError);

	OmnString isNoErr = unicomtitle_doc->getAttrStr("zky_unicom_file_error", "000"); 

	if (isNoErr != "000")   /// ???? 如何生成LOG
	{
		OmnAlarm << " Document is Error : " << isNoErr << enderr;
		return AosDataProcStatus::eContinue;
	}

	unicomtitle_doc->setAttr(AOSTAG_HPCONTAINER, mUuicomTitlePctrs);
	unicomtitle_doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
	unicomtitle_doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");             
	unicomtitle_doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
	
	OmnString unicomtitle_objid = "";
	u64 unicomtitle_docid = getNextDocid(unicomtitle_objid, rdata_raw);
	aos_assert_r(unicomtitle_docid > 0, AosDataProcStatus::eError);
	aos_assert_r(unicomtitle_objid != "", AosDataProcStatus::eError);

	unicomtitle_doc->setAttr(AOSTAG_OBJID, unicomtitle_objid);
	unicomtitle_doc->setAttr(AOSTAG_DOCID, unicomtitle_docid);
	unicomtitle_doc->setTextBinary("", zip_buff);
	//unicomtitle_doc->setAttr(AOSTAG_BINARY_SOURCELEN, zip_buff->dataLen());

	u32 siteid = rdata_raw->getSiteid();        
	aos_assert_r(siteid != 0, AosDataProcStatus::eError);
	
	unicomtitle_doc->setAttr(AOSTAG_SITEID, siteid);

OmnScreen << " ###### createDocPriv unicomtitle filename : " << fname << endl;
	rslt = createDocPriv(unicomtitle_doc, input_records, output_records, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;

	AosValueRslt output_rslt;
	OmnString docstr = unicomtitle_doc->toString();
	output_rslt.setCharStr1(docstr.data(), docstr.length(), false);
	bool outofmem = false;
	rslt = doc_record->setFieldValue(mOutputDoc.mFieldIdx, output_rslt, outofmem, rdata_raw);
    if (!rslt) return AosDataProcStatus::eError;
	doc_record->setDocidByIdx(mDocNum, unicomtitle_docid);
	mDocNum++;
	doc_record->flush();

	OmnString objid = "";
	u64 docid = 0;
	AosBuffPtr tmp_buff;
	AosXmlTagPtr tmp_doc;
	for(int i=0; i<numbuffs; i++)
	{
		buff_len = buff->getU32(0);
		tmp_buff = buff->getBuff(buff_len, false AosMemoryCheckerArgs);
		aos_assert_r(tmp_buff, AosDataProcStatus::eError);
		
		tmp_doc = AosXmlParser::parse(tmp_buff->data(), buff_len AosMemoryCheckerArgs);
		aos_assert_r(tmp_doc, AosDataProcStatus::eError);

		buff_len = buff->getU32(0);
		tmp_buff = buff->getBuff(buff_len, true AosMemoryCheckerArgs);
		aos_assert_r(tmp_buff, AosDataProcStatus::eError);
		
		map<OmnString, OmnString>::iterator it;
		for(it = mAttrMap.begin(); it != mAttrMap.end(); it++)
		{
			tmp_doc->setAttr(it->first, it->second);
		}
		
		docid = getNextDocid(objid, rdata_raw);
		aos_assert_r(docid > 0, AosDataProcStatus::eError);
		aos_assert_r(objid != "", AosDataProcStatus::eError);
	
		tmp_doc->setAttr(AOSTAG_OBJID, objid);
		tmp_doc->setAttr(AOSTAG_DOCID, docid);
		tmp_doc->setAttr("unicom_title_objid", unicomtitle_objid);
		tmp_doc->setAttr("unicom_title_docid", unicomtitle_docid);
		tmp_doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
		tmp_doc->setAttr(AOSTAG_HPCONTAINER, mUuicomFilePctrs);
		tmp_doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
		tmp_doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");             
		tmp_doc->setAttr(AOSTAG_JOB_DOCID, mTask->getJobDocid());
		//tmp_doc->setAttr(AOSTAG_BINARY_SOURCELEN, tmp_buff->dataLen());
	
		tmp_doc->setAttr(AOSTAG_SITEID, siteid);
		
		addAttrForUpload(tmp_doc, rdata_raw);
		tmp_doc->setTextBinary("", tmp_buff);
		
		rslt = createDocPriv(tmp_doc, input_records, output_records, rdata_raw);
		if (!rslt) return AosDataProcStatus::eError;
	
		docstr = tmp_doc->toString();
		output_rslt.setCharStr1(docstr.data(), docstr.length(), false);
		outofmem = false;
		rslt = doc_record->setFieldValue(mOutputDoc.mFieldIdx, output_rslt, outofmem, rdata_raw);
		if (!rslt) return AosDataProcStatus::eError;
		doc_record->setDocidByIdx(mDocNum, docid);
		mDocNum++;
		doc_record->flush();
		
		rslt = importDoc(tmp_doc, tmp_buff, input_records, output_records, rdata_raw);
		if (!rslt)
		{
			OmnAlarm << " importDoc : " << objid << " error ! " << enderr;
		}

	}

	return AosDataProcStatus::eContinue;
}


u64
AosDataProcUnicomDocument::getNextDocid(
		OmnString &objid,
		const AosRundataPtr &rdata)
{
	u64 docid = AosDocidMgrObj::getDocidMgr()->nextDocid(objid, rdata);
	return docid;
}

bool
AosDataProcUnicomDocument::addAttrForUpload(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	OmnString file_type = doc->getAttrStr("zky_file_type");
	if (file_type == "上传")
	{
		OmnString fname = doc->getAttrStr(AOSTAG_SOURCE_FNAME);
		aos_assert_r(fname != "", false);

		doc->setAttr(AOSTAG_RSC_FNAME, fname);		
		doc->setAttr(AOSTAG_RSCDIR, "/tmp");
		
		OmnString document_type = doc->getAttrStr(AOSTAG_FILE_FORMAT);
		document_type = document_type.toLower();
		if (document_type == "doc" || document_type == "docx" || document_type == "wps" || document_type == "wpt")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE,"DOC");
		}
		else if (document_type == "pdf")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "PDF");
		}
		else if (document_type == "xls" || document_type == "xlsx" || document_type == "et")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "XLS");
		}
		else if (document_type == "ppt" || document_type == "pptx" || document_type == "dps")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "PPT");
		}
		else if (document_type == "txt")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "TXT");
		}
		else if (document_type == "xml" || document_type == "xps")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "XML");
		}
		else if (document_type == "jpg" || document_type == "jpeg" || 
				 document_type == "bmp" || document_type == "gif" ||
				 document_type == "tif" || document_type == "png")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "IMAGE");
		}
		else if (document_type == "aip" || document_type == "rar" || document_type == "zip")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "RAR");
		}
		else
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "OTHER");
		}

		OmnString cid = rdata->getCid();
		aos_assert_r(cid != "", false);

		AosXmlTagPtr user_doc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
		aos_assert_r(user_doc, false);

		OmnString user_name = user_doc->getAttrStr(AOSTAG_USERNAME, "");
		aos_assert_r(user_name != "", false);

		doc->setAttr(AOSTAG_USERNAME, user_name);
	}

	return true;
}

bool
AosDataProcUnicomDocument::importDoc(
		const AosXmlTagPtr &binary_doc,
		const AosBuffPtr &buff,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records,
		const AosRundataPtr &rdata)
{
	int64_t fileidx = mFileIdx;
	mFileIdx++;
	
	OmnString dir = AosGetBaseDirname();
	OmnString source_fname = dir;
	source_fname << "source_" << mTask->getTaskDocid() << "_" << fileidx << ".txt";


	OmnString sfname = binary_doc->getAttrStr(AOSTAG_SOURCE_FNAME);
	aos_assert_r(sfname != "", false);

	AosStrSplit split(sfname, ".");            
	vector<OmnString> strs= split.entriesV();

	OmnString document_type = strs[strs.size()-1];
	aos_assert_r(document_type != "", false);

	OmnFilePtr file = OmnNew OmnFile(source_fname, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);

	file->put(0, buff->data(), buff->dataLen(), true);

	OmnString fname = convertWordPdf(dir, source_fname, document_type, fileidx);
	if (fname == "")
	{
		file->deleteFile();
		return true;
	}

	map<OmnString, OmnString> has_values;
	map<OmnString, vector<OmnString> > split_values;
	AosXmlTagPtr predoc = getPreDoc(binary_doc, sfname, has_values, split_values, rdata);	
	aos_assert_r(predoc, false);

	proc(predoc, has_values, split_values, fname, input_records, output_records, rdata);	

	if (document_type != "txt" || document_type != "TXT")
	{
		file->deleteFile();
	}
	return true;
}

OmnString
AosDataProcUnicomDocument::convertWordPdf(
		const OmnString &dir,
		const OmnString &source_fname,
		const OmnString &document_type,
		const int64_t fileidx)
{
	OmnString target_fname = dir;
	target_fname << "target_" << mTask->getTaskDocid() << "_" << fileidx << ".txt";

	if (document_type == "doc" || document_type == "DOC")
	{
		OmnString cmd = "antiword ";
		cmd << "\"" << source_fname << "\" > \"" << target_fname << "\"";
		system(cmd.data());
	}
	else if (document_type == "pdf" || document_type == "PDF")
	{
		OmnString cmd = "pdftotext -layout -q ";
		cmd << "\"" << source_fname << "\" \"" << target_fname << "\"";
		system(cmd.data());
	}
	else if (document_type == "txt" || document_type == "TXT")
	{
		return source_fname;
	}
	else
	{
		OmnScreen << "~~~~~~~~ invalid document type " << document_type << endl;
		return "";
	}
	return target_fname;
}


AosXmlTagPtr
AosDataProcUnicomDocument::getPreDoc(
		const AosXmlTagPtr &doc,
		const OmnString &sfname,
		map<OmnString, OmnString> &has_values,
		map<OmnString, vector<OmnString> > &split_values,
		const AosRundataPtr &rdata)
{
	AosStrSplit split(sfname, "/");            
	vector<OmnString> strs= split.entriesV();
	OmnString fname = strs[strs.size()-1];

	OmnString file_objid = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(file_objid != "", 0);
	
	OmnString document_type = doc->getAttrStr(AOSTAG_ZKY_TYPE, "OTHER");

	OmnString title_objid = doc->getAttrStr("unicom_title_objid", "");
	OmnString title_docid = doc->getAttrStr("unicom_title_docid", "");

	OmnString ctime = OmnGetTime(AosLocale::eUSA);
	OmnString file_ctime = doc->getAttrStr(AOSTAG_CTIME, ctime);

	OmnString predocstr = "";
	u32 systemsec = OmnGetSecond();                   
	predocstr << "<doc "
		<< AOSTAG_PUBLIC_DOC << "=\"true\" "
		<< AOSTAG_CT_EPOCH << "=\"" << systemsec << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSTAG_DOC_SECTION << "\" "
		<< AOSTAG_PARENTC << "=\"" << mDocumentCtnr << "\" "
		<< AOSTAG_CTIME << "=\"" << ctime << "\" "
		<< "zky_fileobjid=\"" << file_objid << "\" "
		<< "unicom_title_objid=\"" << title_objid << "\" "
		<< "unicom_title_docid=\"" << title_docid << "\" "
		<< AOSTAG_ZKY_TYPE << "=\"" << document_type << "\" "
		<< "zky_file_ctime=\"" << file_ctime << "\" "
		<< "zky_rscfn=\"" << fname << "\" ";

	OmnString cid = rdata->getCid();
	aos_assert_r(cid != "", 0);

	AosXmlTagPtr user_doc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
	aos_assert_r(user_doc, 0);

	OmnString user_name = user_doc->getAttrStr(AOSTAG_USERNAME, "");
	aos_assert_r(user_name != "", 0);

	predocstr << AOSTAG_USERNAME << "=\"" << user_name << "\" "
			  << AOSTAG_CTNR_PUBLIC << "=\"true\" />";

//??????? attrname is the same, but the path is different, what can i do?

	AosXmlParser xmlparser;
	AosXmlTagPtr predoc = xmlparser.parse(predocstr, "" AosMemoryCheckerArgs);
	aos_assert_r(predoc, 0);

	OmnString attrname, attrpath;
	for(u32 i=0; i<mOutputKeys.size(); i++)
	{
		attrname = mOutputKeys[i].mAttrName;
		attrpath = mOutputKeys[i].mAttrPath;
		OmnString value = doc->xpathQuery(attrpath);;
	
		if(attrname == AOSTAG_OTYPE)
		{
			value = AOSTAG_DOC_SECTION;
		}
		else if(attrname == AOSTAG_CT_EPOCH)
		{
			value = "";
			value << systemsec;
		}
		else if(attrname == "zky_fileobjid")
		{
			value = file_objid;
		}
		else if(attrname == AOSTAG_PARENTC)
		{
			value = mDocumentCtnr;
		}
		
		if (value != "")
		{
			if(mOutputKeys[i].mIILType == "hit")
			{
				vector<OmnString> strs;
				if (mOutputKeys[i].mOpr != "")
				{
					AosStrSplit split(value, mOutputKeys[i].mOpr.data());            
					strs= split.entriesV();
				}
				else
				{
					strs.push_back(value);
				}
				split_values.insert(make_pair(attrname, strs));
			}
			else
			{
				has_values.insert(make_pair(attrname, value));
			}
			predoc->xpathSetAttr(attrpath, value, true);
		}
	}
	
	return predoc;
}


bool
AosDataProcUnicomDocument::proc(
		const AosXmlTagPtr &predoc,
		map<OmnString, OmnString> &has_values,
		map<OmnString, vector<OmnString> > &split_values,
		const OmnString &fname, 
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records,
		const AosRundataPtr &rdata)
{
	OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(ff && ff->isGood(), false);

	OmnString filestr, nextstr;
	int partnum = 1;
	bool finished = false;
	while(!(finished = AosSdocDocument::CreateFileDoc::getFileInfo(filestr, nextstr, ff)))
	{
		if (filestr == "") continue;
	
		bool rslt = asmAppend(filestr, partnum++, predoc, has_values, split_values, input_records, output_records, rdata);
		aos_assert_r(rslt, false);

		filestr = nextstr;
		nextstr = "";
	}
	
	if (filestr != "")
	{
		bool rslt = asmAppend(filestr, partnum++, predoc, has_values, split_values, input_records, output_records, rdata);
		aos_assert_r(rslt, false);
	}

	ff->deleteFile();
	return true;
}


bool 
AosDataProcUnicomDocument::asmAppend(
		const OmnString &filestr,
		const int &partnum,
		const AosXmlTagPtr &predoc,
		map<OmnString, OmnString> &has_values,
		map<OmnString, vector<OmnString> > &split_values,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records,
		const AosRundataPtr &rdata)
{
	OmnString objid;
	u64 docid = getNextDocid(objid, rdata);
	aos_assert_r(docid, false);

	OmnString docstr = composeEntry(docid, filestr, partnum, predoc);
	aos_assert_r(docstr != "", false);

	// doc_record set field value
	AosDataRecordObj * doc_record = getRecord(mOutputDoc, input_records, output_records);
	aos_assert_r(doc_record, false);	
	
	AosValueRslt output_rslt;
	output_rslt.setCharStr1(docstr.data(), docstr.length(), false);
	bool outofmem = false;
	bool rslt = doc_record->setFieldValue(mOutputDoc.mFieldIdx, output_rslt, outofmem, rdata.getPtrNoLock());
    if (!rslt) return false;
	doc_record->setDocidByIdx(mDocNum, docid);
	mDocNum++;
	doc_record->flush();

	AosDataRecordObj * hit_record = getRecord(mOutputHitInfo.mOutput, input_records, output_records);
	aos_assert_r(hit_record, false);	

	u64 title_docid = predoc->getAttrU64("unicom_title_docid", 0);
	aos_assert_r(title_docid > 0, false);
	
	iilAppendEntry(docid, title_docid, has_values, split_values, hit_record, input_records, output_records, rdata);

	AosWordParser wordParser;
	wordParser.setSrc(filestr);
	OmnString word;

	while (wordParser.nextWord(word))
	{
		if (word == "\n")
		{
			continue;
		}
		/*
		if (word.length() >= (mRcdLen - (int)sizeof(u64)))
		{
			word.setLength(mRcdLen - (int)sizeof(u64) - 2 );
		}
		*/
		
		splitBytes(word, docid, title_docid, hit_record, rdata);
		
		rslt = appendHitRecord(hit_record, word, docid, title_docid, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosDataProcUnicomDocument::appendHitRecord(
		AosDataRecordObj * hit_record,
		const OmnString &word,
		const u64 docid,
		const u64 title_docid,
		const AosRundataPtr &rdata)
{
	// set word
	AosValueRslt output_rslt;
	output_rslt.setStr(word);
	bool outofmem = false;
	bool rslt = hit_record->setFieldValue(mOutputHitInfo.mOutput.mFieldIdx, output_rslt, outofmem, rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	// set docid
	int field_idx = hit_record->getFieldIdx(mOutputHitInfo.mDocidFieldName, rdata.getPtrNoLock());
	aos_assert_r(field_idx != -1, false);
	output_rslt.setU64(docid);
	outofmem = false;
	rslt = hit_record->setFieldValue(field_idx, output_rslt, outofmem, rdata.getPtrNoLock());
	aos_assert_r(rslt, false);
	hit_record->flush();

	if (title_docid == 0) return true;

	// set title word
	outofmem = false;
	output_rslt.setStr(word);
	rslt = hit_record->setFieldValue(mOutputHitInfo.mOutput.mFieldIdx, output_rslt, outofmem, rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	// set title docid
	outofmem = false;
	output_rslt.setU64(title_docid);
	rslt = hit_record->setFieldValue(field_idx, output_rslt, outofmem, rdata.getPtrNoLock());
	aos_assert_r(rslt, false);
	hit_record->flush();
	return true;
}

void
AosDataProcUnicomDocument::splitBytes(
		const OmnString &word,
		const u64 &docid,
		const u64 &title_docid,
		AosDataRecordObj * hit_record,
		const AosRundataPtr &rdata)
{
	const char * a  = word.data();
	//static unsigned char arr[] = {0b10000000, 0b11100000, 0b11110000, 0b11111000};
	static u8 arr[] = {0b10000000, 0b11100000, 0b11110000, 0b11111000};
	static u8 brr[] = {0b00000000, 0b11000000, 0b11100000, 0b11110000};
	int total = 0;
	//OmnScreen << " ###########  hitword : " << word << endl;
	bool rslt;
	for(int i=0; i<word.length(); )
	{
		int size = 0;
		if ((*a & arr[0]) == brr[0])
		{
			size = 1;
		}
		else if ((*a & arr[1]) == brr[1])
		{
			size = 2;
		}
		else if ((*a & arr[2]) == brr[2])
		{
			size = 3;
		}
		else if((*a & arr[3]) == brr[3])
		{
			size = 4;
		}
		else
		{
			OmnScreen << " Alarm : invalid byte  " << endl;
			return;
		}
	
		total += size;
		if (total > word.length())
		{
			OmnScreen << " Alarm : out of the boundary " << endl;
			return;
		}

		OmnString ss(a, size);
		//OmnScreen << " ###########  hit : " << ss << "  docid : " << docid << endl;

		/*
		AosValueRslt valueRslt1;
		valueRslt1.setCharStr1(a, size, true);
		valueRslt1.setDocid(docid);
		
		mHitIILAsm->appendEntry(valueRslt1, rdata);

		valueRslt1.setDocid(title_docid);
		mHitIILAsm->appendEntry(valueRslt1, rdata);
		*/
		rslt = appendHitRecord(hit_record, ss, docid, title_docid, rdata);
		aos_assert(rslt);
		a += size;
		i += size;
	}
}


void AosDataProcUnicomDocument::onThreadInit(const AosRundataPtr &, void **data)
{
	mLock->lock();
	long tid = mThreadId;
	mThreadId++;
	mLock->unlock();
	OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread();
	thread->setUdata((void*)tid);
}

void AosDataProcUnicomDocument::onThreadExit(const AosRundataPtr &, void **data)
{
	OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread();
	thread->setUdata((void*)0);
}

bool
AosDataProcUnicomDocument::addAttrWordIIL(
		const OmnString &attrname,
		const vector<OmnString> &values,
		const u64 &docid,
		const u64 &title_docid,
		AosDataRecordObj *hit_record,
		const AosRundataPtr &rdata)
{
	vector<OmnString> attrwords;
    
	OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread();
	void* udata = thread->getUdata();
	int logicId = (long)udata;
	aos_assert_r(logicId < eMaxThrdIds, false);

	for(u32 i=0; i<values.size(); i++)
	{
		if (values[i] == "") continue;
		mWords[logicId].reset();
		bool rslt = AosDocProcUtil::collectWords(mWords[logicId], mWordParser[logicId], mWordNorm[logicId], values[i]);
		aos_assert_r(rslt, false);

		OmnString word;
		AosEntryMark::E mark;
		while(mWords[logicId].nextEntry(word, mark))
		{
			OmnString iilname = AosIILName::composeAttrWordIILName(attrname, word);
			if (iilname == "") continue;

			if (iilname.length() > eMaxStrLen)
			{
				OmnAlarm << "iilname is too long " << iilname << enderr;
				continue;
			}

			/*
			AosValueRslt valueRslt1;
			valueRslt1.setCharStr1(iilname.data(), iilname.length(), true);
			valueRslt1.setDocid(docid);
			//OmnScreen << "@@@@@@  hitiilasm key : " << iilname << " docid : " << docid << endl;
			mHitIILAsm->appendEntry(valueRslt1, rdata);

			valueRslt1.setDocid(title_docid);
			mHitIILAsm->appendEntry(valueRslt1, rdata);
			*/

			rslt = appendHitRecord(hit_record, iilname, docid, title_docid, rdata);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}

bool
AosDataProcUnicomDocument::iilAppendEntry(
		const u64 docid,
		const u64 title_docid,
		map<OmnString, OmnString> &has_values,
		map<OmnString, vector<OmnString> > &split_values,
		AosDataRecordObj * hit_record,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records,
		const AosRundataPtr &rdata)
{
	map<OmnString, OmnString>::iterator itr;
	map<OmnString, vector<OmnString> >::iterator split_itr;
	OmnString attrname, opr, iiltype;
	AosDataRecordObj *record;
	AosValueRslt output_rslt;
	int field_idx;
	for(u32 i=0; i<mOutputKeys.size(); i++)
	{
		attrname = mOutputKeys[i].mAttrName;
		iiltype = mOutputKeys[i].mIILType;
		if (iiltype == "hit")
		{
			split_itr = split_values.find(attrname);
			aos_assert_r(split_itr != split_values.end(), false);
			vector<OmnString> strs = split_itr->second;
			addAttrWordIIL(attrname, strs, docid, title_docid, hit_record, rdata);
		}
		else
		{
			itr = has_values.find(attrname);
			aos_assert_r(itr != has_values.end(), false);
			output_rslt.setStr(itr->second);
			
			record = getRecord(mOutputKeys[i].mOutput, input_records, output_records);
			aos_assert_r(record, false);	

			bool outofmem = false;
			bool rslt = record->setFieldValue(mOutputKeys[i].mOutput.mFieldIdx, output_rslt, outofmem, rdata.getPtrNoLock());
			if (!rslt) return false;

			output_rslt.setU64(docid);
			field_idx = record->getFieldIdx(mOutputKeys[i].mDocidFieldName, rdata.getPtrNoLock());
			aos_assert_r(field_idx != -1, false);
			outofmem = false;
			rslt = record->setFieldValue(field_idx, output_rslt, outofmem, rdata.getPtrNoLock());
			if (!rslt) return false;
			record->flush();
		}
	}
	return true;
}

OmnString
AosDataProcUnicomDocument::composeEntry(
		const u64 &docid,
		const OmnString &filestr, 
		const int partnum,
		const AosXmlTagPtr &predoc)
{
	AosXmlTagPtr doc = predoc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(doc, "");

	doc->setAttr("zky_partnum", partnum);
	doc->setAttr(AOSTAG_DOCID, docid);

	doc->setNodeText("zky_context", filestr, true);

	OmnString docstr = doc->toString();
	return docstr;
}

bool
AosDataProcUnicomDocument::createDocPriv(
		const AosXmlTagPtr &doc,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc, false);
	doc->setReadOnly(true);
	
	AosDataRecordObj * hit_record = getRecord(mOutputHitInfo.mOutput, input_records, output_records);
	aos_assert_r(hit_record, false);	
	
	AosDataRecordObj * attr_record = getRecord(mOutputAttrInfo.mOutput, input_records, output_records);
	aos_assert_r(attr_record, false);	
	
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	OmnString siteid = doc->getAttrStr(AOSTAG_SITEID);
	aos_assert_r(siteid != "", false);
	AosXmlTagPtr parent_doc;

	OmnString parent_objid = doc->getAttrStr(AOSTAG_PARENTC);
	if (parent_objid != "")
	{
		parent_doc = AosDocClientObj::getDocClient()->getDocByObjid(parent_objid, rdata);
		if (parent_doc) parent_doc->setRCObjInUse(eAosRCObjClassId_XmlTag);
	}

	// Collect words form xml , it has words in tag name ,attrbute name, 
	// attribute value and node text.
	// 1. Add all the words in mWordHash[tid].
	AosEntryMark::E mark, marknull;

	OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread();
	void* udata = thread->getUdata();
	int logicId = (long)udata;
	aos_assert_r(logicId < eMaxThrdIds, false);

	mWords[logicId].reset();
	bool rslt = AosDocProcUtil::collectWords(mWords[logicId], mWordParser[logicId], mWordNorm[logicId], parent_doc, doc, false);
	if (!rslt)
	{
		OmnAlarm << "Failed collecting words!" << enderr;
		if (parent_doc) parent_doc->setRCObjInUse(false);
		return false;
	}

	OmnString word;
	int guard = eMaxWordsPerDoc;
	while (guard-- && mWords[logicId].nextEntry(word, mark))
	{
		//????????
		//addWordIIL(allTrans, num_phs, word, docid, parent_objid, mark, rdata, __LINE__);
		rslt = appendHitRecord(hit_record, word, docid, 0, rdata);
		aos_assert_r(rslt, false);
	}
	if (guard <= 0)
	{
		OmnAlarm << "Serious problem: " << eMaxWordsPerDoc << enderr;
		if (parent_doc) parent_doc->setRCObjInUse(false);
		return false;
	}

	// 2. Add attributes: "name=value"
	mAttrs[logicId].reset();
	rslt = AosDocProcUtil::collectAttrs(mAttrs[logicId], parent_doc, doc, true);
	if (!rslt)
	{
		OmnAlarm << "Failed collecting attributes: " << doc->toString() << enderr;
		if (parent_doc) parent_doc->setRCObjInUse(false);
		return false;
	}
	OmnString name, value;
	guard = eMaxWordsPerDoc;
	mAttrs[logicId].resetWordLoop();
	while (guard-- && mAttrs[logicId].nextEntry(name, value, mark))
	{
		if (value.length() <= 0)
		{
			OmnAlarm << "Value is empty: " << name << enderr;
		}
		else
		{
	    	//???????????
			//addAttrIIL(allTrans, num_phs, name, value, docid, parent_objid, mark, rdata);
			addAttrIIL(name, value, docid, parent_objid, mark, attr_record, rdata);
		}

		if (mark & AosEntryMark::eAttrWordIndex)
		{
			mWords[logicId].reset();
			rslt = AosDocProcUtil::collectWords(mWords[logicId], mWordParser[logicId], mWordNorm[logicId], value);
			aos_assert_r(rslt, false);
			while(mWords[logicId].nextEntry(word, marknull))
			{
				//???????
				//addAttrWordIIL(allTrans, num_phs, name, word, docid, parent_objid, mark, rdata);
				rslt = appendHitRecord(hit_record, word, docid, 0, rdata);
				aos_assert_r(rslt, false);
			}
		}
	}
	if (guard <= 0)
	{
		OmnAlarm << "Too many attributes: " << eMaxWordsPerDoc 
			<< ". Doc: " << doc->toString() << enderr;
		if (parent_doc) parent_doc->setRCObjInUse(false);
		return false;
	}

	/*??????????//
	// 3 Add system infomation to iil
	bool isCtnr = doc->getAttrStr(AOSTAG_OTYPE,"") == AOSOTYPE_CONTAINER;
	u64 epoch = doc->getAttrU64(AOSTAG_CT_EPOCH, 0);
	if (epoch == 0)
	{
		// The doc does not have epoch time. If it is not repairing, 
		// need to raise alarm.
		if (!mIsRepairing)
		{
			OmnAlarm << "Doc missging creation Epoch time: " 
				<< doc->toString() << enderr;
		}
		epoch = OmnGetSecond();
		doc->setReadOnly(false);
		doc->setAttr(AOSTAG_CT_EPOCH, epoch);
		doc->setReadOnly(true);
	}
	addSysInfoIIL(allTrans, num_phs, docid, objid, parent_docid, parent_objid, epoch, rdata, isCtnr);

	// 4 add meta data
	AosDocProcUtil::addMetaAttrs(allTrans, num_phs, doc, docid, rdata);
	*/
	return true;
}

bool 
AosDataProcUnicomDocument::addAttrIIL(
		const OmnString &name, 
		const OmnString &value, 
		const u64 docid, 
		const OmnString &ctnr_objid, 
		const AosEntryMark::E mark,
		AosDataRecordObj *attr_record,
		const AosRundataPtr &rdata)
{
	bool rslt;
	OmnString iilname = "";
	AosEntryMark::E mark1 = (AosEntryMark::E)(mark & ~AosEntryMark::eAttrWordIndex);
	switch(mark1)
	{
	case AosEntryMark::eBothIndex:

	case AosEntryMark::eContainerIndex:
		 iilname = AosIILName::composeContainerAttrIILName(ctnr_objid, name);
		 //rslt = AosDocProcUtil::addValueToIIL(allTrans, arr_len, iilname, value, docid, false, true, rdata);

	case AosEntryMark::eGlobalIndex:
		 iilname = AosIILName::composeAttrIILName(name);
		 //rslt = AosDocProcUtil::addValueToIIL(allTrans, arr_len, iilname, value, docid, false, true, rdata);
		 break;

	case AosEntryMark::eCtnrOnly:
		 iilname = AosIILName::composeContainerAttrIILName(ctnr_objid, name);
		 //rslt = AosDocProcUtil::addValueToIIL(allTrans, arr_len, iilname, value, docid, false, true, rdata);
		 break;

	case AosEntryMark::eNoIndex:
		 break;

	default:
		 break;
	}

	if (iilname == "") return true;

	AosValueRslt output_rslt;
	output_rslt.setStr(iilname);
	bool outofmem = false;
	rslt = attr_record->setFieldValue(mOutputAttrInfo.mOutput.mFieldIdx, output_rslt, outofmem, rdata.getPtrNoLock()); 
	aos_assert_r(rslt, false);

	aos_assert_r(mOutputAttrInfo.mValueFieldName != "", false);
	output_rslt.setStr(value);
	int field_idx = attr_record->getFieldIdx(mOutputAttrInfo.mValueFieldName, rdata.getPtrNoLock());
	aos_assert_r(field_idx != -1, false);
	outofmem = false;
	rslt = attr_record->setFieldValue(field_idx, output_rslt, outofmem, rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	aos_assert_r(mOutputAttrInfo.mDocidFieldName != "", false);
	output_rslt.setU64(docid);
	field_idx = attr_record->getFieldIdx(mOutputAttrInfo.mDocidFieldName, rdata.getPtrNoLock());
	aos_assert_r(field_idx != -1, false);
	outofmem = false;
	rslt = attr_record->setFieldValue(field_idx, output_rslt, outofmem, rdata.getPtrNoLock());
	return rslt;
}



AosDataProcStatus::E
AosDataProcUnicomDocument::procData(
		const AosDataRecordObjPtr &record,			
		const u64 &docid,
		const AosDataRecordObjPtr &output,			
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return AosDataProcStatus::eError;
}


AosDataProcObjPtr
AosDataProcUnicomDocument::cloneProc()
{
	return OmnNew AosDataProcUnicomDocument(*this);
}


AosDataProcObjPtr
AosDataProcUnicomDocument::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcUnicomDocument * proc = OmnNew AosDataProcUnicomDocument(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}

bool
AosDataProcUnicomDocument::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return true;
}

#endif
