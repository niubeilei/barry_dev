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
// Created: 08/03/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Book_Tag_h
#define Omn_Book_Tag_h

#include "Book/SystemData.h"
#include "Obj/ObjDb.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/XmlRc.h"
#include <string.h>


class AosSystemData;

class AosTag : virtual public AosSystemData
{
	OmnDefineRCObject;

protected:
	OmnString	mType;
	OmnString 	mDataid;
	OmnString 	mTags;
	OmnString 	mTag;
	OmnString 	mName;
	OmnString 	mThumbnail;
	OmnString 	mOrderName;
	OmnString 	mDesc;
	OmnString 	mCreator ;
	u32			mCreateTime;
	OmnString	mXml;

public:
	AosTag(); 
	AosTag(const OmnString &type,
		const OmnString &dataid, 
		const OmnString &tags,
		const OmnString &name, 
		const OmnString &thumbnail, 
		const OmnString &ordername, 
		const OmnString &desc,
		const OmnString &user, 
		const OmnString &xml);
	virtual ~AosTag() {}

	// OmnObjDb Interface
	virtual OmnString 	insertStmt() const;
	static bool 	cleanTagTable();

	bool	getEntry(const OmnString &tag, const OmnString &datid);
	bool	addEntries();
	bool	addEntries(char **tags, const int num, 
				const AosSystemData &data);
	bool	deleteEntries(const OmnString &);
	bool 	addOneEntry(
				const OmnString &tag,
				const OmnString &dataid,
				const OmnString &objtype,
				const OmnString &xmlvalue);
};

#endif

