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
// 2013/11/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataFile_DataFileMySQL_h
#define AOS_DataFile_DataFileMySQL_h

#include "DataFile/DataFile.h"


class AosDataFileMySQL : virtual public AosDataFile
{
	OmnDefineRCObject;

private:

public:
	AosDataFileMySQL();
	AosDataFileMySQL(const AosRundataPtr &rdata, 
				const AosXmlTagPtr &def);
	AosDataFileMySQL(const AosRundataPtr &rdata, 
				const OmnString &objid);
	~AosDataFileMySQL();

	virtual int getPhysicalId() const;
};
#endif
