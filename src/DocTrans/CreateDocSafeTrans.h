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
#ifndef Aos_DocTrans_CreateDocSafeTrans_h
#define Aos_DocTrans_CreateDocSafeTrans_h

#include "TransUtil/DocTrans.h"

class AosCreateDocSafeTrans : virtual public AosDocTrans
{

private:
	u64			mDocid;
	bool		mSaveDocFlag;
	AosXmlTagPtr mNewDoc;

public:
	AosCreateDocSafeTrans(const bool regflag);
	AosCreateDocSafeTrans(
			const u64 docid,
			const AosXmlTagPtr &new_doc,
			const bool save_doc_flag,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosCreateDocSafeTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

