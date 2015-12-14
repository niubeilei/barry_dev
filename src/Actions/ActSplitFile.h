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
// 06/02/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActSplitFile_h
#define Aos_SdocAction_ActSplitFile_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"

class AosActSplitFile : virtual public AosSdocAction
{
private:
	OmnString	mFilename;
	int			mRecordSize;
	int			mNumRcdsPerTask;
	bool		mEvenDist;
	u64			mJobDocid;

public:
	AosActSplitFile(const bool flag);
	AosActSplitFile(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosActSplitFile();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool runTemplate(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	bool config(OmnString &fname, 
				int &record_size, 
				int &num_rcds_per_task,
				bool &even_dist,
				u64 &job_docid,
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);

	bool runPriv(
				const OmnString &fname, 
				const int record_size,
				const int num_rcds_per_task,
				const bool even_dist,
				const u64 &job_docid,
				const AosRundataPtr &rdata);
};
#endif

