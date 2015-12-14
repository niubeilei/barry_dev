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
// 2013/05/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDocDynamic_SdocCreateFile_h
#define Aos_SmartDocDynamic_SdocCreateFile_h

#include "Rundata/Ptrs.h"
#include "TransUtil/TaskTrans.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
using namespace std;

class AosSdocCreateFile 
{
public:
	static bool createFile(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &sdoc, 
					const int orig_phy_id, 
					const OmnString &dir);
};

class AosTransSdocCreateFile : virtual public AosTaskTrans 
{
	OmnDefineRCObject;

private:
	int				mReqServerId;
	AosXmlTagPtr	mSdoc;

public:
	AosTransSdocCreateFile(const bool regflag);
	AosTransSdocCreateFile(
			const AosRundataPtr &rdata, 
			const int phy_id, 
			const AosXmlTagPtr &sdoc);
	~AosTransSdocCreateFile();

	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
};
#endif

