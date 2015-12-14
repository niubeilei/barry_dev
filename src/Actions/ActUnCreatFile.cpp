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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActUnCreatFile.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/DirDesc.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "API/AosApiC.h"
#include "DocSelector/DocSelector.h"
#include <string>
#include <vector>

const int MAX_SIZE = 1 * 1000 * 1000 * 1000;
const bool IS_PUBLIC = true;

AosActUnCreatFile::AosActUnCreatFile(const bool flag)
:
AosSdocAction(AOSACTTYPE_UNCREATEFILE, AosActionType::eUnCreateFile, flag)
{
}

AosActUnCreatFile::~AosActUnCreatFile()
{
}

bool
AosActUnCreatFile::isXMLFile(
		const OmnString fileName)
{	
	aos_assert_r(fileName != "", false);	
	int idx = fileName.indexOf(0, '.', true);
	int len = fileName.length();
	OmnString fileFormat = fileName.substr(idx+1, len - 1);
	OmnString format = "xml"; 
	if (fileFormat == format)
		return true;
	return false;
}


bool 
AosActUnCreatFile::xmlParse(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &xml)
{
	aos_assert_rr(xml, rdata, false);
	AosXmlTagPtr doc = xml->getFirstChild(true);
	while (doc)
	{
		if(doc->getFirstChild(true))
		{
			OmnScreen << "------------- next ------"<< endl;
			xmlParse(rdata, doc);
			doc = xml->getNextChild();
			continue;
		}
		OmnString name = doc->getTagname();
		OmnScreen << "----------- name ::" << name << endl;
		aos_assert_rr(name != "", rdata, false);

		OmnString value = doc->getNodeText();
		OmnScreen << "----------- value ::" << value<< endl;
		aos_assert_rr(value!= "", rdata, false);
		
		xml->setAttr(name, value);
		doc = doc->getNextSibling();
		xml->removeNode(name, false, false);
	}
	return true;
}


bool
AosActUnCreatFile::readXmlFindFile(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &doc,
		const OmnString xml_path,
		AosXmlTagPtr &newDoc)
{
	//doc xml format
	//<SIP>
	//	<PackageDescription>
	//	.........
	//	</PackageDescription>
	//	<Package>
	//		<Description>
	//		..........
	//		</Description>
	//		<Content>
	//			<Package>
	//				<Description>
	//				.............
	//				</Description>
	//			</Package>
	//		</Content>
	//		............
	//	</Package>
	//</SIP>
	
	aos_assert_r(xml_path != "", false);
	aos_assert_r(doc, false);
	
	OmnString zip_file_path;
	bool status = getZIPFile(rdata, xml_path, zip_file_path);
	aos_assert_r(status, false);

	OmnString unzip_file_path = unzip(zip_file_path);
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
		
		AosXmlTagPtr file_tag_c = file_tag->clone(AosMemoryCheckerArgsBegin);
		OmnString fname = ((file_tag->getFirstChild(true))->getFirstChild(true))->getNodeText();
		OmnString fileFormat = ((file_tag->getFirstChild(true))->getFirstChild("FileFormat"))->getNodeText();
		fname << "." << fileFormat;
		OmnString file_path;
		file_path << unzip_file_path << "/" << fname;	

		OmnFile file(file_path, OmnFile::eReadOnly AosMemoryCheckerArgs);
		aos_assert_r(file.getLength() > 0, false);

		int buff_size = file.getLength();
		if (buff_size > MAX_SIZE)
		{
			AosSetErrorU(rdata, "file_too_big") << buff_size
				<< ". Filename: " << file_path << enderr;
			return false;
		}

		AosBuffPtr buff = OmnNew AosBuff(buff_size+10 AosMemoryCheckerArgs);
		char *data = buff->data();
		int64_t data_len = buff_size;
		int64_t read_len = file.readToBuff(0, data_len, data);
		aos_assert_r(read_len == buff_size, false);
		buff->setDataLen(data_len);
		
		AosXmlTagPtr binary_doc = xml_template->clone(AosMemoryCheckerArgsBegin);

		((binary_doc->getChild(1))->getChild(1))->addNode(file_tag_c);
		
		binary_doc->setAttr(AOSTAG_HPCONTAINER, mPctrs);
		binary_doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
		binary_doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
		binary_doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
		binary_doc->setAttr(AOSTAG_FILE_FORMAT, fileFormat);
		binary_doc->setAttr(AOSTAG_FILE_SIZE, buff_size);
		binary_doc->setAttr(AOSTAG_FILE_NAME, fname);
		rdata->setWorkingDoc(binary_doc, true);
		xmlParse(rdata, binary_doc);
		
		OmnScreen << binary_doc->toString();
		AosXmlTagPtr newdoc = AosCreateBinaryDoc(-1, binary_doc->toString(), IS_PUBLIC, buff, rdata);
		aos_assert_r(newdoc, false);

		file_tag = files_tag->getNextChild();
	}
	return true;
}


bool
AosActUnCreatFile::getZIPFile(
		const AosRundataPtr &rdata,
		const OmnString xml_path,
		OmnString &zip_path)
{
	aos_assert_r(xml_path != "", false);
	
	zip_path = "";
	AosStrSplit split(xml_path, ".");
	vector<OmnString> tmp = split.entriesV();
	aos_assert_r(tmp.size(), false);
	zip_path << tmp[0] << ".zip" ;		
	return true;
}


OmnString
AosActUnCreatFile::unzip(OmnString &zipPath)                               
{                                                                 
	aos_assert_r(zipPath != "", "");                           

	unsigned found = zipPath.indexOf(0, '/', true); 
	int len = zipPath.length();
	OmnString path = zipPath.substr(0, found);                
	OmnString file = zipPath.substr(found+1, len);                 

	unsigned tmpf = file.indexOf(0, '.', true);                       
	OmnString dirname = file.substr(0, tmpf-1);                        

	OmnString dirPath;
	dirPath << path << dirname;
	OmnString cmdUnzip;
	cmdUnzip << "unzip -oO CP936 " << zipPath << " -d " << path;
	system(cmdUnzip.data());
	mUnZipFilePath = dirPath;
	return dirPath;                                            
}                                                                 


bool
AosActUnCreatFile::readXmlFile(
		const AosRundataPtr &rdata,
		const OmnString &path)
{
	OmnDirDescPtr dir;
	try
	{
		dir = OmnNew OmnDirDesc(path);
	}

	catch (...)
	{
		OmnAlarm << "Directory not exist: " << path << enderr;
		return false;
	}
	aos_assert_r(dir, false);

	OmnString fname;
	bool rslt = dir->getFirstFile(fname);
	while (rslt)
	{
		if (!isXMLFile(fname))
		{
			rslt = dir->getNextFile(fname);
			continue;
		}

		aos_assert_r(fname != "", false);
		OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);

		int file_size = file.getLength();
		if (file_size > MAX_SIZE)
		{
			AosSetErrorU(rdata, "file_too_big") << file_size
				<< ". Filename: " << fname << enderr;
			return false;
		}
		OmnString str; 
		bool status = file.readToString(str);
		aos_assert_r(status, false);
	
		unsigned found = str.indexOf(0, '\n', false);
		str = str.substr(found+1, str.length()-1);

		AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
		aos_assert_r(xml, false);

		AosXmlTagPtr new_xml;
		readXmlFindFile(rdata, xml, fname, new_xml);

		OmnString cmdRmDir;
		cmdRmDir << "rm -rf " << mUnZipFilePath;
		system(cmdRmDir.data());

		rslt = dir->getNextFile(fname);
	}

	return true;
}


bool	
AosActUnCreatFile::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// This action use for unicom file importer . 
	//
	// <action type="uncreatefile" dir_name="/xxxxx/xxxx/xx" zky_pctrs="xxx"
	// </action>

	aos_assert_r(def, false);
	OmnString dirName = def->getAttrStr("dir_name", "");
	aos_assert_r(dirName != "", false);

	mPctrs = def->getAttrStr(AOSTAG_HPCONTAINER, "");

	bool status = readXmlFile(rdata, dirName);
	aos_assert_r(status, false);

	return true;
}


AosActionObjPtr
AosActUnCreatFile::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActUnCreatFile(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


