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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_AccessCtrlTester_h
#define AosSengTester_AccessCtrlTester_h

#include "SengTorUtil/SengTester.h"
#include "SengTorUtil/StUtil.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosAccessCtrlTester : virtual public AosSengTester
{
private:
	u64				mOwndocid;
	OmnString 		mARObjid;
	u64				mARServerDocid;
	u64				mARLocalDocid;
	AosXmlTagPtr	mAR;

	int				eLoginWhenNeeded;
	int 			eLoginAsOwner;
	int				eLoginAsRoot;
	int				eAddAccWeight;
	int				eRemoveAccWeight;
	int 			eModifyAccWeight;
	int				eNormalAccWeight;
	int 			ePicOwnDoc;

private:
	bool	testRead(const AosRundataPtr &rdata);
	bool	testModify(const AosRundataPtr &rdata);
	bool	testModifyAccesses(const AosRundataPtr &rdata);
	bool	testDelete(const AosRundataPtr &rdata);
	bool	randCreateAccessRecord();
	AosStDocPtr pickDoc();
	bool	createAccessRcdObj();
	bool	checkAccessRcdObj();
	bool	addAccessRecord();
	bool	randModifyAccessRecord();
	bool	pickAccessRcdObj();
	bool	modifyAccessRcdObj();
	bool	modifyAccessRcdObjToList();
	bool	doOneTest();
	bool	modifyAccess(const AosStDocPtr &doc, AosStAccessRecord *ard);
	bool	addAccess(
				const AosStDocPtr &doc,
				const AosSecOpr::E opr,
				const AosAccessType::E acctype,
				AosStAccessRecord *ard, 
				const AosXmlTagPtr &xmlrcd);
	bool	modifyAccess(
				const AosStDocPtr &doc,
				const AosSecOpr::E opr,
				const AosAccessType::E acctype,
				AosStAccessRecord *ard, 
				const AosXmlTagPtr &xmlrcd);
	bool	removeAccess(
				const AosStDocPtr &doc,
				const AosSecOpr::E opr,
				const AosAccessType::E acctype,
				AosStAccessRecord *ard, 
				const AosXmlTagPtr &xmlrcd);
	bool	normalizeAccess(
				const AosStDocPtr &doc,	
				const AosSecOpr::E opr,
				const AosAccessType::E acctype,
				AosStAccessRecord *ard, 
				const AosXmlTagPtr &xmlrcd);
	bool	syncAccessWithServer(
				AosStAccessRecord *ard, 
				const AosXmlTagPtr &xmlrcd);
	bool	checkModifyAccessRecord(
				const u64 &local_did,
				AosStAccessRecord *ard, 
				const AosXmlTagPtr &xmlrcd);

public:
	AosAccessCtrlTester(const bool regflag);
	AosAccessCtrlTester();
	~AosAccessCtrlTester();

	virtual bool test();

	bool	loginAsUser(
				const AosSengTesterMgrPtr &mgr,
				const AosSengTestThrdPtr &thread,	
				OmnString &ssid);
	AosSengTesterPtr clone()
			{
				return OmnNew AosAccessCtrlTester();
			}

};	
#endif

