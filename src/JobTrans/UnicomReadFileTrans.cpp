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
//   
//
// Modification History:
// 03/15/2014	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/UnicomReadFileTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "Util/File.h"
#include "Util/Ptrs.h"

const OmnString zipFormat = ".eippack";

AosUnicomReadFileTrans::AosUnicomReadFileTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eUnicomReadFile, regflag)
{
}


AosUnicomReadFileTrans::AosUnicomReadFileTrans(
		const OmnString &fname,
		const int svr_id)
:
AosTaskTrans(AosTransType::eUnicomReadFile, svr_id, false, true),
mFileName(fname)
{
}


AosUnicomReadFileTrans::~AosUnicomReadFileTrans()
{
}


bool
AosUnicomReadFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mFileName = buff->getOmnStr("");
	aos_assert_r(mFileName != "", false);
	return true;
}


bool
AosUnicomReadFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mFileName);
	return true;
}


AosTransPtr
AosUnicomReadFileTrans::clone()
{
	return OmnNew AosUnicomReadFileTrans(false);
}


bool
AosUnicomReadFileTrans::proc()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	
	OmnFile file(mFileName, OmnFile::eReadOnly AosMemoryCheckerArgs); 
	OmnString str;
	bool rslt = file.readToString(str); 
	//aos_assert_r(rslt, false); 
	if (!rslt)
	{
		sendResp(resp_buff);
		return true;
	}

	unsigned found = str.indexOf(0, '\n', false); 
	str = str.substr(found+1, str.length()-1); 
	AosXmlTagPtr file_xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
	//aos_assert_r(file_xml, false);
	if (!file_xml)
	{
		sendResp(resp_buff);
		return true;
	}

	vector<AosBuffPtr> new_buffs;
	vector<AosXmlTagPtr> new_xmls;
	AosBuffPtr zip_buff;
	rslt = readXmlFindFile(file_xml, zip_buff, new_buffs, new_xmls); 
	OmnString cmdRmDir;
	cmdRmDir << "rm -rf " << mRmDataDir;
	if (mRmDataDir != "" && mRmDataDir != "/" && mRmDataDir != "~/");
	{
		system(cmdRmDir.data());
	}

	//aos_assert_r(rslt, false);
	//aos_assert_r(new_buffs.size() == new_xmls.size(), false);
	
	int size = new_buffs.size();
	resp_buff->setInt(size);
	
	AosXmlTagPtr unicomtitle_doc = file_xml->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(unicomtitle_doc, false);

	bool parse_rslt = xmlParse(unicomtitle_doc);
	//aos_assert_r(rslt, false);
	file.deleteFile();
	if (!parse_rslt)
	{
		unicomtitle_doc->setAttr("zip_error", "true");
		u32 len = unicomtitle_doc->getDataLength();
		resp_buff->setU32(len);
		resp_buff->setBuff((char *)unicomtitle_doc->getData(), len);
	
		sendResp(resp_buff);
		return true;
	}

	if (!rslt || new_buffs.size() != new_xmls.size())
	{
		unicomtitle_doc->setAttr("zip_error", "true");
		u32 len = unicomtitle_doc->getDataLength();
		resp_buff->setU32(len);
		resp_buff->setBuff((char *)unicomtitle_doc->getData(), len);
	
		sendResp(resp_buff);
		return true;
	}
	
	u32 len = unicomtitle_doc->getDataLength();
	resp_buff->setU32(len);
	resp_buff->setBuff((char *)unicomtitle_doc->getData(), len);
	
	len = zip_buff->dataLen();
	resp_buff->setU32(len);
	resp_buff->setBuff(zip_buff);

	for(size_t i=0; i<new_buffs.size(); i++)
	{
		len = new_xmls[i]->getDataLength();
		resp_buff->setU32(len);
		resp_buff->setBuff((char *)new_xmls[i]->getData(), len);
		
		resp_buff->setU32(new_buffs[i]->dataLen());
		resp_buff->setBuff(new_buffs[i]);
	}
	sendResp(resp_buff);
	return true;
}


bool 
AosUnicomReadFileTrans::readXmlFindFile(
		const AosXmlTagPtr &doc,
		AosBuffPtr &zip_buff,
		vector<AosBuffPtr> &new_buffs,
		vector<AosXmlTagPtr> &new_xmls)
{
	OmnScreen << " UnicomReadFileTrans starc :" << endl;
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
	
	doc->removeAttr("xmlns");
	doc->removeAttr("xmlns:xsi");
	doc->removeAttr("xsi:schemaLocation");   
	AosXmlTagPtr allDoc = doc->getFirstChild("AllDoc");
	aos_assert_r(allDoc, false); 
	int loopNum = 50;

	vector<AosXmlTagPtr> items = getDomainItem(allDoc, loopNum);
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
	aos_assert_r(child_doc, false);

	AosXmlTagPtr format_tag = child_doc->getFirstChild("PackageFormat");
	aos_assert_r(format_tag, false);
	OmnString zip_file_format = format_tag->getNodeText();
	if(zip_file_format != "ZIP") return false;

	AosXmlTagPtr path_tag = child_doc->getFirstChild("PackageName");
	aos_assert_r(path_tag, false);
	OmnString zip_file_path = path_tag->getNodeText();
	
	OmnString newDir = zip_file_path;
//OmnScreen << "UnicomReadFileTrans zip_file_path is : " << zip_file_path << endl;
	OmnString zip_file_name = zip_file_path;
	zip_file_name << ".zip";
	zip_file_path << zipFormat;
	OmnString path;
	unsigned found = mFileName.indexOf(0, '/', true);
	path = mFileName.substr(0, found);      
	path << zip_file_path;
	
	doc->setAttr(AOSTAG_SOURCE_FNAME, zip_file_name);
	doc->setAttr(AOSTAG_RSC_FNAME, zip_file_name);		
	doc->setAttr("unicom_file_name", newDir);		

//OmnScreen << "@@@@ path : " << path << " zip_file_path : " << zip_file_path << endl;
	OmnFile zip_file(path, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!zip_file.isGood() || zip_file.getLength() <= 0)
	{
		OmnAlarm << " zip_file is not good " << path << enderr;
		doc->setAttr("zky_unicom_file_error", 201);
		zip_file.deleteFile();
		return false;
	}
	//aos_assert_r(zip_file.getLength() > 0, false);
	int zip_buff_size = zip_file.getLength();
	zip_buff = OmnNew AosBuff(zip_buff_size AosMemoryCheckerArgs);
	zip_file.readToBuff(0, zip_buff_size, zip_buff->data());
	zip_buff->setDataLen(zip_buff_size);
	//felicia, 2013/08/27  for download
	//doc->setAttr("zky_binarydoc", "blockfile");
	doc->setAttr(AOSTAG_SOURCE_LENGTH, zip_buff_size);
	doc->setAttr(AOSTAG_RSCDIR, "/tmp");
	
	OmnString unzip_file_path = unzip(path, newDir);
	//doc->removeNode("AllDoc", true, true);
	AosXmlTagPtr xml_template = doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(xml_template, false);

	AosXmlTagPtr tag_package = xml_template->getChild(1);
	aos_assert_r(tag_package, false);

	AosXmlTagPtr tag_content = tag_package->getChild(1);
	aos_assert_r(tag_content, false);
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
//OmnScreen << "unescapeCdataContents 1 :: " << fname_tag->getNodeText() << endl;
		fname_tag->unescapeCdataContents();
//OmnScreen << "unescapeCdataContents 2 :: " << fname_tag->getNodeText() << endl;
		OmnString fname = fname_tag->getNodeText();

		OmnString fileChecksum = ((file_tag->getFirstChild(true))->getFirstChild("FileChecksum"))->getNodeText();

		OmnString fileFormat = ((file_tag->getFirstChild(true))->getFirstChild("FileFormat"))->getNodeText();

		if (fileFormat != "")
		{
			fname << "." << fileFormat;
		}
		OmnString file_path;
		file_path << unzip_file_path << "/" <<fname;	

		OmnFile file(file_path, OmnFile::eReadOnly AosMemoryCheckerArgs);
		if (!file.isGood())
		{
			OmnAlarm << " file is not good " << file_path << enderr;
			doc->setAttr("zky_unicom_file_error", 202);
			zip_file.deleteFile();
			return true;
		}

		int buff_size = file.getLength();
		AosBuffPtr buff = OmnNew AosBuff(buff_size+10 AosMemoryCheckerArgs);
		char *data = buff->data();
		int64_t data_len = buff_size;
		int64_t read_len = file.readToBuff(0, data_len, data);

		aos_assert_r(read_len == buff_size, false);
		buff->setDataLen(data_len);

		OmnString filestr(buff->data(), buff->dataLen());

		
		OmnString mdstr = AosMD5Encrypt(filestr);	
		if (mdstr != fileChecksum)
		{
			OmnAlarm << " fileCheckSum error " << file_path << enderr;
			doc->setAttr("zky_unicom_file_error", 303);
		//	system("mkdir -p /tmp/MDFERROR/");
		//	OmnFile file("/tmp/MDFERROR/md5error.txt", OmnFile::eWriteCreate AosMemoryCheckerArgs);
		//	file.append(file_path, true);
		//	file.append("\n", 2, true);
			zip_file.deleteFile();
			return true;
		}

		AosXmlTagPtr binary_doc = xml_template->clone(AosMemoryCheckerArgsBegin);
		binary_doc->removeNode("AllDoc", true, true);

		((binary_doc->getChild(1))->getChild(1))->addNode(file_tag_c);
		
		binary_doc->setAttr(AOSTAG_UN_READ_PERMISSION, vv);
		binary_doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
		binary_doc->setAttr(AOSTAG_FILE_FORMAT, fileFormat);
		binary_doc->setAttr(AOSTAG_SOURCE_LENGTH, buff_size);
		binary_doc->setAttr(AOSTAG_SOURCE_FNAME, fname);

		mLoopTmp = 100;
		bool rstl = xmlParse(binary_doc);
		aos_assert_r(rstl, false);

		new_buffs.push_back(buff);
		new_xmls.push_back(binary_doc);
		file_tag = files_tag->getNextChild();
	}
	zip_file.deleteFile();
	return true;
}

vector<AosXmlTagPtr>
AosUnicomReadFileTrans::getDomainItem(
		const AosXmlTagPtr &doc,
		int loopNum)
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



OmnString 
AosUnicomReadFileTrans::unzip(
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
//OmnScreen << " UnicomReadFileTrans created Dir : " << newDir << endl;
	OmnString cmdCreateDir;
	cmdCreateDir << "mkdir " << newDir;
//OmnScreen << " UnicomReadFileTrans created Dir : " << cmdCreateDir << endl;
	system(cmdCreateDir.data()); 

	OmnString dirPath; 
	dirPath << path << dirname; 
	OmnString cmdUnzip; 
	cmdUnzip << "unzip -oO CP936 " << zipPath << " -d " << newDir; 
//OmnScreen << "UnicomReadFileTrans cmdUnzip is : " << cmdUnzip << endl;
	system(cmdUnzip.data()); 
	return newDir;                                             
}                                                                  


bool
AosUnicomReadFileTrans::xmlParse(AosXmlTagPtr &xml)
{
	aos_assert_r(xml, false);
	AosXmlTagPtr doc = xml->getFirstChild(true);
	while (doc && (mLoopTmp--))
	{
		if((doc->getTagname()) == "AllDoc") 
		{
			doc = xml->getNextChild();
			continue;
		}

		if(doc->getFirstChild(true))
		{
			xmlParse(doc);
			doc = xml->getNextChild();
			continue;
		}
		OmnString name = doc->getTagname();
		aos_assert_r(name != "", false);

		OmnString value = doc->getNodeText();

		if (value != "")
		xml->setAttr(name, value);

		doc = doc->getNextSibling();
		xml->removeNode(name, false, false);
	}
	return true;
}
