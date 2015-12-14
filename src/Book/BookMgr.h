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
#ifndef Omn_Book_BookMgr_h
#define Omn_Book_BookMgr_h

#include "Book/BookType.h"
#include "Book/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include <string.h>

class TiXmlElement;

class AosBookMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	
public:
	AosBookMgr();
	~AosBookMgr() {}

	static AosBookPtr getBook(const OmnString &bookdid); 
	bool	  	createBook(TiXmlElement *def,
					AosXmlRc &errcode, 
					OmnString &errmsg);

	bool	  	removeBook(TiXmlElement *def,
					AosXmlRc &errcode, 
					OmnString &errmsg);

	bool	  	createHomepage(const OmnString &vpd,
					const OmnString &bookid,
					AosXmlRc &errcode, 
					OmnString &errmsg);

	bool 		processReq(TiXmlElement *req, 
					AosXmlRc &errcode, 
					OmnString &errmsg);
};
#endif
