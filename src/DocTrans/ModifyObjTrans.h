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
#ifndef Aos_DocTrans_ModifyObjTrans_h
#define Aos_DocTrans_ModifyObjTrans_h

#include "TransUtil/DocTrans.h"

class AosModifyObjTrans : virtual public AosDocTrans
{

private:
	u64			mDocid;
	AosXmlTagPtr mNewDoc;
	AosXmlTagPtr mOrigDoc;

public:
	AosModifyObjTrans(const bool regflag);
	AosModifyObjTrans(
			const u64 docid,
			const AosXmlTagPtr &new_doc,
			const AosXmlTagPtr &orig_doc,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosModifyObjTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

