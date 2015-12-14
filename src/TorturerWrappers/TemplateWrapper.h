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
// 05/12/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerWrappers_TemplateWrapper_h
#define Omn_TorturerWrappers_TemplateWrapper_h

#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include <string>

class AosTemplateWrapper : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:
	OmnString			mName;
	OmnString			mDesc;
	OmnString			mXml;

public:
	AosTemplateWrapper();
	~AosTemplateWrapper();

	// OmnObjDb Interface
	virtual OmnRslt		serializeFromDb();
	virtual OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString	insertStmt() const;
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual OmnString	existStmt() const;
	virtual int			objSize() const {return sizeof(*this);}

	OmnString getName() const {return mName;}
	OmnString getDesc() const {return mDesc;}
	OmnString getXml() const {return mXml;}

	void		setName(const OmnString &name) {mName = name;}
	void		setDesc(const OmnString &d) {mDesc = d;}
	void		setXml(const OmnString &d) {mXml = d;}
	bool		getTemplate(const OmnString &name);

	static bool	templateExist(const OmnString &name);
};

#endif

