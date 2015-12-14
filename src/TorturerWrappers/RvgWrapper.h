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
// 04/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerWrappers_RvgWrapper_h
#define Omn_TorturerWrappers_RvgWrapper_h

#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include <string>

class AosRvgWrapper : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:
	OmnString			mName;
	OmnString			mKeywords;
	OmnString			mDescription;
	OmnString			mXml;
	char				mStatus;

public:
	AosRvgWrapper();
	~AosRvgWrapper();

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
	OmnString getKeywords() const {return mKeywords;}
	OmnString getDescription() const {return mDescription;}
	OmnString getXml() const {return mXml;}
	char		getStatus() const {return mStatus;}

	void		setName(const OmnString &name) {mName = name;}
	void		setDescription(const OmnString &k) {mDescription = k;}
	void		setKeywords(const OmnString &k) {mKeywords = k;}
	void		appendKeywords(const OmnString &k) {mKeywords += k;}
	void		setXml(const OmnString &d) {mXml = d;}
	bool		getRvg(const OmnString &name);

	static bool	rvgExist(const OmnString &name);
};

#endif

