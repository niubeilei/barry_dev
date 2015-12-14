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
#ifndef Aos_DocTrans_CommitDocSnapshotTrans_h
#define Aos_DocTrans_CommitDocSnapshotTrans_h

#include "TransUtil/DocTrans.h"

class AosCommitDocSnapshotTrans : virtual public AosDocTrans
{

private:
	int				mVirtualId;
	AosDocType::E 	mDocType;
	u64				mTaskDocid;

public:
	AosCommitDocSnapshotTrans(const bool regflag);
	AosCommitDocSnapshotTrans(
			const int virtual_id,
			const AosDocType::E doc_type,
			const u64 &snap_id,
			const u64 &task_docid,
			const bool need_save);
	~AosCommitDocSnapshotTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

private:
	AosXmlTagPtr 	getDoc();

};
#endif

