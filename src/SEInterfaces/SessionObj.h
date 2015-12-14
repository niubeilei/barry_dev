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
// 01/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SessionObj_h
#define Aos_SEInterfaces_SessionObj_h

#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"

class AosSessionObj : virtual public OmnRCObject
{

public:
	virtual void			setDbName(const OmnString &db_name) = 0;
	virtual OmnString		getDbName() = 0;
	virtual u64 			getUserid() const = 0;
	virtual OmnString		getCid() const = 0;
	virtual OmnString 		getSsid() const = 0;
	virtual AosXmlTagPtr	getAccessedDoc() const = 0;
	virtual void			setAccessedDoc(const AosXmlTagPtr &doc) = 0;
	virtual u64				getAccessedDocUserid(const AosRundataPtr &rdata) const = 0;
	virtual void			setVerificationCode(const OmnString &ver_code) = 0;
	virtual AosUserAcctObjPtr 	getRequesterAcct(const AosRundataPtr &rdata) = 0;
	virtual bool			logout() = 0;
	virtual bool			modifyLoginObj(const AosXmlTagPtr &loginobj) = 0;
	virtual void			setLanguageCode(const AosRundataPtr &rdata, const OmnString &lang) = 0;
	virtual void			setCurDatabase(const AosRundataPtr &rdata, const OmnString &database) = 0;
	virtual OmnString		getCurDatabase() = 0;
	virtual OmnString 		getVerificationCode() const = 0;
	virtual AosXmlTagPtr	getLoginObj() = 0;
	virtual bool 			checkLogin( const AosXmlTagPtr &vpd,
								OmnString &hpvpd, 
								const AosRundataPtr &rdata) = 0;
};
#endif
