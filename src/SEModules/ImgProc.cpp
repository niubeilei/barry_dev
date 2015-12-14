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
// 01/30/2010	Copied from TorturerWrapper/ObjectWrapper.cpp by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEModules/ImgProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Book/Tag.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "SEInterfaces/UserAcctObj.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "IdGen/IdGen.h"
#include "IdGen/U64IdGen.h"
#include "Porting/GetTime.h"
#include "Random/RandomUtil.h"
#include "Thread/Mutex.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/DocServerCb.h"
#include "SearchEngine/Containers.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Objid.h"
#include "SEUtil/SeErrors.h"
#include "SEModules/ObjMgr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEModules/ImgConverter.h"
#include "SEModules/LoginMgr.h"
#include "Thread/Mutex.h"
#include "UserMgmt/User.h"
#include "Util/File.h"
#include "Util/StrSplit.h"
#include "Util/OmnNew.h"
#include "util_c/strutil.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/Server/MainUtil.h"
#include "XmlInterface/Server/XintUtil.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include <dirent.h>
#include <sys/types.h>
//#include <ImageMagick/Magick++.h>
//using namespace Magick;

static OmnMutex sgLock;
static OmnMutex sgIdLock;
static OmnString sgImageFullDir;
static OmnString sgDftImageDir = "img100";
static OmnString sgImgBackdir = "/home/AOS/Backup/images";
static int sgNumFiles[AosImgProc::eMaxDirs];
static u64 sgImageId = 0; 

//AosImgConverterPtr	AosImgProc::mImgConverter;
int AosImgProc::mCrtSeqno = 0;

AosImgProc::AosImgProc()
{
	memset(sgNumFiles, -1, sizeof(int)*eMaxDirs);
}


AosImgProc::~AosImgProc()
{
}


bool
AosImgProc::staticInit(const AosXmlTagPtr &config)
{
if (!config)
	OmnMark;
	aos_assert_r(config, false);
	AosXmlTagPtr cc = config->getFirstChild(AOSCONFIG_IMGCONFIG);
	aos_assert_r(cc, false);

	//mImgConverter = OmnNew AosImgConverter();

  	sgImageFullDir = cc->getAttrStr(AOSCONFIG_IMAGEDIR);
	aos_assert_r(sgImageFullDir != "", false);
	if (sgImageFullDir.data()[sgImageFullDir.length()-1] == '/')
	{
		sgImageFullDir = sgImageFullDir.substr(0, sgImageFullDir.length()-1);
	}
	sgImgBackdir = cc->getAttrStr(AOSCONFIG_IMGBACKDIR, sgImgBackdir);
	return true;
}


OmnString
AosImgProc::getImgDir()
{
	return sgImageFullDir;
}

bool
AosImgProc::stop()
{
	/*
	if (mImgConverter)
	{
		mImgConverter->stop();
	}

	mImgConverter = 0;
	*/
	return true;
}


OmnString
AosImgProc::getImageDir(const u32 siteid)
{
	OmnString dirname;
	OmnString fullDir; 
	DIR *dir;
	struct dirent *dirp;
	int numFiles = 0;

	sgLock.lock();
	do
	{
		dirname = "image";
		dirname << "_";
		dirname << mCrtSeqno;
		// -1 it means we didn't check dir
		if (sgNumFiles[mCrtSeqno] != -1)
		{
			if (sgNumFiles[mCrtSeqno] < eMaxFiles)
			{
				sgLock.unlock();
				return dirname;
			}
			else
			{
				dirname = "image";
				dirname << "_";
				dirname << ++mCrtSeqno;
			}
		}

		fullDir = sgImageFullDir; 
		fullDir << "/";
		fullDir << dirname; 
		if ((dir = opendir(fullDir.data())) == NULL)
		{
			mkdir(fullDir.data(), 0755);
			dir = opendir(fullDir.data());
		}
		if (!dir)
		{
			sgLock.unlock();
			aos_assert_r(dir, "");
		}
		
		//caculate the number of files in directory
		numFiles = 0;
		while((dirp = readdir(dir))!=NULL)
			numFiles++;
		closedir(dir);

		sgNumFiles[mCrtSeqno] = numFiles;
		if (numFiles > eMaxFiles)
			mCrtSeqno++;
	}
	while(numFiles > eMaxFiles && mCrtSeqno < eMaxDirs );
	sgLock.unlock();
	aos_assert_r(dirname!="", sgDftImageDir);	
	return dirname; 
}


/*
OmnString
AosImgProc::getNextDirname(const u32 siteid)
{
	AosXmlTagPtr doc = getImgSysdoc();
	aos_assert_r(doc, sgDftImageDir);
	OmnString namebase = doc->getAttrStr(sgTagNamebase);
	AosXmlRc errcode;
	OmnString errmsg;
	if (namebase == "")
	{
		OmnAlarm << "Incorrect namebase!" << enderr;
		namebase = sgDftNamebase;
		int ttl = 0;
		bool rslt = AosDocServer::getSelf()->modifyAttrStr1(AOSAPPNAME_SYSTEM, 
				AosLoginMgr::getSuperUserDocid(siteid), 
				doc->getAttrU64(AOSTAG_DOCID, 0), 
				doc->getAttrStr(AOSTAG_OBJID), 
				sgTagNamebase, namebase, "", true, true, errcode, errmsg, 
				ttl, __FILE__, __LINE__);
		aos_assert_r(rslt, sgDftImageDir);
	}

	int seqno = doc->getAttrInt(sgTagSeqno, -1);
	if (seqno < 0)
	{
		OmnAlarm << "Incorrect seqno: " << seqno << enderr;
		seqno = sgDftSeqno;
		
		int ttl = 0;
		bool rslt = AosDocServer::getSelf()->modifyAttrU64(AOSAPPNAME_SYSTEM, 
				AosLoginMgr::getSuperUserDocid(siteid), 
				doc->getAttrU64(AOSTAG_DOCID, 0), 
				doc->getAttrStr(AOSTAG_OBJID), 
				sgTagSeqno, seqno, 0, true, true, errcode, errmsg, ttl);
		aos_assert_r(rslt, sgDftImageDir);
	}

	seqno++;
	int ttl = 0;
	bool rslt = AosDocServer::getSelf()->modifyAttrU64(AOSAPPNAME_SYSTEM, 
			AosLoginMgr::getSuperUserDocid(siteid), 
			doc->getAttrU64(AOSTAG_DOCID, 0), 
			doc->getAttrStr(AOSTAG_OBJID), 
			sgTagSeqno, seqno, 0, true, true, errcode, errmsg, ttl);
	aos_assert_r(rslt, sgDftImageDir);

	namebase << seqno;
	return namebase;
}
*/


/*
AosXmlTagPtr
AosImgProc::getImgSysdoc()
{
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDoc(AOS_DFTSITEID, AOSOBJID_IMGDIRMGR);
	if (doc) return doc;
	
	// All image directories are managed by 'AOSOBJID_IMGDIRMGR'
	OmnString docstr = "<rcd ";
	docstr << sgTagNamebase << "=\"" << sgImageDirNamebase << "\" "
		<< sgTagSeqno << "=\"" << sgDftSeqno << "\" "
		<< AOSTAG_SITEID << "=\"" << AOS_DFTSITEID << "\" "
		<< AOSTAG_OBJID << "=\"" << AOSOBJID_IMGDIRMGR << "\" "
		<< sgTagMaxfiles << "=\"" << sgDftMaxfiles << "\" " 
		<< sgTagBlocksize << "=\"" << sgDftBlocksize << "\"/>";
	AosXmlRc errcode;
	OmnString errmsg;
	int ttl = 0;
	//Linda, 2011/02/11 eAOSDOCID_ROOT1->getSuperUserDocid
	doc = AosDocServer::getSelf()->createDocSafe1(docstr, AOS_DFTSITEID, 
			AosLoginMgr::getSuperUserDocid(AOS_DFTSITEID), AOSAPPNAME_SYSTEM, "", "", true, 
			false, errcode, errmsg, false, false, false, true, ttl);
	aos_assert_r(doc, 0);
	return doc;
}
*/


/*
AosXmlTagPtr 
AosImgProc::retrieveDirdoc(const u32 siteid)
{
	AosXmlTagPtr doc = AosDocClinet::getSelf()->getDoc(siteid, AOSOBJID_IMGDIRMGR_SITE);
	if (doc) return doc;
	
	// The doc has not been created yet. Create it.
	OmnString newdirname = getNextDirname(siteid);
	aos_assert_r(newdirname != "", 0);

	// It is the time to create the doc
	// 	<rcd sgTagCrtName="xxx" 
	// 		sgTagCrtNum="0" 
	// 		AOSTAG_SITEID="xxx" 
	// 		AOSTAG_OBJID=AOSOBJID_IMGDIRMGR_SITE
	// 		sgTagBlocksize="xxx" 
	// 		sgTagMaxfiles="xxx"/> 
	// int maxfiles = doc->getAttrInt(sgTagMaxfiles, sgDftMaxfiles);
	// aos_assert_r(maxfiles > 0, 0);
	// int blocksize = doc->getAttrInt(sgTagBlocksize, sgDftBlocksize);
	// aos_assert_r(blocksize > 0, 0);
	int maxfiles = eDftMaxFiles;
	int blocksize = eDftBlocksize;
	OmnString docstr = "<rcd ";
	docstr << sgTagCrtName << "=\""
		<< newdirname << "\" "
		<< sgTagCrtNum << "=\"0\" "
		<< AOSTAG_SITEID << "=\"" << siteid << "\" "
		<< AOSTAG_OBJID << "=\"" << AOSOBJID_IMGDIRMGR_SITE << "\" "
		<< sgTagBlocksize << "=\"" << blocksize << "\" "
		<< sgTagMaxfiles << "=\"" << maxfiles << "\"/>";
	AosXmlRc errcode;
	OmnString errmsg;
	int ttl = 0;
	//Linda, 2011/02/11 eAOSDOCID_ROOT1->getSuperUserDocid
	doc = AosDocServer::getSelf()->createDocSafe1(docstr, AOS_DFTSITEID, 
				AosLoginMgr::getSuperUserDocid(siteid), AOSAPPNAME_SYSTEM, "", "", true, false,
				errcode, errmsg, false, false, false, false, ttl);
	aos_assert_r(doc, 0);
	return doc;
}
*/


bool
AosImgProc::uploadImage(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &objdef,
		const AosXmlTagPtr &cmd,
		OmnString &contents)
{
	// This function is called when an image file has been uploaded
	// into the location:
	// 		/tmp/<dirname>/<fname>.<ext>
	// It can be a single image file or an archive file that contains
	// multiple files (e.g., .rar, .zip, .tar, etc.)
	//
	// The image definition is in 'objdef', which is in the form:
	// <objdef>
	// 		<objdef zky_type="xxx"
	// 				zky_name="xxx"
	// 				zky_tnail="xxx">
	// 			<zky_tag>xxx</zky_tag>
	// 			<zky_tagc>xxx</zky_tagc>
	// 			<fname>xxx</fname>
	// 		</objdef>
	// </objdef>
	OmnString errmsg;
	OmnString dirname = objdef->getAttrStr(AOSTAG_IMGDIR);
	if (dirname == "")
	{
		AosSetErrorU(rdata, "missing_dirname") << enderr;
		return false;
	}

	OmnString fname = objdef->getAttrStr("fname");
	OmnString ext = getFileExtension(fname);
	if (ext == "rar")
	{
		//--Ketty
		//return createImagesPriv(root, objdef, cmd, siteid, userid, 
		//	dirname, fname, "rar", contents, errcode, errmsg);
		return createImagesPriv(rdata, objdef, cmd, dirname, fname, "rar", contents);
	}
	if (ext == "zip")
	{
		OmnNotImplementedYet;
		rdata->setError();
		return false;
	}
	if (ext == "jpg" || ext == "jpeg" || ext=="png" || ext == "bmp" || ext == "gif")
	{
		//--Ketty
		//return uploadImagePriv(root, objdef, cmd, siteid, userid, 
		//		dirname, ext, contents, errcode, errmsg);
		return uploadImagePriv(rdata, objdef, cmd, dirname, ext, contents);
	}

	AosSetErrorU(rdata, "not_image_file") << ": " << fname << enderr;

	// Remove the directory
	OmnString cc = "rm -rf ";
	cc << dirname;
	system(cc.data());
	return false;
}


bool
AosImgProc::uploadImagePriv(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &xmlobj,
		const AosXmlTagPtr &cmd, 
		const OmnString &dirname,
		const OmnString &fileext,
		OmnString &contents)
{
	// This function is called when an image file has been uploaded
	// into the location:
	// 		/tmp/<dirname>/<fname>.<ext>
	// This function assumes that there is one and only one file 
	// in <dirname> directory. 
	// <fname> may contain special characters or non-English characters.
	//
	// The function assumes the file has a three-character extension. 
	// Otherwise, it is an error. The extension can be retrieved
	// from 'xmlobj'. 
	//
	// This function does the following:
	//
	// 1. Rename the original file to:
	// 		/tmp/<dirname>/img.<ext>
	// 2. Assign a new seqno to the image
	// 3. Determine the image resolution
	// 4. Determine the file type
	// 5. Rename the original file based on the seqno and file type.
	// 6. Move the original file to the right place
	// 7. Add a request to the image converter
	// 8. Return a response that contains the image dimension
	//
	// 'xmlobj' should contain the following attributes:
	// 	<obj fname="xxx"
	// 		 dirname="xxx"
	// 		 ext="xxx"
	// 	/>
	//
	OmnString errmsg;

	OmnString fname, dimen, tnail;
	if(!retrieveImgFname(xmlobj, dirname, fname, fileext, errmsg))
	{
		rdata->setError() << errmsg;	
		return false;
	}
	OmnString objid = xmlobj->getAttrStr(AOSTAG_OBJID);
	bool flag = false;
	if (objid == "") 
	{
		objid = "image";
		flag = true;
	}
	OmnString objname = xmlobj->getAttrStr(AOSTAG_OBJNAME);
	if (objname == "")
	{
		objname = "image";
		flag = true;
	}

	//--Ketty
	//if (!uploadOneImagePriv(root, xmlobj, cmd, siteid, userid, 
	//		dirname, fname, fileext, errcode, errmsg)) return false;
	if (!uploadOneImagePriv(rdata, xmlobj, cmd, dirname, fname, fileext)) return false;

	// Remove the directory
	OmnString cc = "rm -rf ";
	cc << dirname;
	system(cc.data());

	// Construct the response
	contents = "<Contents ";
	contents << AOSTAG_TNAIL << "=\"" << xmlobj->getAttrStr(AOSTAG_TNAIL)
		<< "\" dimen=\"" << xmlobj->getAttrStr(AOSTAG_DIMENSION)
		<< "\" " << AOSTAG_OBJID << "=\"" << xmlobj->getAttrStr(AOSTAG_OBJID)
		<< "\" " << AOSTAG_DOCID << "=\"" << xmlobj->getAttrStr(AOSTAG_DOCID)
		<< "\"/>";
	rdata->setOk();
	return true;
}


bool
AosImgProc::uploadOneImagePriv(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &xmlobj,
		const AosXmlTagPtr &cmd, 
		const OmnString &dirname,
		const OmnString &fname,
		const OmnString &fileext)
{
	u32 siteid = rdata->getSiteid();
	u64 userid = rdata->getUserid();
	OmnString errmsg;
	// Phnix, 2011/01/25
	// const char *data = fname.data();
	// int flen = fname.length();
	// if (flen < 5 || data[flen-4] != '.')
	// {
	// 	if (errmsg != "") errmsg << ". ";
	// 	errmsg << "Not a valid image file, ignored: " << fname;
	// 	return true;
	// }
	// 
	// if (userid == AOS_INVDID)
	// {
	// 	errmsg = "User not logged in!";
	// 	return false;
	// }
	if (!xmlobj)
	{
		AosSetErrorU(rdata, "missing_imgobj") << enderr;
		return false;
	}

	OmnString hpctnr = xmlobj->getAttrStr(AOSTAG_PARENTC);
	if (hpctnr == "")
	{
		xmlobj->setAttr(AOSTAG_PARENTC, AOSCTNR_IMAGES);
	}

	//--Ketty
	//if (!AosSecurityMgrObj::getSecurityMgr()->checkLoadImage(siteid, 
	//			AOSAPPNAME_SYSTEM, hpctnr, userid, errcode, errmsg))
	if (!AosSecurityMgrObj::getSecurityMgr()->checkLoadImage(hpctnr, rdata))
	{
		// Access denied
		return false;
	}

	OmnString cid;
	if (userid == 0)
	{
		if (hpctnr == "")
		{
			AosSetErrorU(rdata, "no_container_spec") << enderr;
			return false;
		}
	}
	else
	{
		AosUserAcctObjPtr userdoc = AosDocClientObj::getDocClient()->getUserAcct(userid, rdata);
		if (!userdoc)
		{
			AosSetError(rdata, "user_not_found") << ": " << userid;
			return false;
		}

		cid = userdoc->getAttrStr(AOSTAG_CLOUDID);
		if (cid == "")
		{
			AosSetError(rdata, "missing_cloudid") << ": " << userid;
			return false;
		}
		// Chen Ding, 09/21/2011
		xmlobj->setAttr(AOSTAG_CLOUDID, cid);

		// hpctnr = userdoc->getAttrStr(AOSTAG_CTNR_IMAGE);
		// if (hpctnr == "")
		// {
		// 	hpctnr = AosObjid::composePrivImageCtnrObjid(cid);
		// 	xmlobj->setAttr(AOSTAG_PARENTC, hpctnr);
		// }
	}
	
	//OmnString fileext(&data[flen-3], 3);
	int imgw, imgh, maxsize, sizetype;
	char filetype;
	/*
	aos_assert_r(getImgResolution(dirname, fname, 
		imgw, imgh, maxsize, errmsg), false);
	getFileType(maxsize, filetype, sizetype);
	*/

	u64 seqno = getNextImageId(siteid, filetype, fileext);

	addOneImage(mCrtSeqno);

	// Move the file to the right place
	OmnString origfn;
	origfn << filetype << "o" << seqno << "." << fileext;
	OmnString mvcmd = "mv ";

	OmnString des_dirname = getImageDir(siteid);
	OmnString full_des_dirname = sgImageFullDir;
	full_des_dirname << "/" << des_dirname << "/";
	mvcmd << fname << " " << full_des_dirname << origfn;
	OmnScreen << "---------------- To move file: " << mvcmd << endl;
	system(mvcmd.data());

	// Chen Ding, 08/10/2010, ZKY-1022
	OmnString cpcmd = "cp ";
	cpcmd << full_des_dirname << origfn << " " << sgImgBackdir << "/";
	system(cpcmd.data());
	OmnScreen << "Backup image: " << cpcmd << endl;

	// Add a request to the image converter
	OmnString orig_fn = full_des_dirname;
	orig_fn << filetype << "o" << seqno << "." << fileext;

	/*
	aos_assert_r(mImgConverter, false);
	
	mImgConverter->addRequest(sizetype, filetype, orig_fn, seqno, 
		fileext, imgw, imgh, full_des_dirname, mCrtSeqno);
	*/

	OmnString tnail = des_dirname;
	tnail << "/" << filetype << "t" << seqno << "." << fileext;

	OmnString dimen;
	dimen << imgw << "x" << imgh;
	xmlobj->setAttr(AOSTAG_TNAIL, tnail);
	xmlobj->setAttr(AOSTAG_DIMENSION, dimen);

	OmnString docid;
	//AosXmlDocPtr header;

	// Set the directory name
	OmnString dd = sgImageFullDir;
	dd << "/" << des_dirname;
	// dd.trim(1);
	xmlobj->setAttr(AOSTAG_IMGDIR, dd);

	// Set the original file name
	OmnString ofname;
	ofname << filetype << "o" << seqno << "." << fileext;
	xmlobj->setAttr(AOSTAG_ORIG_FNAME, ofname);

	// Clean the object
	xmlobj->removeAttr("unique", false, true);
	xmlobj->removeAttr("dirname", false, true);

	// Ensure the objtype is AOSOTYPE_IMAGE
	xmlobj->setAttr(AOSTAG_OTYPE, AOSOTYPE_IMAGE);

	// Create the doc
	//--Ketty
	//AosDocServer::getSelf()->createDoc1(siteid, AOSAPPNAME_SYSTEM, 
	//		userid, root, cmd, cid, xmlobj, true, 0, 0, errcode, errmsg, false, ttl);
	AosDocClientObj::getDocClient()->createDoc1(rdata, cmd, cid, xmlobj, true, 0, 0, false);
	rdata->setOk();
	return true;
}


void
AosImgProc::getFileType(
		const int maxsize,
		char &filetype,
		int &sizeType)
{
	// Determine the original image filename. File names are encoded
	// as: 
	// 	a: t				t: 64
	// 	b: t, s				s: 128
	// 	c: t, s, m			c: 256
	// 	d: t, s, m, l		d: 512
	// 	e: t, s, m, l, h	e: 1024
	// 	o: only original
	//
	// 	Filename encoding:
	// 	Char 1: type (a, b, c, d, or e)
	//	Char 2: type 1 (t, s, m, l, h, o)
	//	Other Chars: a sequence number used as unique ID
	//
	/*
	if (maxsize >= AosImgConverter::eLargeSize) 
	{
		filetype = 'e';
		sizeType = eHugeIdx;
	}
	else if (maxsize >= AosImgConverter::eMediumSize) 
	{
		filetype = 'd';
		sizeType = eLargeIdx;
	}
	else if (maxsize >= AosImgConverter::eSmallSize) 
	{
		filetype = 'c';
		sizeType = eMediumIdx;
	}
	else if (maxsize >= AosImgConverter::eThumbnailSize) 
	{
		filetype = 'b';
		sizeType = eSmallIdx;
	}
	else 
	{
		filetype = 'a';
		sizeType = eThumbIdx;
	}
	*/
}


// This function creates an object for each file uploaded
// by an '.rar' file. The rar file is stored in:
// 		/tmp/<dirname>/<rar file>
// This function will first rename the rar file to avoid
// the multi-language problems. It is important that there 
// shall be one and only one file in the directory. 
//
//--Ketty
/*bool
AosImgProc::createImagesPriv(
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &xmlobj,
		const AosXmlTagPtr &cmd, 
		const u32 siteid, 
		const u64 &userid,
		const OmnString &dirname,
		const OmnString &fname,
		const OmnString &archiveType,
		OmnString &contents,
		AosXmlRc &errcode, 
		OmnString &errmsg)*/
bool
AosImgProc::createImagesPriv(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &xmlobj,
		const AosXmlTagPtr &cmd, 
		const OmnString &dirname,
		const OmnString &fname,
		const OmnString &archiveType,
		OmnString &contents)
{
	// 	<cmd dirname="xxx"
	// 		 zky_tags="xxx"
	// 		desc="xxx"
	// 		tname="xxx"
	// 		__name="xxx"
	//		__tnail="xxx"
	//		fname="xxx"/>
	//
	OmnString errmsg;

	std::list<OmnString> fnames;
	aos_assert_r(untarImages(dirname, archiveType, fnames, errmsg), false);
	aos_assert_r(processAiFile(xmlobj, errmsg), false);
	
	OmnString objid = xmlobj->getAttrStr(AOSTAG_OBJID);
	if (objid == "") objid = "image";

	OmnString objname = xmlobj->getAttrStr(AOSTAG_OBJNAME);
	if (objname == "") objname = "image";


	const int max_path_len = 20;
	OmnString parts[max_path_len];
	bool finished;
	std::list<OmnString>::iterator itr;
	const char *data = (const char *)xmlobj->getData();
	int size = xmlobj->getSize();
	AosXmlParser parser;
	int pase=0;
	for (itr = fnames.begin(); itr != fnames.end(); itr++)
	{
		pase++;
		if (pase % 20 == 0)
		{
			sleep(2);
		}
		AosXmlTagPtr root = parser.parse(data, size, "" AosMemoryCheckerArgs);
		AosXmlTagPtr doc = root->getFirstChild();

		OmnString fname = (*itr);
		OmnString ext = getFileExtension(fname);
		AosStrSplit split(fname.data(), "/", parts, max_path_len, finished);
		OmnString ff = parts[split.entries()-1];
		doc->setAttr(AOSTAG_ORIG_IMGFNAME, ff);
		//--Ketty
		//uploadOneImagePriv(root, doc, 
		//	cmd, siteid, userid, dirname, fname, ext, errcode, errmsg);
		uploadOneImagePriv(rdata, doc, 	cmd, dirname, fname, ext);
	}

	// Remove the directory 
	OmnString cc = "rm -rf ";
	cc << dirname;
	system(cc.data());
	rdata->setOk();
	return true;
}


bool
AosImgProc::processAiFile(
		const AosXmlTagPtr &xmlobj, 
		OmnString &errmsg)
{
	// If xmlobj contains the attribute:
	// 	zky_aifname
	// we need to create an entry for the AI file and set the dataid
	// to 'xmlobj'. 
	// AOSTAG_AIFNAME		"zky_aifname"
	// AOSTAG_AITNAILNAME	"zky_aitfname"
	/*
	OmnString aifname = xmlobj->getAttrStr(AOSTAG_AIDIR);
	OmnString aitfname = xmlobj->getAttrStr(AOSTAG_AITNAILNAME);
	xmlobj->removeAttr(AOSTAG_AIFNAME, false);
	xmlobj->removeattr(AOSTAG_AITNAILNAME, false);
	if (aifname == "") return true;

	// Save the tags since this function may modify it.
	OmnString tags = xmlobj->getAttrStr(AOSTAG_TAG);
	OmnString origTnail = xmlobj->getAttrStr(AOSTAG_TNAIL);
	OmnString origDataid = xmlobj->getAttrStr(AOSTAG_NAME);
	OmnString origObjtype = xmlobj->getAttrStr(AOSTAG_OTYPE);
	OmnString origDimen = xmlobj->getAttrStr(AOSTAG_DIMEN);
	OmnString origSubtype = xmlobj->getAttrStr(AOSTAG_SUBTYPE);

	// Check whether the AI file exists.
	OmnString aname = "/tmp/";
	aname << aifname;
	if (!OmnFile::fileExist(aname))
	{
		errmsg = "AI file does not exist: ";
		errmsg << aifname;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Check whether the thumbnail file exists
	aname = "/tmp/";
	aname << aitfname;
	if (!OmnFile::fileExist(aname))
	{
		errmsg = "AI Thumbnail file does not exist: ";
		errmsg << aitfname;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	const char *ddd = aitfname.data();
	int len = aitfname.length();
	if (aitfname.length() < 5 || ddd[len-4] != '.')
	{
		errmsg = "Invalid thumbnail filename: ";
		errmsg << aitfname;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Need to RAR the file
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	u64 seqno = store->getImageSeqno();
	OmnString rarcmd = "rar a ";
	rarcmd << sgImageDir << mDirname << "ai_" << seqno << ".rar /tmp/"
		<< aifname;
cout << "---------------- To RAR file: " << rarcmd << endl;
	system(rarcmd.data());

	OmnString rmcmd = "rm /tmp/";
	rmcmd << aifname;
	system(rmcmd.data());

	// Obtain the Vector Image ID 
	if (!sgImagesIdGen) sgImagesIdGen = OmnNew AosIdGen("imageid");
	OmnString vectorid = "zky_vec_";
	vectorid << sgImagesIdGen->getNextId();

	// Process the thumbnail. If the thumbnail name is in the
	// form: images_xxxx, it means an object has already been 
	// created for it. Otherwise, we need to create an object
	// for it. 
	AosStrSplit split(aitfname.data(), "_", 20);
	OmnString tnailDataid = aitfname;
	bool tnailStandalone = true;
	if (split.entries() <= 1 || split[0] != "images")
	{
		// Add the tag: zky_vectnail and the vector image ID
		mTags = tags;
		if (mTags != "") mTags << ",";
		mTags << "zky_vectnail," << vectorid;
		xmlobj->setattr(AOSTAG_TAG, mTags);
		OmnString fnnn = xmlobj->getAttrStr("fname");
		xmlobj->removeAttr("fname", false);
		OmnString fff = "/tmp/";
		fff << aitfname;
		procOneFile(fff, errmsg, -1);
		tnailDataid = mName;
		tnailStandalone = false;
		if (fnnn != "") xmlobj->setAttr("fname", fnnn);
	}
	else
	{
		// Need to retrieve the thumbnail filename. 
		OmnString stmt = "select tnail from images where dataid='";
		stmt << aitfname << "'";
		OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
		OmnDbRecordPtr record;
		OmnRslt rslt = store->query(stmt, record);
		if (!rslt || !record)
		{
			errmsg = "Failed to retrieve the thumbnail record: ";
			errmsg << aitfname;
			OmnAlarm << errmsg << enderr;
			return false;
		}
		mThumbnail = record->getStr(0, "", rslt);
	}

	if (!sgImagesTableIdGen) sgImagesTableIdGen = 
		OmnNew AosIdGen("imagestableid");
	OmnString id = sgImagesTableIdGen->getNextId();
	mName = mTablename;
	mName << "_" << id;

	aname = "ai_";
	aname << seqno << ".rar";
	OmnString otype = xmlobj->Attribute("__otype");
	mTags = tags;
	if (mTags != "") mTags << ",";
	mTags << "zky_vecimg," << vectorid;
	xmlobj->SetAttribute("__tags", mTags);
	xmlobj->SetAttribute("__tnail", mThumbnail);
	xmlobj->SetAttribute("zky_vecfname", aname);
	xmlobj->SetAttribute("objid", mName);
	xmlobj->SetAttribute("__otype", "VecImage");
	xmlobj->SetAttribute("__stype", "");
	xmlobj->SetAttribute("zky_vectndataid", tnailDataid);
	xmlobj->RemoveAttribute("__dimen");
	if (!tnailStandalone)
		xmlobj->SetAttribute("zky_vectnail_bd", "true");
	mObjType = "VecImage";
	mSubtype = "";

	// ostringstream oss(ostringstream::out);
	// oss << *xmlobj;
	// mXml = oss.str().c_str();
	// mXml.escapeSymbal('\'', '\'');

	aos_assert_r(saveTable("images", errmsg), false);

	if (mTags != "")
	{
		AosTag tagobj(mObjType, mName, mTags,
			mShortname, mThumbnail, mOrderName, mDesc, mUser, mXml);
		tagobj.addEntries();
	}

	mTags = tags;
	if (mTags != "") mTags << ",";
	mTags << vectorid << ",zky_from_vec";
	xmlobj->SetAttribute("__tags", mTags);
	xmlobj->SetAttribute("__otype", otype);
	xmlobj->SetAttribute("__stype", origSubtype);
	xmlobj->RemoveAttribute("zky_aifname");
	xmlobj->RemoveAttribute("zky_aitfname");
	xmlobj->RemoveAttribute("zky_vectndataid");
	xmlobj->RemoveAttribute("zky_vectnail_bd");
	mObjType = origObjtype;
	mSubtype = origSubtype;
	xmlobj->SetAttribute("zky_vecdataid", mName);
	xmlobj->SetAttribute("zky_vecid", vectorid);
	mThumbnail = origTnail;
	xmlobj->SetAttribute("__tnail", mThumbnail);
	xmlobj->SetAttribute("__otype", mObjType);
cout << __FILE__ << ":" << __LINE__ << " to set dimen: " << origDimen << endl;
	xmlobj->SetAttribute("__dimen", origDimen);

	mName = origDataid;
	xmlobj->SetAttribute("objid", mName);

	ostringstream oss1(ostringstream::out);
	oss1 << *xmlobj;
	mXml = oss1.str().c_str();
	mXml.escapeSymbal('\'', '\'');
	*/

	return true;
}


bool
AosImgProc::getImgResolution(
		const OmnString &dirname,
		const OmnString &fname, 
		int &imgw, 
		int &imgh, 
		int &maxsize,
		OmnString &errmsg)
{
	/*
	try
	{
		Magick::Image image(fname.data());
		imgh = image.rows();
		imgw = image.columns();
	}	
	catch(Magick::Exception &error)
	{
		errmsg = "This is not a image file!";
		OmnAlarm << errmsg << enderr;
		cerr << "Caught Magick++ exception: " << error.what() << endl;
		// Remove the original file
		OmnString cmd = "rm -f ";
		cmd << fname;
		system(cmd.data());

		return false;
	}
	maxsize = (imgw > imgh)?imgw:imgh;
	OmnString resolution;
	resolution << imgw << "x" << imgh;
	OmnScreen << "Image dimension: " << resolution << endl;
	*/
	return true;
}


bool
AosImgProc::retrieveImgFname(
		const AosXmlTagPtr &objdef,
		const OmnString &dirname,
		OmnString &fname, 
		const OmnString &fileext,
		OmnString &errmsg)
{
	// This function assumes that the uploaded file (there shall be
	// only one file) is stored in 'dirname'. This function renames
	// the file to 'image.fileext', where 'fileext' is passed in.
	fname = dirname;
	fname << "/image." << fileext;

	// Rename the file
	OmnString cmd = "mv ";
	cmd << dirname << "/* " << fname;
	system(cmd.data());
	return true;
}


bool
AosImgProc::untarImages(
		const OmnString &dirname,
		const OmnString &archiveType,
		std::list<OmnString> &fnames,
		OmnString &errmsg)
{
	// Untar the file and get all the filenames.
	// The tar file is stored in '/tmp/<dirname>/'.
	// There shall be one and only one '.rar' file
	// in the directory. This function will first rename
	// the rar file to a fixed one. 
	
	if (archiveType != "rar")
	{
		errmsg = "Unrecognized archive type: ";
		errmsg << archiveType;
		OmnAlarm << errmsg << enderr;
		return false;
	}
	
	OmnString cmd = "mv ";
	cmd << dirname << "/* " << dirname << "/images.rar";
	system(cmd.data());

	OmnString filename = dirname;
	filename << "/images.rar";
	cmd = "rar -y e ";
	cmd << filename << " -C " << dirname;
	system(cmd.data());

	// Remove the rar file
	cmd = "rm -f ";
	cmd << filename;
	system(cmd.data());

	OmnFile ff(AosMemoryCheckerArgsBegin);
	ff.getFiles(dirname, fnames, false);
	int size = fnames.size();

	//james escape special fname.
	/*
	std::list<OmnString>::iterator it;
	for (it = fnames.begin(); it!= fnames.end(); it++)
	{
	    it->escapeSymbols();
	}
	*/
	aos_assert_r(size > 0, false);
	return true;
}


u64
AosImgProc::getNextImageId(
		const u32 siteid,
		const char filetype, 
		const OmnString &fileext)
{
	// Chen Ding, 02/10/2012
	// Image IDs are managed locally here. 
	static u32 lsImageidSeqno = 0;
	sgIdLock.lock();
	if (sgImageId == 0)
	{
		sgImageId = OmnGetSecond();
		sgImageId = (sgImageId << 16);
	}
	u64 imageid = sgImageId + lsImageidSeqno++;
	sgIdLock.unlock();

	// All image files must have a file named: 
	// 	<filetype> + "o" + seqno + "." + fileextension. 
	OmnString fname = sgImageFullDir;
	fname << "/" << getImageDir(siteid) << "/" << filetype << "o";
	int guard = 100000;
	while (guard-- >= 0)
	{
		// Chen Ding, 02/10/2012
		// imageid = AosSeIdGenMgr::getSelf()->nextImageId();
		OmnString origfn = fname;
		origfn << imageid << "." << fileext;
		OmnScreen << "Verify image id: " << imageid << ":" << origfn << endl;
		if (!OmnFile::fileExist(origfn)) return imageid;
	
		lsImageidSeqno++;
		imageid++;
	}

	OmnAlarm << "Failed to verify image id: " << fname << ":" << imageid << enderr;
	return imageid;
}


void
AosImgProc::addOneImage(const int dirSeqno)
{
	sgLock.lock();
	sgNumFiles[dirSeqno]++;
	sgLock.unlock();
}

//--Ketty
/*bool
AosImgProc::getNewImageFname(
		const u32 siteid, 
		const u64 &userid,
		const bool pub_image,
		OmnString &fname,
		OmnString &imgdir,
		const int imagesize,
		const OmnString &fileext,
		const OmnString &stype,
		AosXmlRc &errcode, 
		OmnString &errmsg) */
bool
AosImgProc::getNewImageFname(
		const AosRundataPtr &rdata,
		const bool pub_image,
		OmnString &fname,
		OmnString &imgdir,
		const int imagesize,
		const OmnString &fileext,
		const OmnString &stype)
{
	// This function returns a new file name for an image in either 
	// the requester's image directory or in the public image directory
	// (if 'pub_image' is true).
	u32 siteid = rdata->getSiteid();
	u64 userid = rdata->getUserid();
	OmnString errmsg;

	if (pub_image)
	{
		//--Ketty
		//return getNewPubImageFname(siteid, userid, fname, imgdir, fileext, errcode, errmsg);
		return getNewPubImageFname(rdata, fname, imgdir, fileext);
	}

	imgdir << sgImageFullDir << "/";
	// 1. Retrieve the user's account.
	AosUserAcctObjPtr useracct = AosDocClientObj::getDocClient()->getUserAcct(userid, rdata);
	if (!useracct)
	{
		// The user account not exist
		AosSetErrorU(rdata, "failed_retrieve_usracct") << enderr;
		return false;
	}

	OmnString imagedir = useracct->getAttrStr(AOSTAG_USER_IMGDIR);
	if (imagedir == "")
	{
		AosSetErrorU(rdata, "user_doesnot_have_imgdir") << enderr;
		return false;
	}

	OmnString cid = useracct->getAttrStr(AOSTAG_CLOUDID);
	if (cid == "")
	{
		AosSetError(rdata, "missing_cloudid") << ": " << userid;
		return false;
	}

	u64 seqno = getNextImageId(siteid, 'o', fileext);
	fname = imagedir;
	fname << "/oo" << seqno << "." << fileext;

	OmnString docstr = "<image ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_IMAGE << "\" "
		<< AOSTAG_PARENTC << "=\"" << AOSTAG_CTNR_IMAGE << "\" "
		<< AOSTAG_IMGDIR << "=\"" << imagedir << "\" "
		<< AOSTAG_ORIG_FNAME << "=\"oo" << seqno << "." << fileext << "\" "
		<< "/>";

	// Create the doc
	//--Ketty
	//AosXmlTagPtr doc = AosDocServer::getSelf()->createDocSafe1(docstr, siteid, 
	//		userid, cid, AOSAPPNAME_SYSTEM, "", false, false, 
	//		errcode, errmsg, false, false, true, true, ttl);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, docstr, cid, "", false, false, 
			 false, false, true, true);
	rdata->setOk();
	return true;
}


//--Ketty
/*bool
AosImgProc::getNewPubImageFname(
		const u32 siteid, 
		const u64 &userid,
		OmnString &fname,
		OmnString &imgdir,
		const OmnString &fileext,
		AosXmlRc &errcode, 
		OmnString &errmsg) */
bool AosImgProc::getNewPubImageFname(
		const AosRundataPtr &rdata,	
		OmnString &fname,
		OmnString &imgdir,
		const OmnString &fileext)
{
	// This function returns a new file name for an image in either 
	// the requester's image directory or in the public image directory
	// (if 'pub_image' is true).
	u32 siteid = rdata->getSiteid();
	u64 userid = rdata->getUserid();
	OmnString errmsg;

	// 1. Retrieve the user's account.
	AosUserAcctObjPtr useracct = AosDocClientObj::getDocClient()->getUserAcct(userid, rdata);
	if (!useracct)
	{
		// The user account not exist
		AosSetErrorU(rdata, "failed_ret_user_acct") << userid << enderr;
		return false;
	}

	OmnString imagedir = useracct->getAttrStr(AOSTAG_USER_IMGDIR);
	if (imagedir == "")
	{
		AosSetErrorU(rdata, "missing_imgdir") << ": " << userid << enderr;
		return false;
	}

	OmnString cid = useracct->getAttrStr(AOSTAG_CLOUDID);
	if (cid == "")
	{
		AosSetError(rdata, "missing_cloudid") << ": " << userid;
		return false;
	}

	imgdir << sgImageFullDir << "/";
	u64 seqno = getNextImageId(siteid, 'o', fileext);
	fname = sgImageFullDir;
	fname << "/"<< imagedir;
	fname << "/oo" << seqno << "." << fileext;

	OmnString docstr = "<image ";
	OmnString ctnr = AosObjid::compose(AOSTAG_CTNR_IMAGE, cid);
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_IMAGE << "\" "
		<< AOSTAG_PARENTC << "=\"" << ctnr << "\" "
		<< AOSTAG_IMGDIR << "=\"" << imagedir << "\" "
		<< AOSTAG_ORIG_FNAME << "=\"oo" << seqno << "." << fileext << "\" "
		<< "/>";

	// Create the doc
	//--Ketty
	//AosXmlTagPtr doc = AosDocServer::getSelf()->createDocSafe1(docstr, siteid, 
	//		userid, cid, AOSAPPNAME_SYSTEM, "", false, false, 
	//		errcode, errmsg, false, false, true, true, ttl);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, docstr, cid, 
			"", false, false, false, false, true, true);
	rdata->setOk();
	return true;
}


OmnString
AosImgProc::getFileExtension(const OmnString &fname)
{
	if (fname != "" && fname.length() >= 5)
	{
		const char *data = fname.data();

		// find file ext
		OmnString ext;
		string fileexttmp(data);
		int fileextstart = fileexttmp.find_last_of(".");
		ext << fileexttmp.substr(fileextstart+1);
		ext.toLower();
		return ext;
	}

	return "";
}


// Chen Ding, 2011/02/28
OmnString 
AosImgProc::getImageFullDir()
{
	return sgImageFullDir;
}

