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
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocMgr_DocMgr_h
#define AOS_DocMgr_DocMgr_h

#include "SEInterfaces/DocMgrObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include <hash_map>
using namespace std;


OmnDefineSingletonClass(AosDocMgr1Singleton,
						AosDocMgr1,
						AosDocMgr1Self,
						OmnSingletonObjId::eDocMgr,
						"DocMgr");
struct AosDocKey
{
	u64 docid;
	u32 siteid;
	AosDocKey(const u64 did, const u32 sid)
	:
	docid(did),
	siteid(sid)
	{
	}
};

struct AosDocKeyHash
{
	inline size_t operator()(const AosDocKey &rhs) const
	{
		return rhs.docid;
	}
};

struct AosDocKeyCompare
{
	bool operator ()(const AosDocKey &lhs, const AosDocKey &rhs) const
	{
		return lhs.siteid == rhs.siteid && lhs.docid == rhs.docid;
	}
};

class AosDocMgr1 : virtual public AosDocMgrObj
{
	OmnDefineRCObject;

	friend class AosDocServer;

public:
	enum
	{
		eMaxNumDocs2Cache = 1000
	};

private:
	typedef hash_map<const AosDocKey, AosXmlTagPtr, AosDocKeyHash, AosDocKeyCompare> map_t;
	typedef hash_map<const AosDocKey, AosXmlTagPtr, AosDocKeyHash, AosDocKeyCompare>::iterator mapitr_t;

	OmnMutexPtr		mLock;
	map_t			mDocMap;
	AosXmlTagPtr	mDocHead;
	u32				mNumDocsCache; 

	AosDocMgr1();
	~AosDocMgr1();

public:
    // Singleton class interface
    static AosDocMgr1* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	virtual bool addCopy(
						const u64 &docid, 
						const AosXmlTagPtr &doc,
						const AosRundataPtr &rdata);
	virtual bool addDoc(const AosXmlTagPtr &doc, 
						const u64 &docid,
						const AosRundataPtr &rdata);
	virtual bool deleteDoc(
						const u64 &docid,
						const AosRundataPtr &rdata);
	virtual AosXmlTagPtr getDocByDocid(
						const OmnString &docid,
						const AosRundataPtr &rdata);
	virtual AosXmlTagPtr getDocByDocid(
						const u64 &docid,
						const AosRundataPtr &rdata);
	virtual OmnString	getLogs();

	// Testing purpose only, Chen Ding, 2011/01/27
	u32	 			getNumCached(){return mDocMap.size();};
	AosXmlTagPtr	getLastDoc();
	AosXmlTagPtr	getDocHead() const;
	u32				getCacheSize() const {return mNumDocsCache;}
	bool			sanityCheck(const bool lock = true);
	
private:
	bool			removeNodeLocked(const AosXmlTagPtr &node);
	bool			moveNodeLocked(const AosXmlTagPtr &node);
};
#endif

