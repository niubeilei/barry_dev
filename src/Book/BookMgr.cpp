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
#include "Book/BookMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Book/Webbook.h"
#include "Debug/Debug.h"
#include "TinyXml/tinyxml.h"

AosBookMgr::AosBookMgr()
{
}


AosBookPtr 
AosBookMgr::getBook(const OmnString &bookid)
{
	// Books are identified by 'bookid', which is in the form:
	// 	wbook_nnn	for webbook
	// 	sbook_nnn	for spreadsheets
	const char *data = bookid.data();
	aos_assert_r(bookid.length() > 0, 0);
	aos_assert_r(data, 0);

	// Currently we only support webbook
	AosBookPtr book = OmnNew AosWebbook();
	if (book->getBook(bookid)) return book;
	return 0;
}


bool
AosBookMgr::createBook(
		TiXmlElement *def, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// 	<bookdef
	// 		bookname="xxx"
	// 		keywords="xxx"		[optional]
	// 		description="xxx"	[optional]
	// 		booktype="x"		[optional, default to 'W']
	// 	</bookdef>
	
	errcode = eAosXmlInt_General;
	errmsg = "Internal error";

	aos_assert_r(def, false);

	// Retrieve the information
	const char * btype = def->Attribute("booktype");
	if (!btype || btype[0] == 'W')
	{
		AosWebbook book;
		return book.createBook(def, errcode, errmsg);
	}

	OmnAlarm << "Unrecognized book type: " << btype << enderr;
	errcode = eAosXmlInt_SyntaxError;
	errmsg = "Unrecognized book type: ";
	errmsg << btype;
	return false;
}


bool
AosBookMgr::removeBook(
		TiXmlElement *def, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// This function removes a book. This includes removing the entry 
	// in the book table, and the two tables for that book. 
	errcode = eAosXmlInt_General;
	errmsg = "Internal error";
	aos_assert_r(def, false);

	// Retrieve the information
	const char * btype = def->Attribute("booktype");
	if (!btype || btype[0] == 'W')
	{
		AosWebbook book;
		return book.removeBook(def, errcode, errmsg);
	}

	OmnAlarm << "Unrecognized book type: " << btype << enderr;
	errcode = eAosXmlInt_SyntaxError;
	errmsg = "Unrecognized book type: ";
	errmsg << btype;
	return false;	
}


bool
AosBookMgr::createHomepage(
		const OmnString &vpd, 
		const OmnString &bookid, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	errcode = eAosXmlInt_SyntaxError;
	errmsg = "internal error";

	// It creates a book home page, which is a VPD. The VPD is
	// defined in 'vpd'. The book is identified by 'bookid'. 
	
	AosBookPtr book = getBook(bookid);
	if (!book)
	{
		errmsg = "Failed to find the book: ";
		errmsg << bookid;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	return book->createHomepage(vpd, errcode, errmsg);
}


bool
AosBookMgr::processReq(TiXmlElement *req, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	OmnNotImplementedYet;
	return false;
}

