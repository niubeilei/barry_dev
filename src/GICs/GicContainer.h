////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 07/08/2010: Created by Tom Xiao
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicContainer_h
#define Aos_GICs_GicContainer_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"


class AosGicContainer : public AosGic
{
	AosHtmlCode		mGicCode;

public:
	AosGicContainer(const bool flag);
	~AosGicContainer();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
	
	bool createObj(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code,
		const AosRundataPtr &rdata);
	
	bool createSubGic(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code,
		const AosRundataPtr &rdata);

	void setGicCode(const AosHtmlCode &code){mGicCode = code;}

	class createSubContainer : public OmnThrdShellProc
	{
		friend class AosGicContainer;

		OmnDefineRCObject;

		AosGicContainer *	mCaller;
		AosHtmlReqProcPtr 	mHtmlPtr;
		AosXmlTagPtr 		mVpd;
		AosXmlTagPtr 		mObj;
		OmnString 			mParentId;
		AosHtmlCode 		mCode;

	public:
		createSubContainer(
			AosGicContainer * caller,
			const AosHtmlReqProcPtr &htmlPtr,
			AosXmlTagPtr &vpd, 
			const AosXmlTagPtr &obj, 
			const OmnString &parentId,
			AosHtmlCode &code)
		:
		OmnThrdShellProc("createSubContainer"),
		mCaller(caller),
		mHtmlPtr(htmlPtr),
		mVpd(vpd),
		mObj(obj),
		mParentId(parentId),
		mCode(code)
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(){return true;} 
	};

private:
	void getDefAttr(
		const AosXmlTagPtr &nChild,
		OmnString &objpath,
		OmnString &addgic,
		OmnString &lastgic);
	bool setPos(
		const OmnString attrname,
		const AosXmlTagPtr &obj,
		const AosXmlTagPtr &vpd);
	bool	addPos(
		const AosXmlTagPtr &obj,
		const AosXmlTagPtr &vpd);
	bool	addChildPos(
		const AosXmlTagPtr &obj,
		const AosXmlTagPtr &vpd);

	bool	isDeepLevel(const AosXmlTagPtr &obj);
};

#endif

