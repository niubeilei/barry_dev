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
#ifndef Aos_JobTrans_UnicomReadFileTrans_h
#define Aos_JobTrans_UnicomReadFileTrans_h

#include "TransUtil/TaskTrans.h"

class AosUnicomReadFileTrans : virtual public AosTaskTrans
{

private:
	OmnString			mFileName;
	int					mLoopTmp;
	OmnString			mRmDataDir;

public:
	AosUnicomReadFileTrans(const bool regflag);
	AosUnicomReadFileTrans(
			const OmnString &fname,
			const int svr_id);
	~AosUnicomReadFileTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

private:
	bool readXmlFindFile( 
			const AosXmlTagPtr &doc,
			AosBuffPtr &zip_buff,
			vector<AosBuffPtr> &new_buffs,
			vector<AosXmlTagPtr> &new_xmls);

	vector<AosXmlTagPtr> getDomainItem(
	        const AosXmlTagPtr &doc,
			int loopNum);
	
	bool xmlParse(AosXmlTagPtr &xml);

	OmnString unzip(OmnString &zipPath, const OmnString &fname);
};
#endif

