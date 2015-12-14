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
#ifndef Omn_Book_Webbook_h
#define Omn_Book_Webbook_h

#include "Book/Book.h"


class AosWebbook : public AosBook
{

private:

public:
	AosWebbook();
	virtual ~AosWebbook();

	virtual bool		createBook(TiXmlElement *bookdef, 
							AosXmlRc &errcode, 
							OmnString &errmsg);
	
	virtual bool		removeBook(TiXmlElement *bookdef, 
							AosXmlRc &errcode, 
							OmnString &errmsg);
	
	// OmnObjDb Interface
	virtual OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	insertStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	existStmt() const;
	virtual int			objSize() const {return sizeof(*this);}
};

#endif

