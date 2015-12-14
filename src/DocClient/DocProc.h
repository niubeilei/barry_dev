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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocClient_DocProc_h
#define AOS_DocClient_DocProc_h

#include "DocUtil/DocUtil.h"
#include "DocUtil/DocProcUtil.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
//#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "SEUtil/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/DocReq.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThreadMgr.h"
#include "Thread/CondVar.h"
#include "Util/RCObject.h"
#include "WordParser/Ptrs.h"


OmnDefineSingletonClass(AosDocProcSingleton,
						AosDocProc,
						AosDocProcSelf,
						OmnSingletonObjId::eDocProc,
						"DocProc");

class AosDocProc : virtual public OmnThreadedObj
{
	OmnDefineRCObject;
	friend class AosSeSysLog;
	friend class AosDocMgr;

private:
	enum
	{
		eDftMaxWordLen = 90,
		eMaxSortedLists = 30,			// Chen Ding, 2011/01/18
		eWordFlag_Container = 3,
		eIgnoredWordSize = 1023,
		eMaxThreads = 20,				// Chen Ding, 09/30/2011
		eMaxDocTransId = 1000, 
		eBuffSize = 100000, 
		eBuffIncSize = 10000,
		eNumDocTransIds = 1000,
		eDftNumThreads = 5,
		eProcTimer = 10,
		eMaxWordsPerDoc = 1000000
	};

	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	int					mNumReqs;

	AosDocProcUtil::AosSeWordAttrHash	mAttrWordHash[eMaxThreads];
	AosDocProcUtil::AosSeWordHash		mWordHashNew[eMaxThreads];
	AosDocProcUtil::AosSeAttrHash		mAttrHashNew[eMaxThreads];
	AosDocProcUtil::AosSeWordHash		mWordHashOld[eMaxThreads];
	AosDocProcUtil::AosSeAttrHash		mAttrHashOld[eMaxThreads];
	set<OmnString> 		mMemberOfNew[eMaxThreads];
	set<OmnString> 		mMemberOfOld[eMaxThreads];

	AosDocReqPtr		mHead;
	AosDocReqPtr		mTail;
	AosWordNormPtr      mWordNorm[eMaxThreads];
	OmnString           mWordNormName;
	int					mMaxWordLen;
	bool				mIsRepairing;

	u32					mTotalAccepted;
	u32					mTotalProcessed;
	AosWordParserPtr	mWordParser[eMaxThreads];			// Chen Ding, 10/11/2010
	AosWordIdHashPtr	mIgnoredWords[eMaxThreads];

	static bool			smShowLog;

	// Chen Ding, 09/30/2011
	OmnThreadPtr		mThreads[eMaxThreads];
	u64					mProcessedDocids[eMaxThreads];
	OmnString			mCreatedCtnrs[eMaxThreads];
	static int			smMaxOutstandingMsgs;
	int					mNumThreads;

	// Chen Ding, 02/15/2014
	static AosIILClientObjPtr	smIILClient;

	//felicia, 2013/06/19
	bool				mStatus[eMaxThreads];
public:
	AosDocProc();
	~AosDocProc();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, 
						const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread111(OmnString &err, const int thrdLogicId);
    //virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

    // Singleton class interface
    static AosDocProc *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	AosDocReqPtr addDeleteRequest(
			const AosXmlTagPtr &docroot,
			const AosXmlTagPtr &doc,
			const u64 userid,
			const OmnString &appname, 
			const AosRundataPtr &rdata);
			//u64 &doc_transid);
			//const AosTaskTransPtr &task_trans);

	//AosDocReqPtr addDeleteLogRequest(
	bool addDeleteLogRequest(
			const AosXmlTagPtr &doc,
			const u64 userid,
			const AosRundataPtr &rdata);

	AosDocReqPtr addModifyRequest(
			const AosXmlTagPtr &root,
			const u64 &userid,
			const AosXmlTagPtr &doc,
			const AosXmlTagPtr &origdoc, 
			const bool synobj, 
			const AosRundataPtr &rdata);
			//u64 &doc_transid);
			//const AosTaskTransPtr &task_trans);

	AosDocReqPtr addCreateRequest(
			const AosXmlTagPtr &root,
			const u64 &userid,
			const AosXmlTagPtr &doc,
			const AosDocSvrCbObjPtr &caller, 
			void *userdata, 
			const AosRundataPtr &rdata);
			//u64 &doc_transid);
			//const AosTaskTransPtr &task_trans);

	//AosDocReqPtr addCreateLogRequest(
	bool addCreateLogRequest(
			const u64 &userid,
			const AosXmlTagPtr &doc,
			const AosRundataPtr &rdata);
			//const AosTaskTransPtr &task_trans);
	
	//bool addModifyAttrRequest(
	//		const OmnString &aname,
	//		const u64 &oldvalue,
	//		const u64 &newvalue, 
	//		const bool exist,
	//		const bool value_unique,
	//		const bool docid_unique,
	//		const u64 &docid, 
	//		const AosRundataPtr &rdata);

	AosDocReqPtr  addModifyAttrRequest(
			const OmnString &aname,
			const OmnString &oldvalue,
			const OmnString &newvalue, 
			const bool exist,
			const bool value_unique,
			const bool docid_unique,
			const u64 &docid, 
			const AosRundataPtr &rdata);
			//u64 &doc_transid);
			//const AosTaskTransPtr &task_trans);
	
	OmnString	getWordIILName(
			const AosEntryMark::E mark,
			const OmnString &ctnr_objid,
			const OmnString &word);

	OmnString	getAttrIILName(
			const AosEntryMark::E mark,
			const OmnString &ctnr_objid,
			const OmnString &name);

	OmnString	getAttrWordIILName(
			const AosEntryMark::E mark,
			const OmnString &ctnr_objid,
			const OmnString &attrname,
			const OmnString &word);
	//u64 getDocTransId(){return createDocTransid();};

	// Chen Ding, 12/26/2011
	// static bool removeMetaAttrs(const AosXmlTagPtr &doc);

	//bool addAddMemberRequest(
	//		const AosXmlTagPtr &doc,
	//		const OmnString &ctnr_objidconst, 
	//		const u64 &userid, 
	//		const AosRundataPtr &rdata);

	//bool addRemoveMemberRequest(
	//		const AosXmlTagPtr &doc, 
	//		const OmnString &ctnr_objid, 
	//		const AosRundataPtr &rdata);

	//static bool addContainerMember(
	//		const AosRundataPtr &rdata,
	//		const OmnString &ctnr_objid,
	//		const AosXmlTagPtr &doc,
	//		const OmnString &objid,
	//		const u64 &docid);

	//static bool removeContainerMember(
	//		const OmnString &ctnr_objid,
	//		const AosXmlTagPtr &doc,
	//		const u64 &docId,
	//		const AosRundataPtr &rdata);

private:
	bool	procRequest(const AosDocReqPtr &request);
	bool	addDoc(const OmnString &word, const int len, const u64 &docid);
	bool	createDocPriv(const AosDocReqPtr &request);
	bool	createLogPriv(const AosDocReqPtr &request);
	bool	modifyDocPriv(const AosDocReqPtr &request);
	bool	deleteDocPriv(const AosDocReqPtr &request);
	bool	deleteLogPriv(const AosDocReqPtr &request);
	bool	modifyAttrStrPriv(const AosDocReqPtr &request);
	//bool	modifyAttrU64Priv(const AosDocReqPtr &request);
	//bool	addCtnrMemberPriv(const AosDocReqPtr &request);
	//bool	removeCtnrMemberPriv(const AosDocReqPtr &request);
	//void	resetHashGroup();
	//void	resetHashGroup2();
	//void	resetLoopHashGroup();
	//void	resetLoopHashGroup2();
	
	// bool 	removeMetaAttrs(
	// 			vector<AosBuffPtr> &buffs,
	// 			const AosXmlTagPtr &doc,
	// 			const u64 &docid,
	// 			const AosRundataPtr &rdata);
	// void 	setMetaAttrs();
	bool	isWordIgnored(const OmnString &word);
	//bool 	procOrderAttrs( const AosXmlTagPtr origxml, const AosXmlTagPtr newxml);
/*
	bool collectWords(
			AosSeWordHash &wordHash,
			const AosXmlTagPtr &parent_doc,
			const AosXmlTagPtr &xml,
			const bool marked); 
	bool collectWords(
			AosSeWordHash &attrWordHash,
			const OmnString &value);
	bool collectAttrs(
			AosSeAttrHash &attrs,
			const AosXmlTagPtr &parent_doc,
			const AosXmlTagPtr &xml,
			const bool marked);
*/
	//bool procContainers(
	//		const AosRundataPtr &rdata,
	//		const OmnString &ctn,
	//		const AosXmlTagPtr &doc,
	//		const u64 &docid);

	// static bool addMetaAttrs(
	// 		vector<AosBuffPtr> &buffs,
	// 		const AosXmlTagPtr &doc, 
	// 		const u64 &docId,
	// 		const AosRundataPtr &rdata);

	bool modifyDocPriv(
			const AosXmlTagPtr &origxml,
			const AosXmlTagPtr &newxml, 
			const AosDocReqPtr &request,
			const AosRundataPtr &rdata);

	// static bool modifyMetaAttrs(
	// 		vector<AosBuffPtr> &buffs,
	// 		const AosXmlTagPtr &olddoc, 
	// 		const AosXmlTagPtr &newdoc, 
	// 		const u64 &docid,
	// 		const AosRundataPtr &rdata);

	//bool removeValueU64(
	//		const OmnString &name,
	//		const OmnString &valueStr,
	//		const OmnString &word,
	//		const u64 &docid,
	//		bool &removed,
	//		const AosRundataPtr &rdata);

	//bool addValueU64(
	//		const OmnString &valueStr,
	//		const int &i,
	//		const u64 &docid,
	//		bool &added,
	//		const AosRundataPtr &rdata);

	//bool addContainerOrder(const OmnString &attrname, const OmnString &ctnrname,
	//			const OmnString &siteid);

private:
	// Chen Ding, 08/09/2011
	// Chen Ding, 03/20/2012, moved to DocProcUtil
	// static bool addValueToIIL(
	// 		vector<AosBuffPtr> &buffs,
	// 		const OmnString &iilname, 
	// 		const OmnString &value,
	// 		const u64 &docid, 
	// 		const bool value_unique,
	// 		const bool docid_unique,
	// 		const AosRundataPtr &rdata)
	// {
	// 	aos_assert_r(smIILClient, false);
	// 	// int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// 	// aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	// 	//u64 transid = mDocTransId[tid] ++;
	// 	int attrtype = (int)eAosAttrType_Str;
	// 	if (AosDocUtil::isTypedAttr(iilname.data(), iilname.length()))
	// 	{
	// 		attrtype = (int)iilname.data()[iilname.length()-1];
	// 	}
	// 	switch (attrtype)
	// 	{
	// 	case eAosAttrType_U64:
	// 	case eAosAttrType_Date:
	// 		 smIILClient->addU64ValueDoc(buffs, iilname, 
	// 			 atoll(value.data()), docid, value_unique, docid_unique, rdata); 
	// 		 break;
	//
	//	default:
	//	 	 smIILClient->addStrValueDoc(buffs, iilname, 
	//				 value, docid, value_unique, docid_unique, rdata); 
	//		 break;
	//	}
	//	return true;
	//}

	// Chen Ding, 03/20/2012, Moved to DocProcUtil
	// static bool removeValueFromIIL(
	// 		vector<AosBuffPtr> &buffs,
	// 		const OmnString &iilname, 
	// 		const OmnString &value, 
	// 		const u64 &docid,
	// 		const AosRundataPtr &rdata)
	// {
	// 	aos_assert_r(smIILClient, false);
	// 	int tid = OmnThreadMgr::getSelf()->getCrtThreadLogicId();
	// 	aos_assert_r(tid >= 0 && tid < eMaxThreads, false);
	// 	//u64 transid = mDocTransId[tid];
	// 	int attrtype = (int)eAosAttrType_Str;
	// 	if (AosDocUtil::isTypedAttr(iilname.data(), iilname.length()))
	// 	{
	// 		attrtype = (int)iilname.data()[iilname.length()-1];
	// 	}
	// 	switch (attrtype)
	// 	{
	// 	case eAosAttrType_U64:
	// 	case eAosAttrType_Date:
	// 		 if (smShowLog) OmnScreen << "Remove value doc: " << iilname 
	// 			 << ":" << value << ":" << docid << endl;
	// 		 smIILClient->removeU64ValueDoc(buffs, iilname, atoll(value.data()), docid, rdata);
	// 		 break;
	// 
	//	default:
	//		 if (smShowLog) OmnScreen << "Remove value doc: " << iilname 
	//			 << ":" << value << ":" << docid << endl;
	//		 smIILClient->removeValueDoc(buffs, iilname, value, docid, rdata);
	//		 break;
	//	}
	//	return true;
	//}

	bool addWordIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &word, 
			const u64 docid,
			const OmnString &ctnr_objid,
			const AosEntryMark::E mark,
			const AosRundataPtr &rdata, 
			const int line);
	bool removeWordIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &word, 
			const u64 docid, 
			const OmnString &ctnr_objid,
			const AosEntryMark::E mark,
			const AosRundataPtr &rdata); 
	bool addAttrIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &name, 
			const OmnString &value, 
			const u64 docid, 
			const OmnString &ctnr_objid,
			const AosEntryMark::E mark,
			const AosRundataPtr &rdata);
	bool removeAttrIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &name, 
			const OmnString &value, 
			const u64 docid, 
			const OmnString &ctnr_objid,
			const AosEntryMark::E mark,
			const AosRundataPtr &rdata);
	bool addAttrWordIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &name, 
			const OmnString &value, 
			const u64 docid, 
			const OmnString &ctnr_objid,
			const AosEntryMark::E mark,
			const AosRundataPtr &rdata);
	bool removeAttrWordIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const OmnString &name, 
			const OmnString &value, 
			const u64 docid, 
			const OmnString &ctnr_objid,
			const AosEntryMark::E mark,
			const AosRundataPtr &rdata);
	bool addMemberOf(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const AosXmlTagPtr doc,
			const AosXmlTagPtr &memberof_doc,
			const OmnString &memberof_objid,
			const AosRundataPtr &rdata);
	bool removeMemberOf(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const AosXmlTagPtr doc,
			const AosXmlTagPtr &memberof_doc,
			const AosRundataPtr &rdata);
	bool modifyMemberOf(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const AosXmlTagPtr &origxml,
			const AosXmlTagPtr &newxml,
			const AosRundataPtr &rdata);
	bool addSysInfoIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const u64 docid, 
			const OmnString &objid, 
			const u64 ctnr_docid, 
			const OmnString &ctnr_objid,
			const u32 ctime, 
			const AosRundataPtr &rdata,
			const bool isCtnr);
	bool removeSysInfoIIL(
			//vector<AosBuffPtr> &buffs,
			vector<AosTransPtr>  *allTrans,
			const u32 arr_len,
			const u64 docid, 
			const OmnString &objid, 
			const u64 ctnr_docid, 
			const OmnString &ctnr_objid,
			const u32 ctime, 
			const AosRundataPtr &rdata,
			const bool isCtnr);
	bool removeContainer(const AosXmlTagPtr &ctnr, const AosRundataPtr &rdata);

	AosEntryMark::E setMarkMemberOf(const AosEntryMark::E mark)
	{
		if (mark & AosEntryMark::eBothIndex || mark & AosEntryMark::eContainerIndex)
		{
			if (mark & AosEntryMark::eAttrWordIndex)
			{
		    	return  AosEntryMark::E(AosEntryMark::eCtnrOnly | AosEntryMark::eAttrWordIndex);
			}
			return  AosEntryMark::eCtnrOnly;
		}
		else
		{
		    return  AosEntryMark::eNoIndex;
		}
	}

	// Ketty 2013/02/26
	//u64  createDocTransid();
	//bool loadNewTransId();  
	//bool openDocTransIdFile(const OmnString &fname);
	//bool createBuffs(vector<AosBuffPtr> &buffs);

	inline bool addRequest(const AosDocReqPtr &req)
	{
		// It adds the request 'req' into the message queue. 
		// But if the queue is full, it will wait until
		// it becomes not full. It should block the execution
		// of the current thread until the queue is not full.

	//	if (doc_transid == 0)                
	//	{                                    
	//		doc_transid = createDocTransid();
	//	}                                    
	//	req->setDocTransId(doc_transid);

		bool timeout = false;
		while (1)
		{
			mLock->lock();
			if (mNumReqs > smMaxOutstandingMsgs) 
			{
				mCondVar->timedWait(mLock, timeout, 1);
				mLock->unlock();
				continue;
			}

			mTotalAccepted++;
			mNumReqs++;
			if (mHead) mHead->prev = req;
			req->next = mHead;
			req->prev = 0;
			mHead = req;
			if (!mTail) mTail = req;
			mCondVar->signal();
			mLock->unlock();
			return true;
		}
	}

	// Ketty 2013/03/19	
	bool 	sendTrans(const AosRundataPtr &rdata,
				vector<AosTransPtr>  *allTrans,
				const u32 arr_len);
};
#endif
