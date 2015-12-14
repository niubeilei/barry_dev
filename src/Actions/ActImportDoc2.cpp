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
#if 0
#include "Actions/ActImportDoc2.h"

#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocidMgrObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "DataAssembler/DataAssembler.h"
#include "DataAssembler/DocAsmVar.h"
#include "SEUtil/IILName.h"
#include "Porting/Sleep.h"
#include "QueryClient/QueryClient.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BuffData.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/SdocDocument.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"
#include "WordParser/WordParser.h"
#include "XmlUtil/XmlTag.h"

AosDocProc::AosSeWordHash   AosActImportDoc2::mWords[eMaxThrdIds];

AosActImportDoc2::AosActImportDoc2(const bool flag)
:
AosTaskAction(AOSACTTYPE_IMPORTDOC2, AosActionType::eImportDoc2, flag),
mLock(OmnNew OmnMutex()),
mFileIdx(0),
mRunNum(0),
mThreadId(0)
{
}


AosActImportDoc2::~AosActImportDoc2()
{
}

bool
AosActImportDoc2::initAction(
	const AosTaskObjPtr &task, 
	const AosXmlTagPtr &sdoc,
	const AosRundataPtr &rdata)
{
	if(!task)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
/*
	AosTaskDataObjPtr task_data = task->getTaskData();
	
	if(!task_data)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
*/

	mTask = task;

	OmnString dftctnr_objid = "ctnr";
	dftctnr_objid << AOSZTG_DOCUMENT;
	
	mDocumentCtnr = sdoc->getAttrStr("zky_document_ctnr", dftctnr_objid);
	
	AosXmlTagPtr docasm_conf = sdoc->getFirstChild("zky_docassembler");
	aos_assert_r(docasm_conf, false);

	OmnString dclid = docasm_conf->getAttrStr(AOSTAG_NAME);
	aos_assert_r(dclid != "", false);
	
	AosXmlTagPtr datacol_tag = mTask->getDataColTag(dclid, rdata);
	aos_assert_r(datacol_tag, false);

	OmnString asm_key = datacol_tag->getAttrStr(AOSTAG_NAME);
	aos_assert_r(asm_key != "", false);
	
	mDocVarAssembler = AosDataAssembler::createAssemblerV1(asm_key, mTask, datacol_tag, rdata.getPtrNoLock());

	//OmnString dclid = sdoc->getAttrStr("zky_datacolid");
	dclid = sdoc->getAttrStr(AOSTAG_NAME);
	aos_assert_r(dclid != "", false);
	
	datacol_tag = mTask->getDataColTag(dclid, rdata);
	aos_assert_r(datacol_tag, false);
	
	//OmnString asm_key = datacol_tag->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
	asm_key = datacol_tag->getAttrStr(AOSTAG_NAME);
	aos_assert_r(asm_key != "", false);

	//AosXmlTagPtr iilasm_tag = datacol_tag->getFirstChild("iilasm");
	AosXmlTagPtr iilasm_tag = datacol_tag->getFirstChild("asm");
	aos_assert_r(iilasm_tag, false);

	AosXmlTagPtr record_tag = iilasm_tag->getFirstChild("datarecord");
	aos_assert_r(record_tag, false);

	//mRcdLen = iilasm_tag->getAttrInt(AOSTAG_LENGTH, -1);
	mRcdLen = record_tag->getAttrInt(AOSTAG_LENGTH, -1);
	aos_assert_r(mRcdLen != -1, false);

	// Chen Ding, 2013/12/15
	// mHitIILAsm = AosDataAssembler::createIILAssembler(
	// 				asm_key, mTask, datacol_tag, rdata);
	//mHitIILAsm = AosCreateIILAssembler(rdata, asm_key, mTask, datacol_tag);
	
	mHitIILAsm = AosDataAssembler::createAssemblerV1(asm_key, mTask, datacol_tag, rdata.getPtr());
	aos_assert_r(mHitIILAsm, false);
	
	bool rslt = false;

	mAttrPaths.clear();
	mAttrNames.clear();
	//rslt = getIILAsms(sdoc, mAttrPaths, mAttrNames, rdata);
	rslt = getIILAsms(sdoc, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(mAttrPaths.size() == mAttrNames.size(), false);

	//mScanner = task_data->createDataScanner(rdata);	
	//aos_assert_r(mScanner, false);

	rslt = createDataScanner(sdoc, rdata);
	aos_assert_r(rslt, false);

	 for (int i=0; i<eMaxThrdIds; i++)
	 {
	      mWordNorm[i] = OmnNew AosWordNorm("", "wordNorm");
	      mWordParser[i] = OmnNew AosWordParser();
	 }
	return true;
}


bool
AosActImportDoc2::createDataScanner(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr splitter_conf = sdoc->getFirstChild("splitter");
	aos_assert_r(splitter_conf, false);

	AosXmlTagPtr query_xml = splitter_conf->getFirstChild("query");
	aos_assert_r(query_xml, false);
	
	AosXmlTagPtr cond = query_xml->xpathGetFirstChild("conds/cond");
	aos_assert_r(cond, false);
			
	OmnString opr = "eq";
	OmnString envname = "zky_jobdocid";
	//Get Env Value
	u64 evalue = mTask->getJobDocid();

	OmnString termstr = "<term type=\"arith\" order=\"false\" reverse=\"false\"><selector type=\"attr\" aname=\"";
	termstr << envname << "\"><![CDATA[]]></selector><cond type=\"arith\" ctype=\"const\" zky_opr=\"";
	termstr << opr << "\"><![CDATA[";
	termstr << evalue << "]]></cond></term>";
	AosXmlParser termparser;
	AosXmlTagPtr node = termparser.parse(termstr, "" AosMemoryCheckerArgs);
	cond->addNode(node);
/*
	AosXmlTagPtr entries = splitter_conf->getFirstChild("entries");
	if(entries)
	{
		AosXmlTagPtr cond = query_xml->xpathGetFirstChild("conds/cond");
		AosXmlTagPtr entry = entries->getFirstChild(true);
		while(entry)
		{
			OmnString opr = entry->getAttrStr(AOSTAG_OPR, "");
			aos_assert_r(opr != "",  false);
			OmnString envname = entry->getAttrStr("envname", "");
			aos_assert_r(envname != "", false);
			//Get Env Value
			OmnString evalue = mJobENV[envname];
			aos_assert_r(evalue != "", false);

			OmnString termstr = "<term type=\"arith\" order=\"false\" reverse=\"false\"><selector type=\"attr\" aname=\"";
				termstr << envname << "\"><![CDATA[]]></selector><cond type=\"arith\" ctype=\"const\" zky_opr=\"";
				termstr << opr << "\"><![CDATA[";
				termstr << evalue << "]]></cond></term>";
			AosXmlParser termparser;
			AosXmlTagPtr node = termparser.parse(termstr, "" AosMemoryCheckerArgs);
			cond->addNode(node);
			entry = entries->getNextChild();
		}
	}
*/
	OmnString row_delimiter= splitter_conf->getAttrStr(AOSTAG_ROW_DELIMITER);
	
	vector<u64> docids;
	bool rslt = query(query_xml, docids, rdata);
	aos_assert_r(rslt, false);

OmnScreen << " qqqqqqqqqqq  docids_num : " << docids.size() << endl;
	mScanner = AosDataScannerObj::createBinaryDocScannerStatic(docids, 0, rdata);
	aos_assert_r(mScanner, false);
	return true;
}
	
bool
AosActImportDoc2::query(
		const AosXmlTagPtr &query_xml,
		vector<u64> &docids,
		const AosRundataPtr &rdata) 
{
	docids.clear();

	//OmnSleep(10);

	int total = 0;
	int num;
	int start_idx = 0;
	AosXmlTagPtr record;
	query_xml->setAttr("psize", 1000);
	
	//int pre_physicalid = -1;
	AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(query_xml, rdata);
	aos_assert_rr(query && query->isGood(), rdata, false); 
	
	while(!query->finished())
	{
		OmnString contents;
		bool rslt = query->procPublic(query_xml, contents, rdata);
		aos_assert_rr(rslt, rdata , false);
		
		//<contents>
		//	<record zky_docid="xxxx"/>
		//	...........
		//</contents>
		AosXmlTagPtr data= AosXmlParser::parse(contents AosMemoryCheckerArgs);
		aos_assert_rr(data, rdata, false);

		total = data->getAttrInt("total", 0);	

		record = data->getFirstChild(true);
		while(record)
		{
			u64 docid = record->getAttrU64(AOSTAG_DOCID, 0);
			aos_assert_r(docid, false);
		/*
			int physicalid = AosGetPhysicalId(docid); 
			if (pre_physicalid != -1 && pre_physicalid != physicalid)
			{
				OmnAlarm << " error " << enderr;
			}
			else
			{
				pre_physicalid = physicalid;
			}
		*/
			docids.push_back(docid);
			record = data->getNextChild();
		}

		num = data->getAttrInt("num", 0);
		if (num == 0) break;

		start_idx += num;
		if (start_idx >= total) break;

		query->setStartIdx(start_idx);
	}
	return true;
}


bool
AosActImportDoc2::runAction(const AosRundataPtr &rdata)
{
//OmnScreen << " ###################### ActImportDoc2 runAction ########## " << endl;
	
	bool rslt = false;

	AosBuffPtr buff = 0;
	
	mLock->lock();
	aos_assert_rl(mScanner, mLock, false);
	AosBuffDataPtr info;
	rslt = mScanner->getNextBlock(info, rdata);
	aos_assert_rl(info, mLock, false);
	buff = info->getBuff();
	if (!rslt || !buff)
	{
		//finished	
		mLock->unlock();
		rslt = actionFinished(rdata);
		return rslt;
	}

	mRunNum++;
	int64_t fileidx = mFileIdx;
	mFileIdx++;
	mLock->unlock();

	OmnString dir = AosGetBaseDirname();
	OmnString source_fname = dir;
	source_fname << "source_" << mTask->getTaskDocid() << "_" << fileidx << ".txt";

	OmnFilePtr file = 0;

	aos_assert_r(buff->dataLen() > 0, false);

	// Chen Ding, 2013/11/23
	// AosXmlTagPtr binary_doc = info->getXmlDoc();
	AosXmlTagPtr binary_doc = info->getMetadata();
	aos_assert_r(binary_doc, false);

	OmnString nodename = binary_doc->getAttrStr(AOSTAG_BINARY_NODENAME);
	binary_doc->setTextBinary(nodename, buff);

	OmnString sfname = binary_doc->getAttrStr(AOSTAG_SOURCE_FNAME);
	aos_assert_r(sfname != "", false);

	AosStrSplit split(sfname, ".");            
	vector<OmnString> strs= split.entriesV();

	//OmnString document_type = binary_doc->getAttrStr(AOSTAG_FILE_FORMAT, "doc");
	OmnString document_type = strs[strs.size()-1];
	aos_assert_r(document_type != "", false);

	if (!file)
	{
		file = OmnNew OmnFile(source_fname, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(file && file->isGood(), false);
	}
	file->put(0, buff->data(), buff->dataLen(), true);

	OmnString fname = convertWordPdf(dir, source_fname, document_type, fileidx);
	if (fname == "")
	{
		file->deleteFile();
		mLock->lock();
		mRunNum--;
		aos_assert_rl(mRunNum >= 0, mLock, false);
		mLock->unlock();
		return true;
	}

	vector<OmnString> has_attrs;
	vector<OmnString> has_values;
	//AosXmlTagPtr predoc = getPreDoc(binary_doc, sfname, document_type, mAttrPaths, mAttrNames, has_attrs, has_values, rdata);	
	map<OmnString, vector<OmnString> > split_values;
	AosXmlTagPtr predoc = getPreDoc(binary_doc, sfname, has_attrs, has_values, split_values, rdata);	
	aos_assert_r(predoc, false);

	//proc(predoc, has_attrs, has_values, fname, rdata);	

//	file->deleteFile();
	proc(predoc, has_attrs, has_values, split_values, fname, rdata);	

	if (document_type != "txt" || document_type != "TXT")
	{
		file->deleteFile();
	}

	mLock->lock();
	mRunNum--;
	aos_assert_rl(mRunNum >= 0, mLock, false);
	mLock->unlock();
	return true;
}

bool 
AosActImportDoc2::actionFinished(const AosRundataPtr &rdata)
{
	mLock->lock();
	if (!mTask)
	{
		mLock->unlock();
		return true;
	}
	if (!mTask->isTaskRunFinished())
	{
OmnScreen << "~~~~~~~ ActImportDoc2 finished ~~~~~~" <<endl;
		AosActionObjPtr thisptr(this, true);	
  		mTask->actionRunFinished(rdata, thisptr);
		mLock->unlock();
		
		while(1)
		{
			mLock->lock();
			aos_assert_rl(mRunNum >= 0, mLock, false);
			if (mRunNum == 0)
			{
				mLock->unlock();
				break;
			}
			mLock->unlock();
			OmnSleep(1);
		}

		bool rslt = asmSendFinish(rdata);
		aos_assert_r(rslt, false);

		//mTask = 0;
		return true;
	}                                       
	mLock->unlock();	
	return true;
}

bool
AosActImportDoc2::asmSendFinish(const AosRundataPtr &rdata)
{
	bool rslt = mDocVarAssembler->sendFinish(rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	rslt = mHitIILAsm->sendFinish(rdata.getPtr());
	aos_assert_r(rslt, false);

	map<OmnString, AosDataAssemblerObjPtr>::iterator itr = mIILAsms.begin();
	for(; itr != mIILAsms.end(); ++itr)
	{
		rslt = (itr->second)->sendFinish(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}
	
	itr = mOprIILAsms.begin();
	for(; itr != mOprIILAsms.end(); ++itr)
	{
		rslt = (itr->second)->sendFinish(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}
	return true;
}

AosXmlTagPtr
AosActImportDoc2::getPreDoc(
		const AosXmlTagPtr &doc,
		const OmnString &sfname,
		vector<OmnString> &has_attrs,
		vector<OmnString> &has_values,
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

	/*
	if (document_type == "doc" || document_type == "DOC")
	{
		predocstr << "zky_type_pic=\"image_6/do89504623689759.png\" "
				  << AOSTAG_ZKY_TYPE << "=\"DOC\" ";
	}
	else if (document_type == "pdf" || document_type == "PDF")
	{
		predocstr << "zky_type_pic=\"image_6/ao89504623689765.png\" "
				  << AOSTAG_ZKY_TYPE << "=\"PDF\" ";
	}
	else
	{
		predocstr << AOSTAG_ZKY_TYPE << "=\"" << document_type << "\" ";
	}
	*/

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

	for(u32 i=0; i<mAttrPaths.size(); i++)
	{
		OmnString value = doc->xpathQuery(mAttrPaths[i]);
	
		if(mAttrNames[i] == AOSTAG_OTYPE)
		{
			value = AOSTAG_DOC_SECTION;
		}
		else if(mAttrNames[i] == AOSTAG_CT_EPOCH)
		{
			value = "";
			value << systemsec;
		}
		else if(mAttrNames[i] == "zky_fileobjid")
		{
			value = file_objid;
		}
		else if(mAttrNames[i] == AOSTAG_PARENTC)
		{
			value = mDocumentCtnr;
		}
		
		if (value != "")
		{
			predoc->xpathSetAttr(mAttrPaths[i], value, true);
			has_attrs.push_back(mAttrNames[i]);
			has_values.push_back(value);
/*
			if (mAttrNames[i] == "DocumentTitle")
			{
				u64 title_docid = doc->getAttrU64("unicom_title_docid", 0);
				if (title_docid)
				{
					splitBytes(value, title_docid, rdata);
				}
				else
				{
					OmnAlarm << "title docid is 0" << enderr;
				}
			}
*/
		}
	}
	
	for(u32 i=0; i<mOprAttrPaths.size(); i++)
	{
		OmnString value = doc->xpathQuery(mOprAttrPaths[i]);
		if (value != "")
		{
			AosStrSplit split(value, mOprs[i].data());            
			vector<OmnString> strs= split.entriesV();
			mOprValues.insert(make_pair(mOprAttrNames[i], strs));
			predoc->xpathSetAttr(mOprAttrPaths[i], value, true);
		}
	}
	
	for(u32 i=0; i<mSplitAttrPaths.size(); i++)
	{
		OmnString value = doc->xpathQuery(mSplitAttrPaths[i]);
		if (value != "")
		{
			map<OmnString, OmnString>::iterator split_itr = mSplitAttrWords.find(mSplitAttrNames[i]);
			aos_assert_r(split_itr != mSplitAttrWords.end(), 0);
			vector<OmnString> strs;
			if (split_itr->second != "")
			{
				AosStrSplit split(value, (split_itr->second).data());            
				strs= split.entriesV();
			}
			else
			{
				strs.push_back(value);
			}
			split_values.insert(make_pair(mSplitAttrNames[i], strs));
			predoc->xpathSetAttr(mSplitAttrPaths[i], value, true);
		}
	}

	return predoc;
}

bool
AosActImportDoc2::getIILAsms(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr attrs_tag = sdoc->getFirstChild("attrs");
	
	if (!attrs_tag)
		return true;

	AosXmlTagPtr attrtag = attrs_tag->getFirstChild();
	while(attrtag)
	{
		OmnString xpath = attrtag->getAttrStr("zky_attrpath");
		aos_assert_r(xpath != "", false);

		OmnString attrname = attrtag->getAttrStr("zky_attrname");
		aos_assert_r(attrname != "", false);
		
		OmnString opr = attrtag->getAttrStr("zky_opr");
		OmnString iiltype = attrtag->getAttrStr("zky_iiltype");
		if (iiltype == "hit")
		{
			mSplitAttrPaths.push_back(xpath);
			mSplitAttrNames.push_back(attrname);
			mSplitAttrWords.insert(make_pair(attrname, opr));
			attrtag = attrs_tag->getNextChild();
			continue;
		}
		if (opr != "" && iiltype == "")
		{
			mOprs.push_back(opr);
			mOprAttrPaths.push_back(xpath);
			mOprAttrNames.push_back(attrname);
		}
		else
		{
			mAttrPaths.push_back(xpath);
			mAttrNames.push_back(attrname);
		}


		map<OmnString, AosDataAssemblerObjPtr>::iterator itr = mIILAsms.find(attrname);
		if (itr == mIILAsms.end())
		{
			AosXmlTagPtr datacol_tag = mTask->getDataColTag(attrname, rdata);     
			aos_assert_r(datacol_tag, false);
			
			// Chen Ding, 2013/12/15
			// AosDataAssemblerObjPtr iilasm = AosDataAssembler::createIILAssembler(
			// 		attrname, mTask, datacol_tag, rdata);
			//AosDataAssemblerObjPtr iilasm = AosCreateIILAssembler(rdata, attrname, mTask, datacol_tag);
			AosDataAssemblerObjPtr iilasm = AosDataAssembler::createAssemblerV1(attrname, mTask, datacol_tag, rdata.getPtrNoLock());
			aos_assert_r(iilasm, false);

			//mIILAsms.insert(make_pair(attrname, iilasm));
			if (opr != "")
			{
				mOprIILAsms.insert(make_pair(attrname, iilasm));
			}
			else
			{
				mIILAsms.insert(make_pair(attrname, iilasm));
			}
		}

		attrtag = attrs_tag->getNextChild();
	}
	return true;
}

OmnString
AosActImportDoc2::convertWordPdf(
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


bool
AosActImportDoc2::proc(
		const AosXmlTagPtr &predoc,
		const vector<OmnString> &has_attrs,
		const vector<OmnString> &has_values,
		map<OmnString, vector<OmnString> > &split_values,
		const OmnString &fname, 
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
	
		bool rslt = asmAppend(filestr, partnum++, predoc, has_attrs, has_values, split_values, rdata);
		aos_assert_r(rslt, false);

		filestr = nextstr;
		nextstr = "";
	}
	
	if (filestr != "")
	{
		bool rslt = asmAppend(filestr, partnum++, predoc, has_attrs, has_values, split_values, rdata);
		aos_assert_r(rslt, false);
	}

	ff->deleteFile();
	return true;
}

bool 
AosActImportDoc2::asmAppend(
		const OmnString &filestr,
		const int &partnum,
		const AosXmlTagPtr &predoc,
		const vector<OmnString> &has_attrs,
		const vector<OmnString> &has_values,
		map<OmnString, vector<OmnString> > &split_values,
		const AosRundataPtr &rdata)
{
	u64 docid = getNextDocid(rdata);
	aos_assert_r(docid, false);

	OmnString docstr = composeEntry(docid, filestr, partnum, predoc);
	aos_assert_r(docstr != "", false);

	AosValueRslt valueRslt;
	valueRslt.setDocid(docid);
	valueRslt.setCharStr1(docstr.data(), docstr.length(), false);
	mDocVarAssembler->appendEntry(valueRslt, rdata.getPtrNoLock());	

	iilAppendEntry(docid, has_attrs, has_values, rdata);
	OprIILAppendEntry(docid, predoc, split_values, rdata);

	AosWordParser wordParser;
	//wordParser.setSrc(docstr);
	wordParser.setSrc(filestr);
	OmnString word;

	u64 title_docid = predoc->getAttrU64("unicom_title_docid", 0);
	aos_assert_r(title_docid > 0, false);
	while (wordParser.nextWord(word))
	{
		if (word == "\n")
		{
			continue;
		}
		if (word.length() >= (mRcdLen - (int)sizeof(u64)))
		{
			word.setLength(mRcdLen - (int)sizeof(u64) - 2 );
		}

		AosValueRslt valueRslt1;
		valueRslt1.setCharStr1(word.data(), word.length(), true);
		valueRslt1.setDocid(docid);
		
		splitBytes(word, docid, title_docid, rdata);
		
		mHitIILAsm->appendEntry(valueRslt1, rdata.getPtr());

		valueRslt1.setDocid(title_docid);
		mHitIILAsm->appendEntry(valueRslt1, rdata.getPtr());
	}
	return true;
}

void
AosActImportDoc2::splitBytes(
		const OmnString &word,
		const u64 &docid,
		const u64 &title_docid,
		const AosRundataPtr &rdata)
{
	const char * a  = word.data();
	//static unsigned char arr[] = {0b10000000, 0b11100000, 0b11110000, 0b11111000};
	static u8 arr[] = {0b10000000, 0b11100000, 0b11110000, 0b11111000};
	static u8 brr[] = {0b00000000, 0b11000000, 0b11100000, 0b11110000};
	int total = 0;
	//OmnScreen << " ###########  hitword : " << word << endl;
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

		AosValueRslt valueRslt1;
		valueRslt1.setCharStr1(a, size, true);
		valueRslt1.setDocid(docid);
		
		mHitIILAsm->appendEntry(valueRslt1, rdata.getPtr());

		valueRslt1.setDocid(title_docid);
		mHitIILAsm->appendEntry(valueRslt1, rdata.getPtr());
		a += size;
		i += size;
	}
}

void
AosActImportDoc2::OprIILAppendEntry(
		const u64 docid,
		const AosXmlTagPtr &predoc,
		map<OmnString, vector<OmnString> > &split_values,
		const AosRundataPtr &rdata)
{
	map<OmnString, AosDataAssemblerObjPtr>::iterator itr;
	map<OmnString, vector<OmnString> >::iterator vitr = mOprValues.begin();
	for(; vitr != mOprValues.end(); ++vitr)
	{
		itr = mOprIILAsms.find(vitr->first);
		aos_assert(itr != mOprIILAsms.end());
	
		vector<OmnString> values = vitr->second;
		for(u32 i=0; i<values.size(); i++)
		{
			AosValueRslt valueRslt1;
			valueRslt1.setCharStr1(values[i].data(), values[i].length(), true);
			valueRslt1.setDocid(docid);
			(itr->second)->appendEntry(valueRslt1, rdata.getPtrNoLock());
		}
	}
	
	u64 title_docid = predoc->getAttrU64("unicom_title_docid", 0);
	aos_assert(title_docid > 0);
	
	map<OmnString, vector<OmnString> >::iterator split_vitr = split_values.begin();
	for(; split_vitr != split_values.end(); ++split_vitr)
	{
		OmnString attrname = split_vitr->first;
		vector<OmnString> strs = split_vitr->second;
		addAttrWordIIL(attrname, strs, docid, title_docid, rdata);
	}
}


void AosActImportDoc2::onThreadInit(const AosRundataPtr &, void **data)
{
	mLock->lock();
	long tid = mThreadId;
	mThreadId++;
	mLock->unlock();
	OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread();
	thread->setUdata((void*)tid);
}

void AosActImportDoc2::onThreadExit(const AosRundataPtr &, void **data)
{
	OmnThreadPtr thread = OmnThreadMgr::getSelf()->getCurrentThread();
	thread->setUdata((void*)0);
}

bool
AosActImportDoc2::addAttrWordIIL(
		const OmnString &attrname,
		const vector<OmnString> &values,
		const u64 &docid,
		const u64 &title_docid,
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
		bool rslt = AosDocProc::getSelf()->collectWords(mWords[logicId], mWordParser[logicId], mWordNorm[logicId], values[i]);
		aos_assert_r(rslt, false);

		OmnString word;
		AosEntryMark::E mark;
		while(mWords[logicId].nextEntry(word, mark))
		{
//			OmnString iilname = AosDocProc::getSelf()->getAttrWordIILName(mark, "", attrname, word);
			OmnString iilname = AosIILName::composeAttrWordIILName(attrname, word);
			if (iilname == "") continue;

			if (iilname.length() > eMaxStrLen)
			{
				OmnAlarm << "iilname is too long " << iilname << enderr;
				continue;
			}

			AosValueRslt valueRslt1;
			valueRslt1.setCharStr1(iilname.data(), iilname.length(), true);
			valueRslt1.setDocid(docid);
			//OmnScreen << "@@@@@@  hitiilasm key : " << iilname << " docid : " << docid << endl;
			mHitIILAsm->appendEntry(valueRslt1, rdata.getPtr());

			valueRslt1.setDocid(title_docid);
			mHitIILAsm->appendEntry(valueRslt1, rdata.getPtr());
		}
	}
	return true;
}

void
AosActImportDoc2::iilAppendEntry(
		const u64 docid,
		const vector<OmnString> &has_attrs,
		const vector<OmnString> &has_values,
		const AosRundataPtr &rdata)
{
	map<OmnString, AosDataAssemblerObjPtr>::iterator itr;
	for(u32 i=0; i<has_attrs.size(); i++)
	{
		itr = mIILAsms.find(has_attrs[i]);
		aos_assert(itr != mIILAsms.end());
		
		AosValueRslt valueRslt1;
		valueRslt1.setCharStr1(has_values[i].data(), has_values[i].length(), true);
		valueRslt1.setDocid(docid);
		(itr->second)->appendEntry(valueRslt1, rdata.getPtrNoLock());
	}
}

OmnString
AosActImportDoc2::composeEntry(
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


u64
AosActImportDoc2::getNextDocid(const AosRundataPtr &rdata)
{
	OmnString objid = "";
	u64 docid = AosDocidMgrObj::getDocidMgr()->nextDocid(objid, rdata);
	return docid;
}


AosActionObjPtr
AosActImportDoc2::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActImportDoc2(false);
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
AosActImportDoc2::finishedAction(const AosRundataPtr &rdata)
{
	AosActionObjPtr thisptr(this, true);
	mTask->actionFinished(thisptr, rdata);
	return true;
}
#endif
