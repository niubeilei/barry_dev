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
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataFormat_DataFormatter_h
#define Aos_DataFormat_DataFormatter_h

#include "DataFormat/DataFormatType.h"
#include "DataFormat/Ptrs.h"
#include "DataField/DataField.h"
#include "ImportData/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataProcObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
using namespace std;

class AosDataFormatter : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	AosDataFormatType		mType;
	OmnString				mName;
	vector<AosDataFieldObjPtr>	mFields;
	vector<AosDataProcObjPtr>	mDataProcs;
	int						mRecordLength;
	bool					mFixedLength;
	bool					mDataTooShort;

public:
	AosDataFormatter();
	AosDataFormatter(const AosDataFormatType type);
	AosDataFormatter(
		const AosDataFormatType type,
		const AosXmlTagPtr &formatter,
		const AosRundataPtr &rdata);
	~AosDataFormatter();

	AosDataFieldObjPtr	getElement(const int idx, const AosRundataPtr &rdata);	
	virtual AosXmlTagPtr serializeToXmlDoc(
	 		                const char *data,
	 						const int data_len,
	 						const AosRundataPtr &rdata) = 0;

	virtual AosBuffPtr serializeToBuff(
	 		                const AosXmlTagPtr &doc,
	 						const AosRundataPtr &rdata) = 0;

	// virtual bool getValue(	const char *record, 
	// 						const int record_len, 
	// 						const int field_idx, 
	// 						AosValueRslt &value) const;
	// virtual u64 getU64(const char *record, const int, const int idx, const u64 &dft) const;
	// virtual u32 getU32(const char *record, const int, const int idx, const u32 dft) const;
	// virtual char getChar(const char *record, const int, const int idx, const char dft) const;
	// virtual const char * getCharStr(
	// 					const char * const record, 
	// 					const int record_len,
	// 					const int idx, 
	// 					int &data_len,
	// 					const char * const dft, 
	// 					const int dft_len) const;
	
	bool createRandomDoc(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	int getRecordLength() const {return mRecordLength;}

	AosDataProcStatus::E convertData(
						const char *record, 
						const int data_len,
						const AosBuffPtr &buff,
						const u64 &docid,
						const AosImportDataThrdPtr &task,
						const AosRundataPtr &rdata);

	static AosDataFormatterPtr 	getFormatter(
				        const AosXmlTagPtr &xml,
						const AosRundataPtr &rdata);

	// Chen Ding, 07/14/2012
	bool resolveIILAssemblers(
						const vector<AosDataAssemblerObjPtr> &iil_assembers, 
						const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &formatter, const AosRundataPtr &rdata);
};

#endif

