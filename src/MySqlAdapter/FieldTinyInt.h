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
#ifndef Aos_MySqlAdapter_FieldTinyInt_h
#define Aos_MySqlAdapter_FieldTinyInt_h

#include "MySqlAdapter/MySqlField.h"

class AosMySqlFieldTinyInt : public AosMySqlField 
{
	OmnDefineRCObject;
private:
	
public:
	AosMySqlFieldTinyInt();
	~AosMySqlFieldTinyInt();
	
	virtual bool encodeHeader(
					const AosRundataPtr &rdata, 
					const AosBuffPtr &buff);

	virtual bool encodeField(
					const AosRundataPtr &rdata, 
					const AosBuffPtr &buff);

};

#endif
