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
// 	Created: 12/14/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Book_Book_h
#define Omn_Book_Book_h

#include "Book/BookType.h"
#include "Obj/ObjDb.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/XmlRc.h"
#include <string.h>


class TiXmlElement;

class AosBook : virtual public OmnDbObj
{
	OmnDefineRCObject;

	enum
	{
		eDefaultObjid = 100
	};

protected:
	// Following member data are stored in database
	OmnString			mBookId;		// Note: this is an auto-crement field
	char				mBookType;
	OmnString			mName;
	OmnString			mKeywords;
	OmnString			mDesc;
	OmnString			mXml;
	u32					mObjid;

	OmnString			mCrtAttrName;
	u32					mCrtDbId;
	u32					mCrtDbIdMax;
	u32					mBlockSize;

	OmnString			mTablename;

public:
	AosBook(const char type, const OmnString &tname); 
	virtual ~AosBook() {}

	// OmnObjDb Interface
	virtual OmnRslt		serializeFromDb();
	virtual OmnString	removeStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual bool		createBook(TiXmlElement *bookdef, 
							AosXmlRc &errcode, 
							OmnString &errmsg) = 0;

	OmnString	getBookId() const {return mBookId;}
	char		getBookType() const {return mBookType;}
	OmnString 	getName() const {return mName;}
	OmnString 	getKeywords() const {return mKeywords;}
	OmnString 	getDesc() const {return mDesc;}
	OmnString 	getXml() const {return mXml;}

	void		setName(const OmnString &name) {mName = name;}
	void		setDesc(const OmnString &k) {mDesc = k;}
	void		setKeywords(const OmnString &k) {mKeywords = k;}
	void		appendKeywords(const OmnString &k) {mKeywords += k;}
	void		setXml(const OmnString &d) {mXml = d;}
	bool		getBook(const OmnString &bookid);
	bool		addObjects(TiXmlElement *obj, 
					TiXmlElement *viewer, 
					TiXmlElement *editor, 
					AosXmlRc &errcode, 
					OmnString &errmsg);
	void		setTablename(const OmnString &t) {mTablename = t;}

	OmnString	getNewAttrName();
	OmnString	getObjid();
	OmnString	getNewBookId();
	bool		updateXml(
					const OmnString &vpd, 
					AosXmlRc &errcode, 
					OmnString &errmsg);
	bool		createHomepage(
					const OmnString &vpd, 
					AosXmlRc &errcode, 
					OmnString &errmsg);

private:
	bool		getDbId();
};

#endif

