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
#ifndef AOS_DataShuffler_DistMapShuffler_h
#define AOS_DataShuffler_DistMapShuffler_h

#include "DataShuffler/DataShuffler.h"
#include "Util/BuffArray.h"

class AosDistMapShuffler : public AosDataShuffler
{
	OmnDefineRCObject;

private:
	int					mStartPos;
	int 				mLength;
	AosBuffArrayPtr		mMap;
	bool				mSortMap;

public:
	AosDistMapShuffler(const bool flag);
	AosDistMapShuffler(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosDistMapShuffler();

	virtual AosDataShufflerPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	virtual bool shuffle(
					const AosDataCacherPtr &cacher,
					vector<AosDataCacherPtr> &cachers,
					const AosRundataPtr &rdata);

	virtual bool appendEntry(const char *data, const int len, const AosRundataPtr &rdata);

	void updateMap(const AosBuffArrayPtr &mm){mMap = mm;}

private:
	virtual bool shuffle(const AosDataCacherPtr &cacher,
					vector<AosDataCacherPtr> &cachers,
					const int64_t &start_idx,
					const int64_t &num_entries,
					const AosRundataPtr &rdata);

	bool 	shufflerStr(const AosDataBlobPtr &blob,
					vector<AosDataCacherPtr> &cachers,
					const AosRundataPtr &rdata);

	bool 	shufflerStr(const AosDataBlobPtr &blob,
					vector<AosDataCacherPtr> &cachers,
					const int64_t &start_idx,
					const int64_t &num_entries,
					const AosRundataPtr &rdata);

	int 	routeByStr(const char *record, const int record_len);
	int		config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

