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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SmartDoc_h
#define Aos_SmartDoc_SmartDoc_h

#include "Actions/Ptrs.h"
#include "alarm_c/alarm.h"
#include "MultiLang/LangTermIds.h"
#include "QueryClient/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Security/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SdocTypes.h"
#include "SmartDoc/SdocIds.h"
#include "SmartDoc/SdocNames.h"
#include "SEInterfaces/SmartDocObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/OmnNew.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosSmartDoc : virtual public AosSmartDocObj
{
	OmnDefineRCObject;
public:
	enum
	{
		eMaxConds = 30,
		eMaxActions = 30
	};

protected:
	AosSdocId::E		mSdocType;	
	AosXmlTagPtr		mSdoc;
	OmnString			mSdocObjid;
	AosXmlTagPtr		mCondHead;
	AosXmlTagPtr		mActionHead;

	AosSmartDocObjPtr		mPrev;
	AosSmartDocObjPtr 		mNext;

public:
	AosSmartDoc(
			const OmnString &name, 
			const AosSdocId::E type, 
			const bool regflag);
	AosSmartDoc(const OmnString &objid, const AosRundataPtr &rdata);
	~AosSmartDoc();

	// Chen Ding, 11/28/2012
	virtual AosSmartDocObjPtr 	clone();
	virtual bool  run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual bool  config(){ return true; };		// Ketty 2012/03/12

	virtual bool  smartdocCreated(
			const OmnString &siteid,
			const AosXmlTagPtr &sdoc,
			const u64 &userid) {return true;}

	// Chen Ding, 11/28/2012
	virtual bool runSmartdoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual bool procSmartdocs(const OmnString &sdoc_objids, const AosRundataPtr &rdata);
	virtual bool runSmartdoc(const u64 &sdocid, const AosRundataPtr &rdata);

	// Static functions to run smart docs
	bool runSmartdocs(const OmnString &sdoc_objids, const AosRundataPtr &rdata);
	static bool runSmartdoc(const OmnString &sdoc_objid, const AosRundataPtr &rdata);
	// static bool runSmartdoc(const u64 &sdoc_docid, const AosRundataPtr &rdata);
	static AosSmartDocObjPtr	getSmartDoc(const AosXmlTagPtr &sdoc)
	{
		aos_assert_r(sdoc, 0);
		return getSmartDoc(sdoc->getAttrStr(AOSTAG_SMARTDOC_TYPE));
	}
	static AosSmartDocObjPtr	getSmartDoc(const OmnString &sdoc_id);
	static AosXmlTagPtr createDoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	static AosXmlTagPtr createDoc(
	 				const AosXmlTagPtr &sdoc, 
	 				const OmnString &tagname, 
	 				const AosRundataPtr &rdata);
	static void  sConfig();		// Ketty 2012/03/12

	bool  runSmartdoc(const AosRundataPtr &rdata) {return run(mSdoc, rdata);}
	bool  run(const AosXmlTagPtr &doc, 
	 				const OmnString &sdoc_objid,
	 				const AosRundataPtr &rdata);
	AosSdocId::E	getType() const {return mSdocType;}

	OmnString getSdocObjid() const { return mSdocObjid; }
	void	setSdoc(const AosXmlTagPtr &sdoc) {mSdoc = sdoc;}
	AosXmlTagPtr getSdoc() const { return mSdoc; }

	virtual AosSmartDocObjPtr getNext() const {return mNext;}
	virtual AosSmartDocObjPtr getPrev() const {return mPrev;}
	virtual void setNext(const AosSmartDocObjPtr &s) {mNext = s;}
	virtual void setPrev(const AosSmartDocObjPtr &s) {mPrev = s;}

	bool 	parseActions(
				const AosXmlTagPtr &sdoc, 
				std::vector<AosActionObjPtr> &the_actions,
				const AosRundataPtr &rdata);
	void	setType(const AosSdocId::E type) {mSdocType = type;}

private:
	bool 	loadSmartdoc(const OmnString &objid, const AosRundataPtr &rdata);
	bool 	registerSdoc(
				const OmnSPtr<AosSmartDoc> &sdoc,
				const OmnString &name,
				const AosSdocId::E type, 
				OmnString &errmsg);

public:

	// bool procSmartdocStatic(
	 // 			const OmnString &siteid,
	 // 			const AosWebRequestPtr &req, 
	 // 			const AosXmlTagPtr &root, 
	 // 			const AosXmlTagPtr &userobj, 
	 // 			const OmnString &sdoc_objid, 
	 // 			OmnString &contents, 
	 // 			AosXmlRc &errcode, 
	 // 			OmnString &errmsg);
protected:
	bool			addThreadShellProc(
						const OmnThrdShellProcPtr &runner,
						const AosRundataPtr &rdata);
};
#endif

