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
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_GetStatBinaryDocidTrans_h
#define Aos_DocTrans_GetStatBinaryDocidTrans_h

#include "TransUtil/DocTrans.h"

class AosGetStatBinaryDocidTrans : virtual public AosDocTrans
{

private:
	u64			mCtrlDocid;
	u64			mStatId;

public:
	AosGetStatBinaryDocidTrans(const bool regflag);
	AosGetStatBinaryDocidTrans(
			const u64 ctrl_docid,
			const u64 stat_id,
			const bool need_save,
			const bool need_resp);
	~AosGetStatBinaryDocidTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

private:
	AosBuffPtr retrieveBinaryDoc(AosXmlTagPtr &doc);

};
#endif

