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
// Each instance of AosSyncherSub has:
// 1. Event: the reason why this instance was created,
// 2. Jimo: the jimo to process the data
//	
// Modification History:
// 2015/01/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SyncEngine/IndexDocsCreated.h"


extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosIndexDocsCreated_1(
			const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosIndexDocsCreated(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosIndexDocsCreated::AosIndexDocsCreated(const int version)
:
AosJimoDataProc(version)
{
}


AosIndexDocsCreated::~AosIndexDocsCreated()
{
}


bool
AosIndexDocsCreated::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// The JQL is:
	// 	CREATE DATAPROC "dp1"
	//	{
	//		type:index, 
	//		key_fields:[t1.f1, t1.f2], 
	//		docidfield:t1.docid,
	//		key_sep:"|", 
	//		condition:"t1.f1>10"
	//	};
	// 
	// The JSON object is saved in 'def'.
	return true;
}


AosJimoPtr 
AosIndexDocsCreated::cloneJimo() const
{
	return OmnNew AosIndexDocsCreated(*this);
}


AosDataProcStatus::E
AosIndexDocsCreated::procData(
		AosRundata *rdata, 
		const AosRecordsetPtr &recordset, 
		vector<AosDataCollector> &data_collectors, 
		AosDataProcStat &statistics)
{
	// This function updates the indexes for 'record'. 
	AosValueRslt vv;
	recordset->reset();
	AosDataRecord *record;
	while (recordset->nextRecord(rdata, record))
	{
		if (!record) break;

		AosBuff key;
		for (u32 i=0; i<mIndexes.size(); i++)
		{
			AosDataProcIndexEntry &index = mIndexes[i];
			if (index.filterOut(rdata)) continue;

			AosRecordObj *entry = index.mRecordRaw;
			entry->clear();
			bool ignore = false;
			for (u32 j=0; j<index.mValueIdxs.size(); j++)
			{
				bool rslt = record->getFieldValue(rdata, index.mValueIdxs[j], vv);
				if (rslt)
				{
					if (vv.isNull() && index.mIgnoreNull) 
					{
						ignore = true;
						break;
					}

					if (j > 0) value.setOmnStr(index.mFieldSep);
					value.serializeTo(rdata, key);
				}
				else
				{
					xxx;
				}
			}

			// Get the docid
			u64 docid = record->getDocid();
			if (!ignored) index.appendEntry(rdata, key, docid);
		}
	}

	return true;
}

