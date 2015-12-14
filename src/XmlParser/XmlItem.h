////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: XmlItem.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef XmlParser_XmlItem_h
#define XmlParser_XmlItem_h

#include "aosUtil/Types.h"
#include "Porting/LongTypes.h"
#include "XmlParser/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/BasicTypes.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"


class OmnXmlItem : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnXmlParserPtr		mParser;
	OmnString			mTag;

public:
	OmnXmlItem(const OmnString &data);
	OmnXmlItem(const OmnString &tag, const char *data, const unsigned int len);
	OmnXmlItem(const OmnString &tag,
		const uint valueStartIndex,
		const uint valueEndIndex,
		OmnString &data);
	OmnXmlItem(const OmnFilePtr &file, const char *moduleName);

	~OmnXmlItem();

	OmnString		getStr();
	bool			getBool();
	int				getInt();
	int64_t			getInt64();
	u64				getUint64();
	long			getLong();
	OmnIpAddr		getIpAddr();

	OmnString		getStr(const OmnString &name);
	bool			getBool(const OmnString &name);
	int				getInt(const OmnString &name);
	int64_t			getInt64(const OmnString &name);
	u64				getUint64(const OmnString &name);
	long			getLong(const OmnString &name);
	OmnIpAddr		getIpAddr(const OmnString &name);

	char			getChar(const OmnString &name, const char dflt);
	OmnString		getStr(const OmnString &name, const OmnString &dvalue);
	bool			getBool(const OmnString &name, const bool dvalue);
	int				getInt(const OmnString &name, const int dvalue);
	long			getLong(const OmnString &name, const long dvalue);
	OmnIpAddr		getIpAddr(const OmnString &name, const OmnIpAddr &dv);
	int64_t			getInt64(const OmnString &name, const int64_t &dv);
	u64				getUint64(const OmnString &name, const u64 &dv);

	OmnString		getTag() const {return mTag;}

	OmnXmlItemPtr	getItem(const OmnString &tag);
	OmnXmlItemPtr	tryItem(const OmnString &tag);
	bool			isAtomicItem() const;
	bool			hasMore();
	void			reset();
	OmnXmlItemPtr	next();
	OmnString		toString() const;
	int				entries() const;

	OmnXmlItemPtr	add(const OmnString &name);

	// 
	// Chen Ding, 02/19/2007
	// 
	bool			readFromFile(const OmnString &filename);
};

#ifndef OmnXmlGetStr
#define OmnXmlGetStr(var, item, name, dflt, rvalue) 	\
	var = item->getStr(name, dflt);						\
	if (var == dflt)									\
	{													\
		OmnAlarm << "Missing tag: " << item->getTag() 	\
			<< "in: " << item->toString() << enderr;	\
		return rvalue;									\
	}													
#endif

#ifndef OmnXmlGetInt
#define OmnXmlGetInt(var, item, name, dflt, rvalue) 	\
	var = item->getInt(name, dflt);						\
	if (var == dflt)									\
	{													\
		OmnAlarm << "Missing tag: " << item->getTag() 	\
			<< "in: " << item->toString() << enderr;	\
		return rvalue;									\
	}													
#endif

#endif
