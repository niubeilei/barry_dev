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
// Created: 13/04/2011 by Lynch
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Dictionary_Dictionary_h
#define AOS_Dictionary_Dictionary_h

#include "DocUtil/DocLink.h"
#include "Rundata/Rundata.h"
#include "Util/String.h"
#include "Util/Locale.h"
#include "XmlInterface/XmlRc.h"
#include <map>
using namespace std;


class AosDictionary : virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxNumDocs2Cache = 1000
	};

	typedef std::map<u64, AosDocLink*> 			 DocMapType;
	typedef std::map<u64, AosDocLink*>::iterator DocMapTypeItr;

private:
	OmnMutexPtr		mLock;
	DocMapType		mDocMap;
	AosDocLink*		mDocHead;
	int				mDocNum;

public:
	AosDictionary();
	~AosDictionary();

	AosXmlTagPtr	getTerm(const u64 &docId, const AosRundataPtr &rdata);

private:
	bool			addNode(AosDocLink* node);
	bool			moveNode(AosDocLink* node, const bool rmMap);
};
#endif

