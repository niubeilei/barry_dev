////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataShuffler_DocidShuffler_h
#define AOS_DataShuffler_DocidShuffler_h

#include "DataShuffler/DataShuffler.h"

class AosDocidShuffler : public AosDataShuffler
{
	OmnDefineRCObject;

private:

	int 		mStartPos;
	int 		mLength;
	int 		mVirNumServers;
	int 		mCrtServerNum;

public:
	AosDocidShuffler(const bool flag);
	AosDocidShuffler(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosDocidShuffler();

	virtual AosDataShufflerPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	virtual bool shuffle(
					const AosDataCacherPtr &cacher,
					vector<AosDataCacherPtr> &cachers,
					const AosRundataPtr &rdata);

	virtual bool appendEntry(const char *data, const int len, const AosRundataPtr &rdata);

private:
	virtual bool shuffle(
					const AosDataCacherPtr &cacher,
					vector<AosDataCacherPtr> &cachers,
					const int64_t &start_idx,
					const int64_t &num_entries,
					const AosRundataPtr &rdata);

	int	route(const u64 &distid);

	bool 	shufflerStr(
				const AosDataBlobPtr &blob,
				vector<AosDataCacherPtr> &cachers,
				const AosRundataPtr &rdata);

	bool	shufflerU64(
				const AosDataBlobPtr &blob,
				vector<AosDataCacherPtr> &cachers,
				const AosRundataPtr &rdata);

	bool	shufflerU32(
				const AosDataBlobPtr &blob,
				vector<AosDataCacherPtr> &cachers,
				const AosRundataPtr &rdata);

	bool	shufflerStr(
				const AosDataBlobPtr &blob,
				vector<AosDataCacherPtr> &cachers,
				const int64_t &start_idx,
				const int64_t &num_entries,
				const AosRundataPtr &rdata);

	bool	shufflerU64(
				const AosDataBlobPtr &blob,
				vector<AosDataCacherPtr> &cachers,
				const int64_t &start_idx,
				const int64_t &num_entries,
				const AosRundataPtr &rdata);

	bool	shufflerU32(
				const AosDataBlobPtr &blob,
				vector<AosDataCacherPtr> &cachers,
				const int64_t &start_idx,
				const int64_t &num_entries,
				const AosRundataPtr &rdata);
};
#endif

