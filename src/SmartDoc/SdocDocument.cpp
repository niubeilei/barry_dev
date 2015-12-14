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
// 03/19/2013	Created by Ice
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocDocument.h"

#include "DocSelector/DocSelector.h"
#include "SEInterfaces/DocClientObj.h"
#include "SmartDoc/SMDMgr.h"
#include "Util/File.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "API/AosApi.h"
#include <boost/regex.hpp>
#if 0

OmnMutexPtr AosSdocDocument::smLock = OmnNew OmnMutex();                       
queue<AosSdocDocument::Entry> AosSdocDocument::smTasks;
int AosSdocDocument::smThrds = AosSdocDocument::eDftThrdShells;
int AosSdocDocument::smActiveTasks = 0;


AosSdocDocument::AosSdocDocument(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_DOCUMENT, AosSdocId::eDocument, flag)
{
}


AosSdocDocument::~AosSdocDocument()
{
}


bool
AosSdocDocument::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	//<smartdoc zky_sdoctp="document" zky_otype="zky_smtdoc">
	//<file_path ..../>
	//<file_doc ..../>
	//</smartdoc>
	
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	//get file_doc
	// AosXmlTagPtr file_doc = AosDocSelector::selectDoc(sdoc, "file_docsel", rdata);  
	AosXmlTagPtr file_doc = AosRunDocSelector(rdata, sdoc, "file_docsel");  
	if (!file_doc)
	{
		rdata->setError() << "Failed to get file doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	//get file_doc_objid
	OmnString file_objid = file_doc->getAttrStr(AOSTAG_OBJID, "");
	if (file_objid == "")
	{
		rdata->setError() << "Failed to get file objid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	//get file_path
	OmnString file_path = file_doc->getAttrStr("zky_rscdir", "");
	if (file_path == "")
	{
		rdata->setError() << "Failed to get file path!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString filename = file_doc->getAttrStr("zky_rscfn", "");
	if (filename == "")
	{
		rdata->setError() << "Failed to get file name!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosSdocDocumentPtr thisptr(this, false);
	smLock->lock();
	if (smActiveTasks < smThrds)
	{
		OmnThrdShellProcPtr runner = OmnNew CreateFileDoc(thisptr, sdoc, file_doc, smActiveTasks, rdata);
		bool rslt = addThreadShellProc(runner, rdata);
		aos_assert_rl(rslt, smLock, false);
		smActiveTasks++;
		smLock->unlock();
		return true;
	}
	smTasks.push(AosSdocDocument::Entry(thisptr, sdoc, file_doc, rdata));
	smLock->unlock();
	return true;
}

bool
AosSdocDocument::triggerNextTask()
{
	smLock->lock();
	smActiveTasks--;
	aos_assert_rl(smActiveTasks >= 0, smLock, false);
	if (smTasks.size() <= 0)
	{
		smLock->unlock();
		return true;
	}
	AosSdocDocument::Entry entry = smTasks.front();                                    
	smTasks.pop();
	OmnThrdShellProcPtr runner = OmnNew CreateFileDoc(entry.mDocument, entry.mSdoc, entry.mFileDoc, smActiveTasks, entry.mRundata);
	bool rslt = addThreadShellProc(runner, entry.mRundata);
	aos_assert_rl(rslt, smLock, false);
	smActiveTasks++;
	smLock->unlock();
	return true;
}

bool
AosSdocDocument::CreateFileDoc::run()
{
	aos_assert_r(mSdoc, false);
	aos_assert_r(mFileDoc, false);
	OmnString file_path = mFileDoc->getAttrStr("zky_rscdir", "");
	aos_assert_r(file_path != "", false);
	OmnString filename = mFileDoc->getAttrStr("zky_rscfn", "");
	aos_assert_r(filename != "", false);
	OmnString basedir = AosGetBaseDirname();
	OmnString sfname = basedir;
	sfname << "User/" << file_path << "/" << filename;
	OmnString file_objid = mFileDoc->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(file_objid != "", false);
	OmnString fname = basedir;
	fname << "User/" << file_path << "/tmp_file_" << mFileIdx << ".txt";
	
	OmnString ctime = OmnGetTime(AosLocale::eUSA);
	OmnString file_ctime = mFileDoc->getAttrStr(AOSTAG_CTIME, ctime);

	//  execute a command 
	int startidx = filename.find('.', true);
	OmnString document_type = filename.substr(startidx+1);  

	if (document_type == "doc" || document_type == "DOC")
	{
		OmnString cmd = "antiword ";
		cmd << "\"" << sfname << "\" > \"" << fname << "\"";
		system(cmd.data());
	}
	else if (document_type == "pdf" || document_type == "PDF")
	{
		OmnString cmd = "pdftotext -layout -q ";
		cmd << "\"" << sfname << "\" \"" << fname << "\"";
		system(cmd.data());
	}
	else
	{
		OmnAlarm << "invalid document type " << enderr;
		return false;
	}
	
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);   
	aos_assert_r(file->isGood(), false);

	OmnString dftctnr_objid = "ctnr";
	dftctnr_objid << AOSZTG_DOCUMENT;
	
	OmnString ctnr_objid = mSdoc->getAttrStr("zky_document_ctnr", dftctnr_objid);

	AosXmlTagPtr tag_doc = mSdoc->getFirstChild(AOSTAG_TAG);
	OmnString tags;
	if (tag_doc)
	{
		AosValueRslt valueRslt;
		if (AosValueSel::getValueStatic(valueRslt, tag_doc, mRundata))      
		{
			if (!valueRslt.isValid())
			{
				mRundata->setError() << "Value is invalid";
				return false;
			}

			if (valueRslt.isXmlDoc())
			{
				mRundata->setError() << "Value is an XML doc";
				return false;
			}
			tags = valueRslt.getValueStr1();
		}
	}

	//create doc
	OmnString fileStr = "";
	OmnString nextStr = "";
	int partnum = 1;

	OmnString tmpstr; 
	tmpstr << "<doc zky_fileobjid=\"" << file_objid << "\" "
		<< AOSTAG_PUBLIC_DOC << "=\"true\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"true\" "
		<< AOSTAG_OTYPE << "=\"" << AOSTAG_DOC_SECTION << "\" "
		<< "zky_rscfn=\"" << filename << "\" "
		<< AOSTAG_PARENTC << "=\"" << ctnr_objid << "\" "
		<< "zky_file_ctime=\"" << file_ctime << "\" "
		<< AOSTAG_CTIME << "=\"" << ctime << "\" ";
	
	if (document_type == "doc" || document_type == "DOC")
	{
		tmpstr << "zky_type_pic=\"image_6/do89504623689759.png\" "
				  << AOSTAG_ZKY_TYPE << "=\"DOC\" ";
	}
	else if (document_type == "pdf" || document_type == "PDF")
	{
		tmpstr << "zky_type_pic=\"image_6/ao89504623689765.png\" "
				  << AOSTAG_ZKY_TYPE << "=\"PDF\" ";
	}

	OmnString cid = mRundata->getCid();
	aos_assert_r(cid != "", 0);

	AosXmlTagPtr user_doc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, mRundata);
	aos_assert_r(user_doc, 0);

	OmnString user_name = user_doc->getAttrStr(AOSTAG_USERNAME, "");
	aos_assert_r(user_name != "", 0);

	tmpstr << AOSTAG_USERNAME << "=\"" << user_name << "\" ";

	if (tags != "")
	{
		tmpstr << AOSTAG_TAG << "=\"" << tags << "\" ";
	}

	while(!getFileInfo(fileStr, nextStr, file))
	{
		if (fileStr == "")
			continue;

		OmnString doc_str = tmpstr;
		doc_str << "zky_partnum=\"" << partnum << "\">"
				<< "<zky_context><![CDATA[" 
				<< fileStr 
				<< "]]></zky_context></doc>";
		AosCreateDoc(doc_str, true, mRundata);
		partnum++;
		fileStr = nextStr;
		nextStr = "";
	}

	if (fileStr != "")
	{
		OmnString doc_str = tmpstr; 
		doc_str	<< "zky_partnum=\"" << partnum << "\">"
				<< "<zky_context><![CDATA[" 
				<< fileStr 
				<< "]]></zky_context></doc>";
		AosCreateDoc(doc_str, true, mRundata);
	}
	
	file->deleteFile();
	file = 0;
	//trigger next task
	mDocument->triggerNextTask();
	return true;
}

bool
AosSdocDocument::CreateFileDoc::getFileInfo(OmnString &doc_str, OmnString &next_str, const OmnFilePtr &file)
{
	aos_assert_r(file && file->isGood(), false);
	bool finished = false;
	//next_str = file->getLine(finished);
	//doc_str += file->getLine(finished);
	int str_len = doc_str.length();
	//while(str_len < eMaxBytes && !finished)
	//string reg_str = ".*(。|\\.|\\?|\\!|！|？)\\s*$";
	string reg_str = ".*(。|？|！|\\.|\\?|\\!)( |　)*$";
	boost::regex reg(reg_str);
	string reg_str1 = "^( |　|\\\302\\\240)+.*";
	boost::regex reg1(reg_str1);
	OmnString pre_str = doc_str;
	if (doc_str != "")
	{
		doc_str << "\n";	
	}
	
	while(!finished)
	{
		next_str = file->getLine(finished);
		next_str.replace('\f', ' ');
		next_str.replace('\a', ' ');
		next_str.replace('\003', ' ');
	

		if (boost::regex_match(pre_str.data(), reg))
		{
			if (boost::regex_match(next_str.data(), reg1) && str_len >= eMaxBytes)
			{
				doc_str << "\n";
				return finished;
			}
		}
		
		if (next_str != "")
		{
			OmnString tmpstr = next_str;
			tmpstr.normalizeWhiteSpace(true, true);
			if (tmpstr != "")
			{
				pre_str = next_str;
				doc_str << pre_str << "\n";
				next_str = "";
			}
		}
		str_len = doc_str.length();
		
		if (str_len >= eMaxBytes)
		{
			doc_str << "\n";
			return finished;
		}
		
	}
	
	OmnString tmp_docstr = doc_str;
	tmp_docstr.normalizeWhiteSpace(true, true);
	if (tmp_docstr == "" || 
		tmp_docstr == "\n" || 
		tmp_docstr == "\t" ||
		tmp_docstr == "\0")
	{
		doc_str = "";
	}

	if (doc_str != "" && doc_str != "\n")
	{
		return false;
	}
	return finished;
	/*
	bool finished = false;
	OmnString str = file->getLine(finished);
	while(!finished)
	{
		AosStrSplit split(str, ":");
		int num = split.entries();
		vector<OmnString> entries = split.entriesV();
		OmnString ctns;
		for(int i=1; i<num-1; i++)
		{
			ctns << entries[i];
			if (i == num-2) 
				break;
			ctns << ":";
		}
		doc_str << entries[0] << "=\"" << ctns << "\" ";

		str = file->getLine(finished);	
	}
	*/
}

bool
AosSdocDocument::CreateFileDoc::procFinished()
{
	return true;
}

#endif
