////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: XmlParser.h
// Description:
//	This is a very simplified XML document parser. An XML document this
//  class accepts are in the following format:
//		<Tag>...</Tag> ... <Tag>...</Tag>
//
//	where <Tag>...</Tag> is an XmlItem. An XmlItem can contain XmlItems.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef XmlParser_XmlParser_h
#define XmlParser_XmlParser_h

#include "aosUtil/Types.h"
#include "Debug/Rslt.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"

class OmnXmlParser : public OmnRCObject
{
	OmnDefineRCObject;

private:
	//
	// Chen Ding, 09/19/2003, 2003-0262
	//
	enum
	{
		eMaxFieldLength = 5000
	};

	OmnString			mData;
	int					mCrtIndex;
	int					mLength;

public:
	OmnXmlParser(const OmnString &data);
	OmnXmlParser();
	~OmnXmlParser();

	// 
	// Chen Ding, 09/19/2003, 2003-0262
	//
	// OmnRslt			nextOpenTag(OmnString &tag, int &valueStartIndex);
	// OmnRslt			nextCloseTag(const OmnString &tag, int &valueEndIndex);
	OmnRslt			nextOpenTag(OmnString &tag, 
								int &valueStartIndex,
								int &fieldLength);
	OmnRslt			nextCloseTag(const OmnString &tag, 
								int &valueEndIndex,
								const int fieldLength);

	OmnXmlItemPtr	nextItem();
	bool			hasMore();
	OmnXmlItemPtr	getItem(const OmnString &tag);
	OmnXmlItemPtr	tryItem(const OmnString &tag);
	void			reset() {mCrtIndex = 0;}

	bool			isStringValue() const;
	bool			isAllDigits(const bool flag = true) const;
	OmnString		getContents() const {return mData;}
	OmnString		toString() const {return mData;}
	long			getLong() const;
	bool			getBool() const;
	int				getInt() const;
	int64_t			getInt64() const;
	u64				getUint64() const;
	void			setData(const OmnString &data);
	int				entries();

	// 
	// Chen Ding, 02/19/2007
	//
	bool			readFromFile(const OmnString &s);

private:
	void			removeComments();

};
#endif
