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
#if 0
#ifndef AOS_SearchEngine_DocMgr_h
#define AOS_SearchEngine_DocMgr_h

#include "DocClient/DocClient.h"
#include "DocUtil/DocLink.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include <map>
using namespace std;


OmnDefineSingletonClass(AosDocMgrSingleton,
						AosDocMgr,
						AosDocMgrSelf,
						OmnSingletonObjId::eDocMgr,
						"DocMgr");

class AosDocMgr : virtual public OmnRCObject 
{
	OmnDefineRCObject;

	friend class AosDocServer;

public:
	enum
	{
		eMaxNumDocs2Cache = 1 
	};

private:
	OmnMutexPtr					mLock;
	std::map<u64, AosDocLink*>	mDocMap;
	AosDocLink*					mDocHead;
	int							mDocNum;
	bool						mIsLocal;

	AosDocMgr();
	~AosDocMgr();

public:
    // Singleton class interface
    static AosDocMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool		start(const AosXmlTagPtr &config, const bool is_local);

	AosXmlTagPtr 	getDocByDocId(
						const OmnString &docId, 
						const OmnString &, 
						const AosRundataPtr &rdata);
	AosXmlTagPtr	getDoc(
						const u64 &docId, 
						const OmnString &, 
						const AosRundataPtr &rdata);
	AosXmlTagPtr 	getDoc(
						const OmnString &siteid, 
						const OmnString &objid, 
						const AosRundataPtr &rdata);
	AosXmlTagPtr 	getDocByCloudId(
						const OmnString &siteid,
						const OmnString &cloudid, 
						const AosRundataPtr &rdata); 
	AosXmlTagPtr	getUserAcct(const u64 &userid, const AosRundataPtr &rdata);
	AosXmlTagPtr	getUserAcct(const OmnString &userid, const AosRundataPtr &rdata)
					{
						u64 did = atoll(userid);
						return getUserAcct(did, rdata);
					}
	u64	 			getDocNum(){return mDocMap.size();};
	int	 			getNum(){return mDocNum;};
	AosXmlTagPtr	getLastDoc(){return mDocHead->p->doc;};
	bool			addCopy(const u64 &docId, const AosXmlTagPtr &doc);
	bool			deleteDoc(const u64 &docId);
	bool			addDoc(const AosXmlTagPtr &doc, 
						const u64 &docid);

	//bool			addDoc(const AosXmlTagPtr &doc, 
	//					const u64 &docid, 
	//					const bool lockflag = true);

	// Testing purpose only, Chen Ding, 2011/01/27
	AosDocLink *	getDocHead() const {return mDocHead;}
	
	AosXmlTagPtr 	getAccessRcd2(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &ref_doc,
						const u64 &ref_docid, 
						const OmnString &objid,
						const bool parent_flag,
						bool &is_parent);

	AosXmlTagPtr 	getParentAR3(
						const AosRundataPtr &rdata,
						const u64 &ref_docid, 
						const OmnString &ref_objid);


	//--Ketty
	/*static AosXmlTagPtr 	checkAccessRecord(
					const AosXmlTagPtr &acd, 
					const u64 &docid,
					const OmnString &siteid);*/
	static AosXmlTagPtr	checkAccessRecord(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &acd, 
				const u64 &docid);

private:
	bool			addNode(AosDocLink* node);
	bool			moveNode(AosDocLink* node, const bool rmMap);
	void			lockDocMgr() {mLock->lock();}
	void			unlockDocMgr() {mLock->unlock();}
	bool			addDocPriv(const AosXmlTagPtr &doc, 
						const u64 &docid);
};
#endif
#endif
