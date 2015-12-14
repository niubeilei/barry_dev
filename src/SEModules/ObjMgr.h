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
// 	Created: 08/24/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEModules_ObjMgr_h
#define AOS_SEModules_ObjMgr_h

#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/Docid.h"
#include "SEUtil/Objid.h"
#include "SEUtil/Objname.h"
#include "SEUtil/ObjType.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/HashUtil.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "Rundata/Ptrs.h"

using namespace std;

OmnDefineSingletonClass(AosObjMgrSingleton,
						AosObjMgr,
						AosObjMgrSelf,
						OmnSingletonObjId::eObjMgr,
						"ObjMgr");


class AosObjMgr : virtual public OmnRCObject, public AosObjMgrObj
{
	OmnDefineRCObject;

public:

private:
	bool			mIsStopping;
	OmnMutexPtr		mLock;
	OmnString       mUserCtnrTags[AosObjType::eMax+1];

public:
	AosObjMgr();
	~AosObjMgr();

    // Singleton class interface
    static AosObjMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	AosXmlTagPtr	createLoginLogCtnr(const AosRundataPtr &rdata);
	AosXmlTagPtr	createInvReadLogCtnr(const AosRundataPtr &rdata);
	AosXmlTagPtr	createCidCtnr(const AosRundataPtr &rdata);
	AosXmlTagPtr	createRootCtnr(const AosRundataPtr &rdata);
	virtual AosXmlTagPtr	createPubNamespace(const AosRundataPtr &rdata);
	virtual bool    createObjsAtStartup();
	
	AosXmlTagPtr createDftContainer(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		const bool is_public,
		const OmnString &cid);
	
	AosXmlTagPtr createSysDftContainer(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc);

	AosXmlTagPtr createDftUserContainer(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		const OmnString &cid);

	virtual AosXmlTagPtr createUserContainer(
			const AosRundataPtr &rdata,
			const OmnString &ctnr_objid,
			const OmnString &ctnr_name,
			const OmnString &cid);

	AosXmlTagPtr 	createGuestUser(const u32 siteid, const OmnString &passwd);
	AosXmlTagPtr	createLostFoundCtnr(const AosRundataPtr &rdata);
	
	AosXmlTagPtr createSysContainer(
		const AosRundataPtr &rdata,
		const OmnString &ctnr_objid, 
		const u64 &docid,
		const OmnString &ctnr_name, 
		const OmnString &parent_ctnr_objid);
	
	// Brian Zhang 12/22/2011
	// Create smstem log's smartdoc
	AosXmlTagPtr createSysLogSmartdoc(
		const AosRundataPtr &rdata,
		const OmnString &objid, 
		const OmnString &ctnr_name);
	
	AosXmlTagPtr createSysLogContainer(
		const AosRundataPtr &rdata,
		const OmnString &ctnr_objid, 
		const u64 &docid,
		const OmnString &ctnr_name);

	virtual AosXmlTagPtr createSysUserCtnr(const AosRundataPtr &rdata);
	
	AosXmlTagPtr 	getSysUserCtnr1(const u32 siteid, const AosRundataPtr &rdata);
	AosXmlTagPtr	getGuest(const u32 siteid, const AosRundataPtr &rdata);
	
	AosXmlTagPtr createUserInboxCtnr(
		const AosRundataPtr &rdata,
		const OmnString &cid);

	AosXmlTagPtr	
	createLostFoundCtnr(
			const AosRundataPtr &rdata,
			const OmnString &cid) 
	{
		OmnString ctnr_objid = AosObjid::compose(AOSOBJIDPRE_LOSTaFOUND, cid);
		return createUserContainer(rdata, ctnr_objid, AOSOBJNAME_LOSTFOUND_CTNR, cid);
	}

	AosAccessRcdPtr createUserAccessRecord(
		const AosRundataPtr &rdata,
		const OmnString &cloudid,
		const u64 &creator_docid); 

	AosAccessRcdPtr createAccessRecord1(
		const AosRundataPtr &rdata,
		const OmnString &cloudid, 
		const u64 &docid);

	AosAccessRcdPtr createRootArcd(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &rootdoc);

	AosAccessRcdPtr createGuestArcd(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &guestdoc);

	AosAccessRcdPtr createUnknownUserArcd(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &unknowndoc);

	OmnString	getSysUserCtnrObjid(const AosRundataPtr &rdata);

	// Chen Ding, 11/20/2010
	static bool procMetaObjidOnRetObj(
		const OmnString &objid,
		const AosMetaExtension::E meta_objid_type,
		const AosRundataPtr &rdata);

	static bool createObjAsNeeded(
		const OmnString &objid, const AosRundataPtr &rdata);
	OmnString getRootCtnrObjid(const u32 siteid);
	OmnString getRootCloudid(const u32 siteid, const AosRundataPtr &rdata);
	AosXmlTagPtr getRootCtnr(const u32 siteid, const AosRundataPtr &rdata);
	AosXmlTagPtr getSysUserCtnr(const u32 siteid, const AosRundataPtr &rdata);

	AosXmlTagPtr createSystemCtnr(
		const OmnString &objid,
		const u64 &docid,
		const OmnString &stype,
		const AosRundataPtr &rdata);

	//Jozhi 2014-04-21
	bool createSizeIdContainer(const AosRundataPtr &rdata);

	AosXmlTagPtr	createSysRootAcct(const AosRundataPtr &rdata);

	// Linda, 08/02/2011
	static u64 getSysRootDocid(const u32 siteid, const AosRundataPtr &rdata)
	{
		return AOSDOCID_SYSROOT;
	}
	inline static u64 getSuperUserDocid(const u32 siteid, const AosRundataPtr &rdata)
	{
		return AosObjMgrObj::getSuperUserDocid();
	}

	static OmnString getSysRootCid(const u32 siteid, const AosRundataPtr &rdata)
	{
		return AOSCLOUDID_SYSROOT;
	}

	AosXmlTagPtr createAccessRecord(const AosRundataPtr &rdata, const AosXmlTagPtr &ref_doc);

	// Created by Young : 2014/05/27
	bool createSystemDocs(const AosRundataPtr &rdata);


private:
	bool 	init();
	OmnString getCtnrAttrname(const OmnString &otype) const
	{
		AosObjType::E objtype = AosObjType::convertToEnum(otype);
		if (objtype > AosObjType::eInvalid && objtype < AosObjType::eMax)
		{
			return mUserCtnrTags[objtype];
		}
		return AosObjname::composeCtnrAttrName(otype);
	}

public:
	virtual AosXmlTagPtr createIdGenDoc(
						const OmnString &ctnr_objid,
						const bool is_public,
						const AosRundataPtr &rdata);
};
#endif

