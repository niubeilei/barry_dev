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
// 03/20/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DocidMgrObj_h
#define Aos_SEInterfaces_DocidMgrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/DocTypes.h"

class AosDocidMgrObj : virtual public OmnRCObject
{
private:
	static AosDocidMgrObjPtr		smDocidMgr;

public:
     virtual bool		config(const AosXmlTagPtr &def) = 0;
	 virtual void    returnDocid(const u64 &docid, const AosRundataPtr &rdata) =0;

	 virtual bool    bindObjid(
			          OmnString &objid,
					  u64 &docid,          
					  bool &objid_changed,
					  const bool resolve,
					  const bool keepDocid,
					  const AosRundataPtr &rdata)=0;

	    // Chen Ding, 2013/11/01
	 virtual u64 nextDocid(const AosRundataPtr &rdata, const int vid) =0;
	 virtual u64 nextLogGroupDocid(const AosRundataPtr &rdata) =0;
		
	 virtual u64 nextDocid(const int &vid, 
			 OmnString &objid, 
			 const AosRundataPtr &rdata) =0;
		
	virtual u64 nextDocid(OmnString &objid, const AosRundataPtr &rdata) =0;                          

	static void setDocidMgr(const AosDocidMgrObjPtr &obj) {smDocidMgr = obj;}
	static AosDocidMgrObjPtr getDocidMgr() {return smDocidMgr;}
};
#endif

