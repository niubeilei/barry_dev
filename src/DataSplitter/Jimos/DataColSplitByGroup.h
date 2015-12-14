////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataSplitter_Jimos_DataColSplitByGroup_h
#define Aos_DataSplitter_Jimos_DataColSplitByGroup_h

#include "alarm_c/alarm.h"
#include "DataSplitter/DataSplitter.h"
#include "DataCollector/DataCollectorGroup.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"


class AosDataColSplitByGroup : public AosDataSplitter
{
	OmnDefineRCObject;
private:
	enum
	{
		eMaxSplitSize = 10000000000
	};
	
protected:
	map<int64_t, map<int, vector<AosFileInfo> > >			mFileList;
	AosDataCollectorGroup::E								mShuffleType;


public:
	AosDataColSplitByGroup(const u32 version);
	AosDataColSplitByGroup(const OmnString &type, const u32 version);
	~AosDataColSplitByGroup();

	// Jimo	Interface
	virtual bool	config(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);
	virtual AosJimoPtr cloneJimo() const;


	// DataSplitter interface
	virtual bool	config(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc);
	virtual bool	split(
						AosRundata *rdata,
						vector<AosXmlTagPtr> &data_units);

private:
	int	getSvrId(
						const int64_t shuffle_id,
						const int phy_id);

};
#endif



