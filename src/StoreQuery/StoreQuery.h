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
//	12/07/2009	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_StoreQuery_StoreQuery_h
#define Omn_StoreQuery_StoreQuery_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Util/String.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"
#include "Util/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/OmnNew.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include "QueryUtil/QrUtil.h"

#define AOSBATCHQUERY_SAVEFILE        "savefile"
#define AOSBATCHQUERY_SAVEBINARYDOC   "savebinarydoc"

class AosStoreQuery : virtual public OmnRCObject, public AosMemoryCheckerObj
{
	OmnDefineRCObject;

	struct sSaveType
	{
		enum E
		{
			eSaveFile,
			eSaveBinaryDoc,
			eInvalid
		};

		OmnString toStr(const E code)
		{
			switch (code)
			{
			case eSaveFile:
				return AOSBATCHQUERY_SAVEFILE;
			case eSaveBinaryDoc:
				return AOSBATCHQUERY_SAVEBINARYDOC;
			default: 
				return "invalid";
			}
			return "invalid";
		}
		static	E toEnum(const OmnString &name)
		{
			if (name == AOSBATCHQUERY_SAVEFILE) 	return eSaveFile;
			if (name == AOSBATCHQUERY_SAVEBINARYDOC) 	return eSaveBinaryDoc;
			return eInvalid;
		}
	};

	struct config
	{
		OmnString		rsltdoc_ctnr;
		OmnString		binaryctnr;
		int				max_records;
		int				blocksize; //total entries of every block
		AosXmlTagPtr	querynode;
		AosXmlTagPtr	replace_entries;
		AosXmlTagPtr	querydoc;
		AosXmlTagPtr	schedulenode;
		AosXmlTagPtr	recordformat;
		AosXmlTagPtr	resultnode;
		AosXmlTagPtr	downloaddoc;
		bool			hasquerydoc;
		OmnString 		savetype;
	};

private:
	struct config		mConfig;
	AosBuffPtr			mIndexBlock;  //index (u32*1000)
	AosBuffPtr			mBodyBlock;  //records
	AosXmlTagPtr		mQueryResultDoc;
	u64					mBlocksSize;
	OmnFilePtr			mUserFile;
	vector<AosQrUtil::FieldDef> mFieldDef;
	int					mBlockIdx;
	int					mAllRecords;
	AosRundataPtr       mRundata;


	bool	setConfig(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	bool	openUserFile( OmnString &filepath, 
						  OmnString &filename);
	bool 	createUserFile();
	bool	updateRate(int &total, int &procnum);
	bool	saveHeadToUserFile();
	bool	saveBlockToUserFile(const vector<AosBuffPtr> &batchdatas, int &batchnum, int &fieldnums, AosRundataPtr &rdata);

public:
	AosStoreQuery(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	~AosStoreQuery();
	bool	runQuery();
	bool	saveBlock(const vector<AosBuffPtr> &batchdatas, int &records, int &fieldnums);
	bool 	saveBlockToUserFile(const vector<AosBuffPtr> &batchdatas, int &records, int &fieldnums);
	bool	saveBlockToBinaryDoc(const vector<AosBuffPtr> &batchdatas, int &records, int &fieldnums);
	bool	saveQueryResultDoc();
	AosXmlTagPtr	getQueryResultDoc(const AosRundataPtr &rdata);
};
#endif




