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
// 2013/07/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_QueryProc_BitmapQueryProc_h
#define AOS_QueryProc_BitmapQueryProc_h

#include "Jimo/Jimo.h"
#include "QueryEngine/QueryProc.h"
#include "QueryEngine/Ptrs.h"


class AosBitmapQueryProc : public AosQueryProc
{
	OmnDefineRCObject;

private:
	OmnString				mVersion;
	OmnString				mJimoName;
	AosQueryReqObjPtr		mQueryReq;
	AosBitmapQueryTermOrPtr	mOrTerm;
	AosQueryProcCallbackPtr	mCallback;

public:
	AosBitmapQueryProc(const OmnString &version);
/*	AosBitmapQueryProc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version);
*/
	~AosBitmapQueryProc();

	using AosJimo::run;

	virtual bool runQuery(
					const AosRundataPtr &rdata,
					const AosQueryReqObjPtr &query, 
					const AosQueryProcCallbackPtr &callback);
	virtual bool queryFinished(const AosRundataPtr &rdata,
					const AosQueryRsltObjPtr &data);
	virtual bool queryFailed(const AosRundataPtr &rdata,
					const OmnString &errmsg);
	virtual int getPageSize() const;
	virtual u64 getQueryId() const;
	virtual bool queryProcResponded(
					const AosRundataPtr &rdata, 
					const OmnString &errmsg, 
					const AosBitmapObjPtr &bitmap);

	// Jimo Interface
	virtual bool run(const AosRundataPtr &rdata);
	virtual bool run(const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker);
	virtual AosJimoPtr cloneJimo()  const;
	//virtual AosJimoType::E getJimoType() const {return AosJimoType::eQueryProc;}
	virtual OmnString getVersion() const {return mVersion;}
	virtual AosJimoPtr cloneJimo(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &worker_doc,
				const AosXmlTagPtr &jimo_doc)  const;
	virtual OmnString toString() const;
	virtual OmnString getJimoName() const {return mJimoName;}

	virtual bool config(const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc);

	virtual bool serializeTo(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual bool serializeFrom(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual void * getMethod(const AosRundataPtr &rdata, 
						const OmnString &name, 
						AosMethodId::E &method_id);

	virtual bool queryInterface(
							const AosRundataPtr &rdata, 
							const OmnString &interface_objid) const;
private:
	bool init(const AosRundataPtr &rdata);
	bool registerMethods();
};
#endif

