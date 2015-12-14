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
// 05/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SQLInterface_ImportData_h
#define Omn_SQLInterface_ImportData_h

#include "Obj/ObjDb.h"
#include "SQLServer/SqlProc.h"



class AosImportData : virtual public AosSqlProc, 
						   virtual public OmnDbObj
{

private:

public:
	AosImportData(const bool regflag);
	~AosImportData();

	// ImportData Interface
	virtual bool proc(const AosTransPtr &trans, const AosRundataPtr &rdata);

	// OmnObjDb Interface
	virtual OmnRslt     serializeFromDb();
	virtual OmnRslt     serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString   insertStmt() const;
	virtual OmnString   retrieveStmt() const;
	virtual OmnString   updateStmt() const;
	virtual OmnString   removeStmt() const;
	virtual OmnString   removeAllStmt() const;
	virtual OmnString   existStmt() const;
	virtual int         objSize() const {return sizeof(*this);}
	
private:
//	bool runActions(
	AosXmlTagPtr createXml(
		const OmnDbRecordPtr &rcd, 
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata);
	bool createDocOnServer(
		const AosXmlTagPtr &xml, 
		const AosRundataPtr &rdata);
};
#endif

