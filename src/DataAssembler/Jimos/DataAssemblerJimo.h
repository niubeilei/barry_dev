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
// 2013/12/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataAssembler_Jimos_DataAssemblerJimo_h
#define Aos_DataAssembler_Jimos_DataAssemblerJimo_h

#include "DataAssembler/DataAssembler.h"
#include "Jimo/Jimo.h"


class AosDataAssemblerJimo : public AosJimo, public AosDataAssembler
{
protected:

public:
	AosDataAssemblerJimo(
			const OmnString &name,
			const OmnString &version);
	~AosDataAssemblerJimo();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	virtual AosDataRecordObjPtr cloneDataRecord(const AosRundataPtr &rdata);

	virtual bool addEntry( 		
						const AosRundataPtr &rdata, 
						AosDataRecordObj *record, 
						const u64 docid);
};

#endif

