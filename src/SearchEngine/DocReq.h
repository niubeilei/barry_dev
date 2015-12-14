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
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SearchEngine_DocReq_h
#define AOS_SearchEngine_DocReq_h

#include "Rundata/Rundata.h"
#include "SearchEngine/Ptrs.h"
//#include "SearchEngine/DocServerCb.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "SEUtil/Ptrs.h"
#include "TransUtil/Ptrs.h"
//#include "TransUtil/BigTrans.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlDoc.h"


struct AosDocReq : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum ReqType
	{
		eCreateDoc,
		eCreateLog,
		eModifyDoc,
		eDeleteDoc,
		eDeleteLog,
		eModifyAttrU64,
		eModifyAttrStr,
		eAddCtnrMember,
		eRemoveCtnrMember,
		eSaveDoc			// Chen Ding, 12/19/2011
	};

	AosDocReqPtr		next;
	AosDocReqPtr		prev;
	AosXmlTagPtr		root;
	AosXmlTagPtr		doc;
	AosXmlTagPtr		origdocroot;
	AosDocSvrCbObjPtr	caller;
	void			   *userdata;
	ReqType				reqtype;
	OmnString			siteid;
	u64 				docid;
	OmnString			objid;
	OmnString			appname;
	u64 				userid;
	bool				synobj;
	OmnString			aname;
	OmnString			strval1;
	OmnString			strval2;
	OmnString			ctnr_objid;		// Ken, 2011/1/22
	u64					u64val1;
	u64					u64val2;
	bool				exist;
	bool				value_unique;
	bool				docid_unique;
	OmnString			filename;
	int					line;
	//u64					doc_transid;
	//AosTaskTransPtr		task_trans;		// Ketty 2013/02/22
	//AosBigTransPtr		trans;		// Ketty 2013/03/15
	AosRundataPtr		mRundata;

public:
	AosDocReq(
		const ReqType type,
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &doc, 
		const u64 &userid,
		const AosDocSvrCbObjPtr &caller,
		const void *userdata, 
		const AosRundataPtr &rdata);
	AosDocReq(const ReqType type, const u64 &docid, const AosRundataPtr &rdata)
	:
	reqtype(type),
	docid(docid)
	{
		mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
		mRundata->copyCtlrInfo(rdata.getPtr());
	}

	~AosDocReq();
	
	//u64 getDocTransid(){return doc_transid;}
	//void setTrans(const AosBigTransPtr &t){trans = t;}
	//AosBigTransPtr getTrans() {return trans;}
	//void setDocTransId(const u64 &id) {doc_transid = id;}
	AosRundataPtr getRundata() const {return mRundata;}
};

#endif
