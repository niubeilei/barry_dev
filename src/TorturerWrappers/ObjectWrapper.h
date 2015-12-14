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
// 05/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerWrappers_ObjectWrapper_h
#define Omn_TorturerWrappers_ObjectWrapper_h

#include "Obj/ObjDb.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "TorturerWrappers/Ptrs.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/Ptrs.h"
#include <string>

class TiXmlNode;
class TiXmlElement;

class AosObjectWrapper : virtual public OmnDbObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eThumbIdx 	= 2,
		eSmallIdx 	= 3,
		eMediumIdx 	= 4,
		eLargeIdx 	= 5,
		eHugeIdx 	= 6,

		eMaxParentContainers = 1
	};

private:
	OmnString 			mName;
	OmnString			mTags;
	OmnString 			mDesc;
	OmnString 			mXml;
	OmnString			mTablename;
	OmnString			mShortname;
	OmnString			mThumbnail;
	OmnString			mObjType;
	OmnString			mOrderName;
	OmnString			mOperator;
	OmnString			mUser;
	OmnString			mSiteId;
	OmnString			mDirname;
	OmnString			mImageDir;
	OmnString			mContainer;
	OmnString			mVVPD;
	OmnString			mEVPD;
	OmnString			mCreator;
	OmnString			mSubtype;
	TiXmlElement *		mXmlObj;
	static AosImgConverterPtr	mImgConverter;

public:
	AosObjectWrapper();
	~AosObjectWrapper();

	// OmnObjDb Interface
	virtual OmnRslt		serializeFromDb();
	virtual OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString	insertStmt() const;
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual OmnString	existStmt() const;
	virtual int			objSize() const {return sizeof(*this);}

	OmnString getName() const {return mName;}
	OmnString getDesc() const {return mDesc;}
	OmnString getXml() const {return mXml;}

	void		setName(const OmnString &name) {mName = name;}
	void		setUsername(const OmnString &name) {mUser = name;}
	void		setTablename(const OmnString &name) {mTablename = name;}
	void		setDesc(const OmnString &d) {mDesc = d;}
	void		setXml(const OmnString &d) {mXml = d;}
	void		setXml(TiXmlNode *);
	bool		getObject(const OmnString &name, OmnString &errmsg);
	bool		objectExist();
	static bool	objectExist(const OmnString &name);
	static void	setImgConverter(const AosImgConverterPtr &c);

	bool 		modifyObj(
					TiXmlElement *objdef,
					OmnString &contents,
					AosXmlRc &errcode, 
					OmnString &errmsg);

	bool 		getObject(
					const OmnString &tname,
					const OmnString &objid_fname,
					const OmnString &objid, 
					const char *ffs,
					OmnString &contents,
					OmnString &errmsg);

	bool 		createCtnrObj(
					TiXmlNode *root,
					TiXmlElement *cmd,
					OmnString &contents,
					AosXmlRc &errcode, 
					OmnString &errmsg);

	bool 		createObj(
					TiXmlNode *child, 
					TiXmlElement *childelem,
					AosXmlRc &errcode, 
					OmnString &errmsg);
	bool 		delObj(
					TiXmlNode *cmd, 
					AosXmlRc &errcode, 
					OmnString &errmsg);
	bool 		uploadImage(
					const OmnString &orig_fname,
					TiXmlElement *objdef_elem,
					AosXmlRc &errcode, 
					OmnString &errmsg);
	bool 		convertFile(
					const bool removeFlag,
					const char filetype, 
					const OmnString &orig_fn,
					const char type,
					const u64 seqno, 
					const OmnString &fileext, 
					const int ww, 
					const int hh, 
					const int fsize);

	void 		renameFile(
					const char fileCat, 
					const char filetype, 
					const char tt, 
					const u64 seqno, 
					const char *fileext);

	void 		removeFile(
					const char fileCat, 
					const char tt, 
					const u64 seqno, 
					const char *fileext);
	bool 		removeAllImageFiles(const OmnString &tn);
	bool 		saveTable(const OmnString &tname, OmnString &errmsg);
	OmnString	getImageDir();
	bool 		uploadOneImage(
					const OmnString &orig_fn);
	bool 		createImages(
					const OmnString &archiveType,
					OmnString &contents,
					AosXmlRc &errcode, 
					OmnString &errmsg);
	bool 		procOneFile(
					const OmnString &fname, 
					OmnString &errmsg, 
					const int idx);
	void		getFileLength(const int maxside, char &filetype, int &maxsize);
	bool 		fromImgToImages();
	int			checkTags(OmnString &tags);
	bool 		toImages(
					const OmnString &tname, 
					const OmnString &query, 
					const OmnString &container,
					const OmnString &creator,
					const OmnString &imgtype);
	bool 		processAiFile(OmnString &errmsg);
	bool 		modifyTable(const OmnString &oldversion);
	bool 		updateCounter(
					const OmnString &counter_name,
					const OmnString &dataid,
					const int delta,
					OmnString &errmsg);
	OmnString 	getTablename(
					const OmnString &dataid, 
					OmnString &idfname);
	AosXmlTagPtr retrieveXml(
					const OmnString &tname, 
					const OmnString &idfname,
					const OmnString &dataid,
					OmnString &errmsg);
	bool 		updateXml(
					const AosXmlTagPtr &xml,
					const OmnString &idfname, 
					const OmnString &tname,
					const OmnString &dataid,
					OmnString &errmsg);
	bool 		addLoginEntry(
					const OmnString &username, 
					const OmnString &dataid,
					const AosXmlTagPtr &xml,
					OmnString &contents,
					const OmnRslt &rslt, 
					const OmnString &errmsg);
	bool 		addDataidTag(
					const OmnString &tag,
					const OmnString &dataid, 
					OmnString &errmsg, 
					const OmnString &msg);
	bool 		createComment(
					TiXmlElement *cmd,
					OmnString &contents,
					OmnString &errmsg,
					AosXmlRc &errcode); 
	bool 		checkNotEmpty(
					const OmnString &data, 
					OmnString &errmsg, 
					const OmnString &msg);
	OmnString 	incrementVersion(const OmnString &version);
	bool 		addVersionObj(
					const OmnString &keyfname, 
					const OmnString &oldversion);
};

#endif

