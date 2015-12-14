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
// 2015/01/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataProcUtil_DataProcIndexEntry_h
#define AOS_DataProcUtil_DataProcIndexEntry_h

class AosDataProcIndexEntry 
{
public:
	vector<OmnString>	key_fields;
	AosDataRecord *		output_record;
	bool				ignore_null;
	OmnString			field_sep;
	AosDataCollectorPtr	data_collector;
	bool				is_dirty;
	OmnString			iil_name;

public:
	AosDataProcIndexEntry(AosRundata *rdata, 
			const int event, 
			const int version, 
			const AosBuffPtr &buff);

	bool filterOut(AosRundata *rdata, AosDataRecord *record);
	bool appendEntry(AosRundata *rdata);
	void reset() {is_dirty = false;}
};

#endif
