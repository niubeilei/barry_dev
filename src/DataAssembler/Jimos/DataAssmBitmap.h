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
#ifndef Aos_DataAssembler_Jimos_DataAssmBitmap_t
#define Aos_DataAssembler_Jimos_DataAssmBitmap_t

#include "DataAssembler/Jimos/DataAssemblerJimo.h"


class AosDataAssmBitmap : public AosDataAssemblerJimo
{
protected:

public:
	AosDataAssmBitmap(
			const OmnString &name,
			const OmnString &version);
	~AosDataAssmBitmap();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	virtual AosDataRecordObjPtr cloneDataRecord(const AosRundataPtr &rdata);

	virtual bool	sendStart(const AosRundataPtr &rdata);
	virtual bool	sendFinish(const AosRundataPtr &rdata);
	virtual bool	appendEntry(
						const AosValueRslt &value,
						const AosRundataPtr &rdata);

	virtual bool 	addEntry( 		
						const AosRundataPtr &rdata, 
						const u64 docid);
};

#endif

