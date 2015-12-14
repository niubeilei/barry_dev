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
// 01/30/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEModules_ImgProc_h
#define Aos_SEModules_ImgProc_h

#include "Database/Ptrs.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEServer/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"

#include <list>


class AosImgProc : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxFiles = 10000,
		eMaxDirs = 500,
		eDftMaxFiles = 500,
		eDftBlocksize = 10,
		eThumbIdx 	= 2,
		eSmallIdx 	= 3,
		eMediumIdx 	= 4,
		eLargeIdx 	= 5,
		eHugeIdx 	= 6,

		eMaxParentContainers = 1
	};

	OmnString			mCrtDirname;
	// OmnString			mImageDir;

private:
	//static AosImgConverterPtr	mImgConverter;
	static u64					mNewId;
	static int 					mCrtSeqno;

public:
	AosImgProc();
	~AosImgProc();

	bool		stop();
	//--Ketty
	/*bool		uploadImage(
					const AosXmlTagPtr &root,
					const AosXmlTagPtr &doc,
					const AosXmlTagPtr &cmd, 
					const u32 siteid, 
					const u64 &userid,
					OmnString &contents,
					AosXmlRc &errcode, 
					OmnString &errmsg);*/
	bool	uploadImage(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &objdef,
				const AosXmlTagPtr &cmd,
				OmnString &contents);

	//void 		setImgConverter(const AosImgConverterPtr &c);
	static bool staticInit(const AosXmlTagPtr &config);
	static OmnString getImgDir();

	//--Ketty
	/*bool getNewImageFname(
		const u32 siteid, 
		const u64 &userid,
		const bool pub_image,
		OmnString &fname,
		OmnString &imgdir,
		const int imagesize,
		const OmnString &fileext,
		const OmnString &stype,
		AosXmlRc &errcode, 
		OmnString &errmsg); */
	bool	getNewImageFname(
				const AosRundataPtr &rdata,
				const bool pub_image,
				OmnString &fname,
				OmnString &imgdir,
				const int imagesize,
				const OmnString &fileext,
				const OmnString &stype);

	static void addOneImage(const int dirSeqno);

	// Chen Ding, 2011/02/28
	static OmnString getImageFullDir();

private:
	//--Ketty
	/*bool 		createImagesPriv(
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
					OmnString &errmsg);*/
	bool	createImagesPriv(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &xmlobj,
				const AosXmlTagPtr &cmd, 
				const OmnString &dirname,
				const OmnString &fname,
				const OmnString &archiveType,
				OmnString &contents);

	//--Ketty
	/*bool		uploadOneImagePriv(
					const AosXmlTagPtr &root,
					const AosXmlTagPtr &xmlobj,
					const AosXmlTagPtr &cmd, 
					const u32 siteid, 
					const u64 &userid,
					const OmnString &dirname,
					const OmnString &fname,
					const OmnString &fileext,
					AosXmlRc &errcode, 
					OmnString &errmsg);*/ 
	bool	uploadOneImagePriv(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &xmlobj,
			const AosXmlTagPtr &cmd, 
			const OmnString &dirname,
			const OmnString &fname,
			const OmnString &fileext);
	
	static void getFileType(
					const int maxsize,
					char &filetype,
					int &sizeType);
	bool 		processAiFile(
					const AosXmlTagPtr &xmlobj, 
					OmnString &errmsg);
	static bool getImgResolution(
					const OmnString &dirname,
					const OmnString &fname, 
					int &imgw, 
					int &imgh, 
					int &maxsize,
					OmnString &errmsg);
	static bool retrieveImgFname(
					const AosXmlTagPtr &objdef,
					const OmnString &dirname,
					OmnString &fname, 
					const OmnString &ext,
					OmnString &errmsg);
	bool 		retrieveImgSeqno(u64 &seqno);
	bool 		untarImages(
					const OmnString &dirname,
					const OmnString &archiveType,
					std::list<OmnString> &fnames,
					OmnString &errmsg);
	
	//--Ketty
	/*bool		uploadImagePriv(
					const AosXmlTagPtr &root,
					const AosXmlTagPtr &xmlobj,
					const AosXmlTagPtr &cmd,
					const u32 siteid,
					const u64 &userid,
					const OmnString &dirname,
					const OmnString &fileext,
					OmnString &contents,
					AosXmlRc &errcode, 
					OmnString &errmsg);*/
	bool	uploadImagePriv(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &xmlobj,
			const AosXmlTagPtr &cmd, 
			const OmnString &dirname,
			const OmnString &fileext,
			OmnString &contents);

	OmnString 	getImageDir(const u32 siteid);
	OmnString 	getNextDirname(const u32 siteid);
	AosXmlTagPtr retrieveDirdoc(const u32 siteid);
	AosXmlTagPtr getImgSysdoc();
	u64 getNextImageId(
		const u32 siteid,
		const char filetype, 
		const OmnString &fileext);

	//--Ketty
	/*
	bool getNewPubImageFname(
		const u32 siteid, 
		const u64 &userid,
		OmnString &fname,
		OmnString &imgdir,
		const OmnString &fileext,
		AosXmlRc &errcode, 
		OmnString &errmsg);*/
	bool getNewPubImageFname(
			const AosRundataPtr &rdata,	
			OmnString &fname,
			OmnString &imgdir,
			const OmnString &fileext);

	OmnString	getFileExtension(const OmnString &fname);
};
#endif

