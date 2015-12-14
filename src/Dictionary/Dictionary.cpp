////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 12/04/2011 by Lynch 
////////////////////////////////////////////////////////////////////////////
#include "Dictionary/Dictionary.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"



AosDictionary::AosDictionary()
:
mLock(OmnNew OmnMutex()),
mDocNum(0)
{
	mDocHead = OmnNew AosDocLink;
}


AosDictionary::~AosDictionary()
{
	
	std::map<u64, AosDocLink*>::const_iterator itr = mDocMap.begin();
	for(; itr!=mDocMap.end(); ++itr)
	{
		delete itr->second;
	}
}


AosXmlTagPtr
AosDictionary::getTerm(const u64 &docId, const AosRundataPtr &rdata)
{
	//1 read doc from map
	//  if the doc exsit ,return doc ,put the doc to front of the link list 
	//  else retreive the doc from file, and put it to front of the link list
	//  if the link list if full, it will remove the end docs of the list 
	aos_assert_r(docId, NULL);
	mLock->lock();
//can not cache
/*	DocMapTypeItr itr = mDocMap.find(docId);
	if (itr != mDocMap.end())
	{
		// This means that there are entries that match 'docId'. 
		AosDocLink* node = itr->second;
		if (mDocNum != 1)
		{
			// Move the doc to the front of the list. This will 
			// ensure that the doc will not be removed from 
			// the cache in the near future.
			moveNode(node, false);
		}
		AosXmlTagPtr doc = node->doc;
		mLock->unlock();
		return doc;
	}
*/
	// Not in the cache. Load it from files.
//	OmnString objid = AOSZTG_DICTIONARYTERM;
//	objid << "_" << docId;
//OmnScreen << "-------:" << objid << endl;
//	AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByObjid(
//			rdata->getSiteid(), "", objid, true);
	AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByDocid(
			rdata->getSiteid(), rdata->getSsid() ,rdata->getUrldocDocid(),docId);
	if (!doc) 
	{
		// This can happen either the docid is invalid or 
		// the doc was deleted.
		mLock->unlock();
		rdata->setError() << "Failed retrieving the language entry: " << docId;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return NULL;
	}

	AosDocLink* node = 0;
	if(mDocNum < eMaxNumDocs2Cache)
	{
		node = OmnNew AosDocLink(docId, doc);
		addNode(node);
		mDocNum++;
	}
	else
	{
		node = mDocHead->p;
		moveNode(node, true);
		node->doc = doc;
		node->docId = docId;
	}

	mDocMap[docId] = node;
	mLock->unlock();
	return doc;
}


bool
AosDictionary::moveNode(AosDocLink* node, const bool rmMap)
{
	// If 'rmMap' is true, it removes the last entry in the
	// list. Otherwise, it simply moves the node.
	if (rmMap)
	{
		u64 mapKey = mDocHead->p->docId;
		mDocHead->p = node->p;
		node->p->n = mDocHead;
		mDocMap.erase(mapKey);
	}
	addNode(node);
	return true;
}


bool
AosDictionary::addNode(AosDocLink* node)
{
	// It adds the node 'node' to the front of the list. 
	if(mDocNum == 0)
	{
		// This is the first node to add:
		// mDocHead next to the node
		// mDocHead prev to the node
		// node->next to mDocHead
		// node->prev to mDocHead
		mDocHead->n = node;
		mDocHead->p = node;
		node->p = mDocHead;
		node->n = mDocHead;
	}
	else
	{
		// Insert it to the front:
		// node next to mDocHead next
		// node prev to mDocHead
		// mDocHead next's prev to node
		// mDocHead next to node

		// 1. Remove 'node' from the list.
		if (node->p) node->p->n = node->n;	
		if (node->n) node->n->p = node->p;	

		// 2. Insert 'node' right after 'mDocHead'
		node->p = mDocHead;
		node->n = mDocHead->n;

		// 3. Modify mDocHead so that its next's prev points to 'node'
		// and mDocHead points to node.
		mDocHead->n->p = node;
		mDocHead->n = node;
	}
	return true;
}
