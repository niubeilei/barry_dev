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
#if 0
#include "SmartDoc/SdocThumbNail.h"

#include "AppMgr/App.h"
#include "Debug/Error.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "Rundata/Rundata.h"
#include "Security/Session.h"
#include "SEModules/ImgProc.h"
#include "Util/StrSplit.h"
#include "Util/Locale.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"

#include <algorithm>
#include <string>
#include <ImageMagick/Magick++.h>
using namespace Magick;

AosSdocThumbNail::AosSdocThumbNail(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_THUMBNAIL, AosSdocId::eThumbNail, flag)
{
}


AosSdocThumbNail::~AosSdocThumbNail()
{
}


bool
AosSdocThumbNail::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//
	// This feature creates a thumbnail automatically based on a smartdoc and an object. 
	// The thumb nail is created on the backend through the ImageMagic. 
	// It composes the thumbnail from one or more images. 
	//
	// The smartdoc is defined as:
	// <smartdoc tnailw="xxx" tnailh="xxx" public="true|false">
	// </smartdoc>
	//
	//
	// <dressroom obj_flag="dressup" zky_otype="dressroom" zky_unit="meiluo" gic_posneg="false" model_width="154" model_height="" zky_pctrs="abc">
	// <cloth dr_repid="shortskirt" dr_srcss="null"/>
	// <cloth dr_repid="trousers" dr_srcss="image_1/eo55148.png$$image_1/eo55149.png" dr_docid="173251"/>
	// <cloth dr_repid="boot" dr_srcss="null"/>
	// <cloth dr_repid="model" dr_srcss="image_1/eo55139.png$$image_1/eo55140.png" dr_docid="173235"/>
	// </dressroom>
	//

	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr root = rdata->getReceivedDoc();
	AosXmlTagPtr child;
	if (!root || !(child = root->getFirstChild()))
	{
		rdata->setError() << "Failed to parse the request!";
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	AosXmlTagPtr config= OmnApp::getAppConfig();
	if (!config)
	{
		rdata->setError() << "Can't Retrieve Config!";
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	// change dir to the ....images
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

	u64 userid = rdata->getUserid();
	if (userid == AOS_INVDID)
	{
		rdata->setError() << "Missing userid";
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	AosXmlTagPtr objdef = child->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError() << "Request incorrect!";
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	AosXmlTagPtr dressroom = objdef->getFirstChild();
	if (!dressroom)
	{
		rdata->setError() << "Failed to Get DressRoom doc!";
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	AosXmlTagPtr cloth = dressroom->getFirstChild("cloth");
	int layer = 20;
	int guard = eMaxImages;
	mImage.clear();
	OmnString model_width = dressroom->getAttrStr("model_width");
	OmnString model_height = dressroom->getAttrStr("model_height");
	double model_widthf = atof(model_width.data());
	double model_heightf = atof(model_height.data());
	
	while (cloth && guard--)
	{

		OmnString dr_srcss = cloth->getAttrStr("dr_srcss", "");
		if (dr_srcss != "" && dr_srcss != "null")
		{
			struct_image.layer = layer--;
			AosStrSplit split;
			OmnString parts[2];
			int nn = split.splitStr(dr_srcss, "$$", parts, 2);
			if(nn != 2)
			{
				parts[1] = "";
			}
			struct_image.path[0] = parts[0];
			struct_image.path[1] = parts[1];

			mImage.push_back(struct_image);
		}
		cloth = dressroom->getNextChild();
	}

	bool backend = dressroom->getAttrStr("gic_posneg")=="false" ? false : true;
	std::sort(mImage.begin(), mImage.end(), imgcmp);

	// composite pic ...
	OmnString pic_tmp;
	bool base_flag = true;
	Image *image_base = 0;
	int side = 0;
	if (backend)
	{
		side = 1;	
	}
	for (int i=0; i < (int)mImage.size(); i++)
	{
		//get the base image
		if (base_flag)
		{
			try
			{
				image_base = OmnNew Image(mImage[i].path[side].data());
			}
			catch(Magick::Exception &error)
			{
				rdata->setError() << "Can't open image file!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				cerr << "Caught Magick++ exception: " << error.what() << endl;
				return false;
			}
			OmnString size_base;
			if (model_width != "")
			{
				double ratiow = model_widthf / image_base->columns();
				OmnString ratioh = "100";
				if (model_height != "")
				{
					ratioh = "";
					double rh = model_heightf / image_base->rows();
					ratioh << (rh*100);
				}
				size_base << (ratiow * 100) << "x" << ratioh << "%";
				image_base->scale(size_base.data());
			}
			else 
			{
				size_base << image_base->columns() << "x" << image_base->rows();
				image_base->size(size_base.data());
			}

			base_flag = false;
			continue;
		}

		// get every over layer image
		OmnString src_tmp;
		src_tmp = mImage[i].path[side];

		Image *image_over_layer;
		try
		{
			image_over_layer = OmnNew Image(src_tmp.data());
		}
		catch(Magick::Exception &error)
		{
			rdata->setError() << "Can't open image file!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			cerr << "Caught Magick++ exception: " << error.what() << endl;
			return false;
		}

		OmnString gic_wh;

		if (model_width != "")
		{
			double ratiow = model_widthf / image_over_layer->columns();
			OmnString ratioh = "100";
			if (model_height != "")
			{
				ratioh = "";
				double rh = model_heightf / image_over_layer->rows();
				ratioh << (rh*100);
			}
			//gic_wh << (ratio * 100) << "x" << image_over_layer->rows() << "%";
			gic_wh << (ratiow * 100) << "x" << ratioh << "%";
			image_over_layer->scale(gic_wh.data());
		}
		else 
		{
			gic_wh << image_over_layer->columns() << "x" << image_over_layer->rows();
			image_over_layer->size(gic_wh.data());
		}

		Image tmp = *image_over_layer;

		image_base->composite(tmp, 0, 0, OverCompositeOp);
	}

	bool pub_image = true;
	if (sdoc->getAttrStr(AOSACTP_PUBLIC, "") == "true")
	{
		pub_image = true;
	}
	else
	{
		pub_image = false;
	}

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
	//AosXmlRc errcode;
	//OmnString errmsg;
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
		image_base->write(fname.data());
	}
	catch (Magick::Exception &error)
	{
		rdata->setError() << "Can't open image file!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		cerr << "Caught Magick++ exception: " << error.what() << endl;
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
	dressroom->setAttr(AOSTAG_ZKY_SLT, str3.data());

	OmnString docstr = dressroom->toString();

	OmnString cid = rdata->getCid();
	//--Ketty
	/*AosXmlTagPtr dressroomObj = AosDocServer::getSelf()->createDocSafe1(
			docstr, siteid, userid, cid, AOSAPPNAME_SYSTEM, "", 
			true, false, errcode, errmsg, false, false, false, true, ttl);*/
	AosXmlTagPtr dressroomObj = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr,cid, "", true, false, false, false, false, true);

	if (!dressroomObj)
	{
		rdata->setError() << "Can't Create dressroomObj!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString results;
	results << "<Contents>"<< dressroomObj->toString() << "</Contents>";
	rdata->setResults(results);

	rdata->setOk();
	return true;
}
#endif
