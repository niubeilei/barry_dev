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
// 2011/02/15	Created by Phnix
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocComposerTN.h"
#if 0

#include "AppMgr/App.h"
#include "Debug/Error.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "Rundata/Rundata.h"
#include "Security/Session.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/ImgProc.h"
#include "Util/StrSplit.h"
#include "Util/Locale.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

#include <algorithm>
#include <string>
#include <ImageMagick/Magick++.h>
using namespace Magick;

AosSdocComposerTN::AosSdocComposerTN(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_COMPOSERTN, AosSdocId::eComposerTN, flag)
{
}


AosSdocComposerTN::~AosSdocComposerTN()
{
}


bool
AosSdocComposerTN::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// div format is 
	// <div style="position:relative;width:92;height:216;background:;">
	// 		<div style="background: none repeat scroll 0% 0% transparent; 
	// 				word-wrap: break-word; overflow: hidden; 
	// 				width: 50px; height: 50px; 
	// 				position: absolute; 
	// 				left: 39px; top: 93px; z-index: 0;">
	// 				<img style="width: 100%; height: 100%;" 
	// 					src="http://218.64.170.28:8800/lps-4.7.2/images/image_2/et9530.jpg">
	// 		</div>
	// </div>
	AosXmlTagPtr root = rdata->getReceivedDoc();
	if (!root)
	{
		rdata->setError() << "Failed to retrieve doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError() << "Failed to retrieve objdef tag in doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr sdressroom = objdef->getFirstChild("sdressroom");
	if (!sdressroom)
	{
		rdata->setError() << "Failed to retrieve sdressroom tag in doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosXmlTagPtr divContents = sdressroom->getFirstChild("cmpcontents");
	if (!divContents)
	{
		rdata->setError() << "Failed to retrieve sdressroom tag in doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString divStr = divContents->getNodeText();
	divStr = divStr.toLower();
	OmnScreen << "========" << divStr << "=====" << endl;
	//find <img
	//add / before >
	int pos = 0;
	if (divStr.findSubString("</img>", 0) != -1)
	{
		OmnScreen << "div is: " << divStr << endl;
	}
	else
	{
		while(pos != -1)
		{
			pos = divStr.findSubString("<img", pos);
			if (pos != -1)
			{
				pos = divStr.findSubString(">", pos);
				aos_assert_r(pos>0 && pos<divStr.length(), false);
				if (divStr.data()[pos-1] != '/') divStr.insert("/", pos);
			}
		}
	}
	AosXmlParser parser;		
	AosXmlTagPtr divXml = parser.parse(divStr, "" AosMemoryCheckerArgs);
	if (!divXml)
	{
		rdata->setError() << "div incorrect!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr child;
	child = divXml->getFirstChild();
	if (!child)
	{
		rdata->setError() << "Failed to parse the request!";
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	OmnString divInfo = child->getAttrStr("style");
	OmnString tmp;
	mImage.clear();
	int divWidth = findValue(divInfo, "width");
	int divHeight = findValue(divInfo, "height");
	Magick::Image imageBase(Geometry(divWidth, divHeight), Color("white"));
	imageBase.opacity(1);
	imageBase.fx("0", MatteChannel);

	AosXmlTagPtr imageXml = child->getFirstChild("div");
	int guard = eMaxImages;
	while (imageXml && guard--)
	{
		AosXmlTagPtr imgtag = imageXml->getFirstChild("img");

		if (imgtag)
		{
			OmnString divStyle = imageXml->getAttrStr("style");
			OmnString divStyle2 = imgtag->getAttrStr("style");
			struct_image.width = findValue(divStyle, "width");
			struct_image.height = findValue(divStyle, "height");
			struct_image.layer = findValue(divStyle, "z-index");
			struct_image.left = findValue(divStyle, "left") + findValue(divStyle2, "left");
			struct_image.top = findValue(divStyle, "top") + findValue(divStyle2, "top");


			OmnString imagesrc;
			imagesrc = imgtag->getAttrStr("src");
			int imagepos = imagesrc.findSubString("/images/", 0);
			imagesrc = imagesrc.substr(imagepos+strlen("/images/"));
			struct_image.src = imagesrc;
			mImage.push_back(struct_image);
		}
		else 
		{
			imageXml = child->getNextChild();
			continue;
		}
		imageXml = child->getNextChild();
	}

	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u64 userid = rdata->getUserid();
	if (userid == AOS_INVDID)
	{
		rdata->setError() << "Missing userid";
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	sort(mImage.begin(), mImage.end(), imgcmp);

	AosXmlTagPtr config= OmnApp::getAppConfig();
	if (!config)
	{
		rdata->setError() << "Can't Retrieve Config!";
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	AosXmlTagPtr cc = config->getFirstChild(AOSCONFIG_IMGCONFIG);
	aos_assert_r(cc, false);

	OmnString imagedir = cc->getAttrStr(AOSCONFIG_IMAGEDIR, "");
	if (imagedir.data()[imagedir.length()-1] == '/')
	{
		imagedir = imagedir.substr(0, imagedir.length()-1);
	}

	int chflag = chdir(imagedir.data());                                                        
	if (chflag < 0)
	{
		OmnString cmd;
		cmd << "mkdir -p " << imagedir;
		system(cmd.data());
		chflag = chdir(imagedir.data());
		if (chflag < 0)
		{
			rdata->setError() << "Failed to Access Dir!";
			return false;
		}
	}

	// composite pic ...
	//Image *image_base = 0;
	for (u32 i=0; i < mImage.size(); i++)
	{
		OmnString imagetmp;
		Magick::Image *image_tmp;
		try
		{
			image_tmp = OmnNew Magick::Image(mImage[i].src.data());
			OmnString samplesize;
			samplesize << mImage[i].width << "x" << mImage[i].height;
			image_tmp->sample(samplesize.data());
		}
		catch(Magick::Exception &error)
		{
			cerr << "Caught Magick++ exception: " << error.what() << endl;

			rdata->setError() << "Can't find the image!" << mImage[i].src;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		Magick::Image tmp = *image_tmp;
		imageBase.composite(tmp, mImage[i].left, mImage[i].top, OverCompositeOp);
	}

	bool pub_image = true;
	if (sdoc->getAttrStr(AOSACTP_PUBLIC, "") == "true")
		pub_image = true;
	else
		pub_image = false;

	u32 siteid = rdata->getSiteid();
	if (siteid == 0)
	{
		rdata->setError() << "siteid is empty!!!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString fname = "";
	int imagesize = 0;
	AosImgProc imgproc;
	OmnString imgdir;
	//--Ketty
	/*imgproc.getNewImageFname(
						siteid,
						userid,
						pub_image,
						fname,
						imgdir,
						imagesize,
						"png",
						"",
						errcode,
						errmsg);*/
	imgproc.getNewImageFname(rdata,	pub_image, fname, imgdir, imagesize, "png", "");


	// create dir 
	string ss(fname.data()) ;
	int index = ss.find_last_of('/');
	string str2 = ss.substr(0, index);
	string sstr = "mkdir -p " + str2;
	system(sstr.data());

	try
	{
		imageBase.write(fname.data());
		//image_base->write(fname.data());
	}
	catch (Magick::Exception &error)
	{
		cerr << "Caught Magick++ exception: " << error.what() << endl;

		rdata->setError() << "Can't save image!";
		OmnAlarm << rdata->getErrmsg() << enderr;

		return false;
	}

	string str3; 
	if (imgdir.length() < (int)ss.length())
	{
		str3 = ss.substr(imgdir.length());
	}
	else 
	{
		rdata->setError() << "Image Dir Is Wrong!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	sdressroom->setAttr(AOSTAG_ZKY_SLT, str3.data());

	OmnString docstr = sdressroom->toString();

	OmnString cid = rdata->getCid();
	//--Ketty
	//AosXmlTagPtr sdressroomObj = AosDocServer::getSelf()->createDocSafe1(
	//		docstr, siteid, userid, cid, AOSAPPNAME_SYSTEM, "", 
	//		true, false, errcode, errmsg, false, false, false, true, ttl);
	AosXmlTagPtr sdressroomObj = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, cid, "",	true, false, false, false, false, true);

	if (!sdressroomObj)
	{
		rdata->setError() << "Can't Create sdressroomObj!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString results;
	results << "<Contents>" << sdressroomObj->toString() << "</Contents>";
	rdata->setResults(results);
	return true;
}


int AosSdocComposerTN::findValue(const OmnString &src,
			const OmnString &findstr)
{
	int startpos = src.findSubString(findstr, 0);
	startpos = src.findSubString(":", startpos);
	int endpos = src.findSubString(";", startpos);
	OmnString tmp;
	tmp = src.substr(startpos+1, endpos);
	return atoi(tmp.data());
}
#endif
