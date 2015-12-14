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
// 09/11/2015 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryInto_QueryIntoLocalFile_h
#define Aos_QueryInto_QueryIntoLocalFile_h

#include "SEInterfaces/QueryIntoObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Jimo/Jimo.h"

#include "Util/File.h"
#include "Util/RCObjImp.h"

#include "QueryInto/Ptrs.h"


class AosQueryIntoLocalFile : public AosQueryIntoObj, public AosJimo
{
	OmnDefineRCObject;
private:
	enum
	{
		eDftBuffMaxSizeToSend = 10000000,
		eDftMaxFileSize = 500000000
	};

	OmnString					mFileName;
	AosBuffPtr					mBuff;
	OmnFilePtr					mFile;
	u32							mFileNum;

public:
	AosQueryIntoLocalFile();
	AosQueryIntoLocalFile(const int ver);
	AosQueryIntoLocalFile(const AosQueryIntoLocalFile &queryInto);
	~AosQueryIntoLocalFile();

	AosJimoPtr cloneJimo() const ;
	AosQueryIntoObjPtr clone() const ;

	bool	config(
			const AosXmlTagPtr &def,
			AosRundata *rdata);

	bool 	appendEntry(
				AosDataRecordObj *record,
		        AosRundata *rdata);

	bool	flush(AosRundata *rdata);
private:
	bool 	createFile(AosRundata *rdata);
	bool 	checkDirIsExist(AosRundata *rdata);

};

#endif
