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
// 2013/12/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MySqlAdapter_MySqlRecord_h
#define Aos_MySqlAdapter_MySqlRecord_h

#include "SEInterfaces/MySqlRecordObj.h"
#include "MySqlAdapter/Ptrs.h"

class AosMySqlRecord : public AosMySqlRecordObj 
{
	OmnDefineRCObject;

private:
	vector<AosMySqlFieldPtr>	mFields;	

public:
	AosMySqlRecord();
	~AosMySqlRecord();
	
	virtual bool encodeHeader(
					const AosRundataPtr &rdata, 
					const AosBuffPtr &buff); 

	virtual bool encodeRows(
					const AosRundataPtr &rdata, 
					const AosBuffPtr &buff); 

	virtual bool appendField(
					const AosRundataPtr &rdata,
					const AosMySqlFieldPtr &field);

	virtual bool writeDelimiter(
					const AosRundataPtr &rdata,
					const AosBuffPtr &buff);
};
#endif
