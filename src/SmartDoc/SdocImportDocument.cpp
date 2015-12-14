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
// 03/03/2012	Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#if 0
#include "SmartDoc/SdocImportDocument.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SdocDocument.h"
#include "UtilComm/TcpClient.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/File.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"

AosSdocImportDocument::AosSdocImportDocument(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_IMPORTDOCUMENT, AosSdocId::eImportDocument, flag),
mLoopTmp(0)
{
}


AosSdocImportDocument::~AosSdocImportDocument()
{
}


bool
AosSdocImportDocument::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	// 1. create binary doc  //andy
	// 2. import doc  //felicia


	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mLogSmtDocObjid = sdoc->getAttrStr("zky_log_smtdoc");
	aos_assert_r(mLogSmtDocObjid != "", false);

	mLogContainerObjid = sdoc->getAttrStr("zky_log_ctnr", "document_log_ctnr");

	mFileName = sdoc->getAttrStr("zky_document_file", "");
	aos_assert_r(mFileName != "", false);

	OmnString dftctnr_objid = "ctnr";
	dftctnr_objid << AOSZTG_DOCUMENT;
	
	mDocumentCtnr = sdoc->getAttrStr("zky_document_ctnr", dftctnr_objid);

	// file name is "xxxxxxxx.eippack"	

	mZipFormat = sdoc->getAttrStr("zky_zip_format", "eippack");
	
	mUuicomTitlePctrs = sdoc->getAttrStr("unicom_title_ctnr", "");
	aos_assert_r(mUuicomTitlePctrs != "", false)
	
	mUuicomFilePctrs = sdoc->getAttrStr("unicom_file_ctnr", "");
	aos_assert_r(mUuicomFilePctrs != "", false)

/*
	AosXmlTagPtr attrs = sdoc->getFirstChild(true);
	if (attrs)
	{
		AosXmlTagPtr attr = attrs->getFirstChild(true); 
		while (attr)
		{
			AosValueRslt value;
			AosXmlTagPtr key_doc = attr->getFirstChild("xpath");
			AosXmlTagPtr value_doc = attr->getFirstChild("value");

			bool rslt = AosValueSel::getValueStatic(value, key_doc, rdata);
			if (!rslt || value.getTypeStr() != AOSDATATYPE_STRING)
			{
				rdata->setError() << "Failed to get the cash attribute name!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;                                                                   
			}
			OmnString key = value.getValueStr1("");
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
			OmnString strValue = value.getValueStr1("");
			if ( strValue == "")
			{
				rdata->setError() << "cash attribute name is null!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			mAttrMap[key] = strValue;
			attr = attrs->getNextChild();
		}
	}
*/

	//AosBuffMetaDataPtr metaData = OmnNew AosBuffMetaData();
	AosBuffDataPtr metaData = OmnNew AosBuffData();
	bool rslt = readUnicomFileBuff(metaData, rdata);
	aos_assert_r(rslt, false);

	vector<AosXmlTagPtr> docs = metaData->getVectorDocs();
	vector<AosBuffPtr> buffs = metaData->getVectorBuffs();
	AosXmlTagPtr unicom_xml = metaData->getMetadata();


	AosBuffPtr zip_buff = metaData->getBuff();
	unicom_xml->setAttr(AOSTAG_HPCONTAINER, mUuicomTitlePctrs);
	unicom_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	unicom_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");             
	unicom_xml->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
	AosXmlTagPtr new_xml = AosCreateBinaryDoc(-1, unicom_xml->toString(), true, zip_buff, rdata);
	aos_assert_r(new_xml, false);

	//u64 unicomtitle_objid = new_xml->getAttrU64(AOSTAG_OBJID, 0);
	OmnString unicomtitle_objid = new_xml->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(unicomtitle_objid != "", false);
	aos_assert_r(docs.size() == buffs.size(), false);
	
	OmnString fname = unicom_xml->getAttrStr("unicom_file_name", "");
	aos_assert_r(fname != "", false);
	if (docs.size())
	{
		mFileStart[fname] = 1; 
	}
	else
	{
		mFileStart[fname] = 0; 
	}

//OmnScreen << " ActImportUnicomFile docs size : " << docs.size() << endl;
	for(u32 i = 0; i < docs.size(); i++)
	{
		AosXmlTagPtr doc = docs[i];
		AosBuffPtr buff = buffs[i];
		map<OmnString, OmnString>::iterator it;
/*
		for(it = mAttrMap.begin(); it != mAttrMap.end(); it++)
		{
			doc->setAttr(it->first, it->second);
		}
*/
		doc->setAttr("zky_file_type", "上传");
		doc->setAttr("zky_binarydoc", "blockfile");
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
		doc->setAttr("unicom_title_objid", unicomtitle_objid);
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
		doc->setAttr(AOSTAG_HPCONTAINER, mUuicomFilePctrs);
		doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
		doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");             
		
		//felicia, for batch upload file
		addAttrForUpload(doc, rdata);
		
		AosXmlTagPtr newdoc = AosCreateBinaryDoc(-1, doc->toString(), true, buff, rdata);
		aos_assert_r(newdoc, false);
	
		// felicia
		// create docs
		aos_assert_r(buff->dataLen() > 0, false);
		rslt = importData(buff, newdoc, rdata);
		aos_assert_r(rslt, false);
	}

	rslt = finishedAction(rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosSdocImportDocument::addAttrForUpload(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	OmnString file_type = doc->getAttrStr("zky_file_type");
	if (file_type == "上传")
	{
		//AosStrSplit split(info.mFileName, "/");            
		//vector<OmnString> strs= split.entriesV();
		//OmnString fname = strs[strs.size()-1];
		/*
		int startidx = file_name.find('/', true);
		OmnString fname = file_name.substr(startidx+1);  
		doc->setAttr(AOSTAG_RSC_FNAME, fname);		
		doc->setAttr(AOSTAG_RSCDIR, "/tmp");

		startidx = fname.find('.', true);
		OmnString document_type = fname.substr(startidx+1);
		*/
		OmnString fname = doc->getAttrStr(AOSTAG_SOURCE_FNAME);
		aos_assert_r(fname != "", false);

		doc->setAttr(AOSTAG_RSC_FNAME, fname);		
		doc->setAttr(AOSTAG_RSCDIR, "/tmp");
		
		OmnString document_type = doc->getAttrStr(AOSTAG_FILE_FORMAT);
		document_type = document_type.toLower();
		if (document_type == "doc" || document_type == "docx" || document_type == "wps" || document_type == "wpt")
		{
	//		doc->setAttr("zky_type_pic", "image_6/do89504623689759.png");
			doc->setAttr(AOSTAG_ZKY_TYPE,"DOC");
		}
		else if (document_type == "pdf")
		{
	//		doc->setAttr("zky_type_pic", "image_6/ao89504623689765.png");
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
AosSdocImportDocument::finishedAction(const AosRundataPtr &rdata)
{
	if (mLogSmtDocObjid == "") return true;
	OmnString str;
	str << "<entries total_len=\"" << 1 << "\" "
		<< "noupdate_flag=\"" << false << "\">";
	std::map<OmnString, int>::iterator it;	
	OmnString ctime = OmnGetTime(AosLocale::eUSA);
	for (it = mFileStart.begin(); it != mFileStart.end(); it++)
	{
		str << "<entry zky_key=\"" << it->first << "\" "
			<< AOSTAG_CTIME << "=\"" << ctime << "\">" 
			<< it->second << "</entry>";
	}

	str << "</entries>";
	OmnScreen << str << endl;

	AosAddContainerLog(mLogContainerObjid, str, rdata);
//	AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
//	aos_assert_r(xml, false);

	rdata->setLog(str);

	AosSmartDocObjPtr sdoc_obj = AosSmartDocObj::getSmartDocObj();
	aos_assert_r(sdoc_obj, false);
	sdoc_obj->runSmartdocs(mLogSmtDocObjid, rdata);
//	aos_assert_r(mTask, false);
//	mTask->updateTaskProcNum(xml, false, rdata);
	return true;
}


bool
AosSdocImportDocument::readUnicomFileBuff(
		AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	OmnFile file(mFileName, OmnFile::eReadOnly AosMemoryCheckerArgs); 
	aos_assert_r(file.getLength() > 0, false);
	OmnString str;
	bool rslt = file.readToString(str); 
	aos_assert_r(rslt, false); 

	unsigned found = str.indexOf(0, '\n', false); 
	str = str.substr(found+1, str.length()-1); 
	AosXmlTagPtr file_xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(file_xml, false);

	vector<AosBuffPtr> new_buffs;
	vector<AosXmlTagPtr> new_xmls;
	AosBuffPtr zip_buff;
	rslt = readXmlFindFile(rdata, file_xml, zip_buff, new_buffs, new_xmls); 
	OmnString cmdRmDir;
	cmdRmDir << "rm -rf " << mRmDataDir;
	if (mRmDataDir != "" && mRmDataDir != "/" && mRmDataDir != "~/");
	{
OmnScreen << "DirScanner cmdRmDir is : " << cmdRmDir << endl;
		system(cmdRmDir.data());
	}

	info->setVectorDocs(new_xmls);
	info->setVectorBuffs(new_buffs);
	AosXmlTagPtr unicomtitle_doc = file_xml->clone(AosMemoryCheckerArgsBegin);
	bool rstl = xmlParse(rdata, unicomtitle_doc);
	aos_assert_r(rstl, false);
	info->addMetadata(unicomtitle_doc);
	info->setBuff(zip_buff);
	return true;
}

bool 
AosSdocImportDocument::readXmlFindFile(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		AosBuffPtr &zip_buff,
		vector<AosBuffPtr> &new_buffs,
		vector<AosXmlTagPtr> &new_xmls)
{
	OmnScreen << " DirScanner starc :" << endl;
	//doc xml format
	//<SIP>
	//  <PackageDescription>
	//  .........
	//  </PackageDescription>
	//  <Package>
	//      <Description>
	//      ..........
	//      </Description>
	//      <Content>
	//          <Package>
	//              <Description>
	//              .............
	//              </Description>
	//          </Package>
	//      </Content>
	//      ............
	//  </Package>
	//</SIP>
	aos_assert_r(doc, false);
	new_buffs.clear();
	new_xmls.clear();
	
	doc->removeAttr("xmlns:xsi");
	doc->removeAttr("xsi:schemaLocation");   
	AosXmlTagPtr allDoc = doc->getFirstChild("AllDoc");
	aos_assert_r(allDoc, false); 
	int loopNum = 50;

	vector<AosXmlTagPtr> items = getDomainItem(allDoc, loopNum, rdata);
	aos_assert_r(items.size() > 0, false);

	OmnString vv;
	for (u32 i = 0; i < items.size(); i++)
	{
		AosXmlTagPtr textList = items[i]->getFirstChild(true);
		AosXmlTagPtr text = textList->getFirstChild(true);
		
		while(text)
		{
			if (vv != "") vv << ",";
			vv << (text->getNodeText());
			text = textList->getNextChild();
		}
	}

	doc->setAttr(AOSTAG_UN_READ_PERMISSION, vv);

	AosXmlTagPtr child_doc = doc->getFirstChild(true);
	OmnString zip_file_format = child_doc->getFirstChild("PackageFormat")->getNodeText();
	//if(zip_file_format != "ZIP") return false;
	OmnString zip_file_path = child_doc->getFirstChild("PackageName")->getNodeText();
	OmnString newDir = zip_file_path;
OmnScreen << "DirScanner zip_file_path is : " << zip_file_path << endl;
	OmnString zip_file_name = zip_file_path;
	zip_file_name << ".zip";
	zip_file_path << "." <<mZipFormat;
	OmnString path;
	unsigned found = mFileName.indexOf(0, '/', true);
	path = mFileName.substr(0, found);      
	path << zip_file_path;

OmnScreen << "@@@@ path : " << path << " zip_file_path : " << zip_file_path << endl;
	OmnFile zip_file(path, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(zip_file.getLength() > 0, false);
	int zip_buff_size = zip_file.getLength();
	zip_buff = OmnNew AosBuff(zip_buff_size AosMemoryCheckerArgs);
	zip_file.readToBuff(0, zip_buff_size, zip_buff->data());
	zip_buff->setDataLen(zip_buff_size);
	
	//felicia, 2013/08/27  for download
	doc->setAttr("zky_binarydoc", "blockfile");
	doc->setAttr(AOSTAG_SOURCE_LENGTH, zip_buff_size);
	doc->setAttr(AOSTAG_SOURCE_FNAME, zip_file_name);
	doc->setAttr(AOSTAG_RSC_FNAME, zip_file_name);		
	doc->setAttr("unicom_file_name", newDir);		
	doc->setAttr(AOSTAG_RSCDIR, "/tmp");
	
	OmnString unzip_file_path = unzip(path, newDir);
	doc->removeNode("AllDoc", true, true);
	AosXmlTagPtr xml_template = doc->clone(AosMemoryCheckerArgsBegin);
	
	AosXmlTagPtr tag_package = xml_template->getChild(1);
	AosXmlTagPtr tag_content = tag_package->getChild(1);
	AosXmlTagPtr files_tag = tag_content->clone(AosMemoryCheckerArgsBegin);
	tag_content->removeAllChildren();

	AosXmlTagPtr file_tag = files_tag->getFirstChild(true);
	while(file_tag)
	{
		//file_tag
		//<Package>
		//	<Descriptioni>
		//		<OriginalFileName>xxxx</OriginalFileName>
		//		<DocumentType></DocumentType>
		//		<FileSize></FileSize>
		//		<FileTimeStamp></FileTimeStamp>
		//		<FileChecksum></FileChecksum>
		//		<FileFormat></FileFormat>
		//	</Descriptioni>
		//</Package>
		
		//file_tag->unescapeCdataContents();
		AosXmlTagPtr file_tag_c = file_tag->clone(AosMemoryCheckerArgsBegin);
		AosXmlTagPtr fname_tag = (file_tag->getFirstChild(true))->getFirstChild(true);
		fname_tag->unescapeCdataContents();
OmnScreen << "unescapeCdataContents 1 :: " << fname_tag->getNodeText() << endl;
		fname_tag->unescapeCdataContents();
OmnScreen << "unescapeCdataContents 2 :: " << fname_tag->getNodeText() << endl;
		OmnString fname = fname_tag->getNodeText();
		OmnString fileFormat = ((file_tag->getFirstChild(true))->getFirstChild("FileFormat"))->getNodeText();
		fname << "." << fileFormat;
		OmnString file_path;
		file_path << unzip_file_path << "/" <<fname;	

		OmnFile file(file_path, OmnFile::eReadOnly AosMemoryCheckerArgs);
		aos_assert_r(file.getLength() > 0, false);
		int buff_size = file.getLength();

		AosBuffPtr buff = OmnNew AosBuff(buff_size+10 AosMemoryCheckerArgs);
		char *data = buff->data();
		int64_t data_len = buff_size;
		int64_t read_len = file.readToBuff(0, data_len, data);

		aos_assert_r(read_len == buff_size, false);
		buff->setDataLen(data_len);
		
		AosXmlTagPtr binary_doc = xml_template->clone(AosMemoryCheckerArgsBegin);

		((binary_doc->getChild(1))->getChild(1))->addNode(file_tag_c);
		
		binary_doc->setAttr(AOSTAG_UN_READ_PERMISSION, vv);
		binary_doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
		binary_doc->setAttr(AOSTAG_FILE_FORMAT, fileFormat);
		binary_doc->setAttr(AOSTAG_SOURCE_LENGTH, buff_size);
		binary_doc->setAttr(AOSTAG_SOURCE_FNAME, fname);

		mLoopTmp = 100;
		bool rstl = xmlParse(rdata, binary_doc);
		aos_assert_r(rstl, false);

		new_buffs.push_back(buff);
		new_xmls.push_back(binary_doc);
		file_tag = files_tag->getNextChild();
	}
	return true;
}


OmnString
AosSdocImportDocument::unzip(
		OmnString &zipPath,
		const OmnString &fname)
{
	aos_assert_r(zipPath != "", "");

	unsigned found = zipPath.indexOf(0, '/', true);
	int len = zipPath.length();
	OmnString path = zipPath.substr(0, found);
	OmnString file = zipPath.substr(found+1, len);

	unsigned tmpf = file.indexOf(0, '.', true);
	OmnString dirname = file.substr(0, tmpf-1);

	OmnString newDir;
	newDir << path << fname;
	mRmDataDir = newDir;
	OmnScreen << " DirScanner created Dir : " << newDir << endl;
	OmnString cmdCreateDir;
	cmdCreateDir << "mkdir " << newDir;
	OmnScreen << " DirScanner created Dir : " << cmdCreateDir << endl;
	system(cmdCreateDir.data());

	OmnString dirPath;
	dirPath << path << dirname;
	OmnString cmdUnzip;
	cmdUnzip << "unzip -oO CP936 " << zipPath << " -d " << newDir;
	OmnScreen << "DirScanner cmdUnzip is : " << cmdUnzip << endl;
	system(cmdUnzip.data());
	return newDir;
}


//set  tagname to  Attribute
bool
AosSdocImportDocument::xmlParse(                            
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &xml)
{
	aos_assert_rr(xml, rdata, false);
	AosXmlTagPtr doc = xml->getFirstChild(true);
	while (doc && (mLoopTmp--))
	{
		if(doc->getFirstChild(true))
		{
			xmlParse(rdata, doc);
			doc = xml->getNextChild();
			continue;
		}
		OmnString name = doc->getTagname();
		aos_assert_rr(name != "", rdata, false);

		OmnString value = doc->getNodeText();

		if (value != "")
			xml->setAttr(name, value);

		doc = doc->getNextSibling();
		xml->removeNode(name, false, false);
	}
	return true;
}


vector<AosXmlTagPtr>
AosSdocImportDocument::getDomainItem(
		const AosXmlTagPtr &doc,
		int loopNum,
		const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> items;
	AosXmlTagPtr tmpDoc = doc->clone(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr document_tag;
	AosXmlTagPtr item = tmpDoc;
	OmnString vv = tmpDoc->getAttrStr("name", "");
	bool tag = false;
	while(item && tmpDoc && (0 < loopNum))
	{
		OmnString tagname = tmpDoc->getTagname();
		if (tagname != "item" && !tag)
		{
			if (tagname == "document") 
			{
				tag = true;
				document_tag = tmpDoc;
				continue;
			}
			tmpDoc = tmpDoc->getFirstChild(true);
			loopNum--;
			continue;
		}
		if (tag && document_tag)    
		{
			item = document_tag->getNextChild();
			if (!item) continue;
			vv = item->getAttrStr("name", "");
			if (vv == AOSTAG_FLREVIEWEDJOBOS || 
					vv == AOSTAG_LOOKLEADER ||
					vv == AOSTAG_OTHERREADERS)
			{
				items.push_back(item);
			}
			continue;
		}
	}
	return items;
}


bool
AosSdocImportDocument::importData(
		const AosBuffPtr &buff,
		const AosXmlTagPtr &binary_doc,
		const AosRundataPtr &rdata)
{
	OmnString dir = AosGetBaseDirname();
	OmnString source_fname = dir;
	source_fname << "source_file.txt";

	OmnFilePtr file = 0;

	OmnString nodename = binary_doc->getAttrStr(AOSTAG_BINARY_NODENAME);
	binary_doc->setTextBinary(nodename, buff);

	OmnString sfname = binary_doc->getAttrStr(AOSTAG_SOURCE_FNAME);
	aos_assert_r(sfname != "", false);

	AosStrSplit split(sfname, ".");            
	vector<OmnString> strs= split.entriesV();

	OmnString document_type = strs[strs.size()-1];
	aos_assert_r(document_type != "", false);

	if (!file)
	{
		file = OmnNew OmnFile(source_fname, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(file && file->isGood(), false);
	}
	file->put(0, buff->data(), buff->dataLen(), true);

	OmnString fname = convertWordPdf(dir, source_fname, document_type);
	if (fname == "")
	{
		file->deleteFile();
		return true;
	}

	OmnString predocstr = getPreDocStr(binary_doc, sfname, rdata);	
	aos_assert_r(predocstr != "", false);

	bool rslt = proc(predocstr, fname, rdata);	
	aos_assert_r(rslt, false);

	if (document_type != "txt" || document_type != "TXT")
	{
		file->deleteFile();
	}
	return true;
}

OmnString
AosSdocImportDocument::convertWordPdf(
		const OmnString &dir,
		const OmnString &source_fname,
		const OmnString &document_type)
{
	OmnString target_fname = dir;
	target_fname << "target_file.txt";

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

OmnString
AosSdocImportDocument::getPreDocStr(
		const AosXmlTagPtr &doc,
		const OmnString &sfname,
		const AosRundataPtr &rdata)
{
	AosStrSplit split(sfname, "/");            
	vector<OmnString> strs= split.entriesV();
	OmnString fname = strs[strs.size()-1];

	OmnString file_objid = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(file_objid != "", 0);
	
	OmnString document_type = doc->getAttrStr(AOSTAG_ZKY_TYPE, "OTHER");

	OmnString ctime = OmnGetTime(AosLocale::eUSA);
	OmnString file_ctime = doc->getAttrStr(AOSTAG_CTIME, ctime);

	OmnString un_read_permission = doc->getAttrStr("un_read_permission");
	aos_assert_r(un_read_permission != "", "");

	OmnString document_title = doc->xpathQuery("Package/Description/@Title");
	OmnString DocumentType = doc->xpathQuery("Package/Content/Package/Description/@DocumentType");
	OmnString unicom_title_objid = doc->getAttrStr("unicom_title_objid");
	aos_assert_r(unicom_title_objid != "", "");

	OmnString predocstr = "";
	//u32 systemsec = OmnGetSecond();                   
	predocstr << "<doc "
		<< AOSTAG_PUBLIC_DOC << "=\"true\" "
	    << AOSTAG_CTNR_PUBLIC << "=\"true\" "
//		<< AOSTAG_CT_EPOCH << "=\"" << systemsec << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSTAG_DOC_SECTION << "\" "
		<< AOSTAG_PARENTC << "=\"" << mDocumentCtnr << "\" "
//		<< AOSTAG_CTIME << "=\"" << ctime << "\" "
		<< "zky_fileobjid=\"" << file_objid << "\" "
		<< AOSTAG_ZKY_TYPE << "=\"" << document_type << "\" "
		<< "zky_file_ctime=\"" << file_ctime << "\" "
		<< "DocumentType=\"" << DocumentType << "\" "
		<< "DocumentTitle=\"" << document_title << "\" "
		<< "unicom_title_objid=\"" << unicom_title_objid << "\" "
		<< "un_read_permission=\"" << un_read_permission << "\" "
		<< "zky_rscfn=\"" << fname << "\" ";

	OmnString cid = rdata->getCid();
	aos_assert_r(cid != "", 0);

	AosXmlTagPtr user_doc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
	aos_assert_r(user_doc, 0);

	OmnString user_name = user_doc->getAttrStr(AOSTAG_USERNAME, "");
	aos_assert_r(user_name != "", 0);

	predocstr << AOSTAG_USERNAME << "=\"" << user_name << "\" ";

	return predocstr;
}

bool
AosSdocImportDocument::proc(
		const OmnString &predocstr,
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
		
		// createdoc
		OmnString docstr = composeEntry(filestr, partnum++, predocstr);
		aos_assert_r(docstr != "", false);

		AosXmlTagPtr doc = AosCreateDoc(docstr, true, rdata);
		aos_assert_r(doc, false);

		filestr = nextstr;
		nextstr = "";
	}
	
	if (filestr != "")
	{
		OmnString docstr = composeEntry(filestr, partnum++, predocstr);
		aos_assert_r(docstr != "", false);
		
		AosXmlTagPtr doc = AosCreateDoc(docstr, true, rdata);
		aos_assert_r(doc, false);
	}
	
	ff->deleteFile();
	ff = 0;
	return true;
}

OmnString
AosSdocImportDocument::composeEntry(
		OmnString &filestr, 
		const int partnum,
		const OmnString &predocstr)
{
	OmnString doc_str = predocstr; 
	doc_str	<< "zky_partnum=\"" << partnum << "\">"
			<< "<zky_context><![CDATA[" 
			<< filestr 
			<< "]]></zky_context></doc>";
	return doc_str;
}
#endif
