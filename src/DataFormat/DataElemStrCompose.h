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
// 05/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to DataField. Chen Ding, 07/08/2012
#ifndef Aos_DataFormat_DataElemStrCompose_h
#define Aos_DataFormat_DataElemStrCompose_h

#include "DataFormat/DataElem.h"
#include "DataFormat/DataProcStatus.h"
#include "IILClient/Ptrs.h"


class AosDataElemStrCompose : virtual public AosDataElem
{
private:
	int			mCount;

public:
	AosDataElemStrCompose(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosDataElemStrCompose();
	
	virtual bool 	setRecordByPrevValue(const AosDataRecord *record);
	virtual bool	setValueToRecord(
						const AosDataRecord *record,
						const AosValueRslt &value, 
						const AosRundataPtr &rdata);

	virtual bool	getValueFromRecord(
						const AosDataRecord *record,
						AosValueRslt &value, 
						const bool copy_flag,
						const AosRundataPtr &rdata);

	virtual bool	serializeFrom( 	
						OmnString &docstr,
						int &idx, 
						const char *data, 
						const int datalen, 
						const AosRundataPtr &rdata);

	virtual bool	serializeTo(
						const AosXmlTagPtr &doc, 
						const AosBuffPtr &buff, 
						const AosRundataPtr &rdata);
	
	virtual AosDataProcStatus::E convertData(
						const char *record,
						const int data_len,
						int &idx,
						const bool need_convert,
						const AosBuffPtr &buff, 
						const bool need_value,
						AosValueRslt &value,
						const AosRundataPtr &rdata);

	virtual bool	createRandomValue(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);
			
	virtual bool saveAndClear();
	// virtual bool setRecord(char *record, const int len);
	virtual bool updateData(const AosDataRecordPtr &record, bool &updated);
	// virtual u64 getU64(const char *record, const u64 &dft) const;
	// virtual const char * getCharStr(const char *record, int &idx, int &len);

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif
#endif
