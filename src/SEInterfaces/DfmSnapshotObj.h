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
// 2013/07/16 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DfmSnapshotObj_h
#define Aos_SEInterfaces_DfmSnapshotObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "DfmUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
using namespace std;

struct AosSnapshotReadInfo
{
	AosBuffPtr file_data;
	u64		   file_off;
	bool	   finished;
};

class AosSnapshotSts
{
public:
	enum E
	{
		eActive,
		eCommitting,
		eRollbacking,
		eMergeing,
		eCommitted,
		eRollbacked,
		eMerged,
	};
};

class AosDfmSnapshotObj : virtual public OmnRCObject
{
private:
	//static AosDfmSnapshotObjPtr		smSnapshot;

public:
	virtual bool serializeTo(const AosBuffPtr &buff) = 0;
	virtual bool commitFinish(const AosRundataPtr &rdata) = 0;
	virtual bool rollbackFinish(const AosRundataPtr &rdata) = 0;
	virtual bool mergeFinish(const AosRundataPtr &rdata) = 0;
	virtual bool setStatus(const AosSnapshotSts::E sts) = 0;
	virtual AosSnapshotSts::E getStatus() = 0;
	virtual bool isActive() = 0;
	virtual u64  getSnapId() = 0;
	
	virtual bool readEntry(
					const AosRundataPtr &rdata,
					const u64 docid,
					bool &exist,
					AosDfmDocHeaderPtr &header) = 0;

	virtual bool addEntry(
					const AosRundataPtr &rdata,
					const AosDfmDocHeaderPtr &header,
					const bool is_merge = false) = 0;

	virtual bool removeEntry(
					const AosRundataPtr &rdata,
					const u64 docid) = 0;
	
	virtual bool getEntrys(
					const AosRundataPtr &rdata, 
					vector<AosDfmDocHeaderPtr> &entrys,
					AosSnapshotReadInfo &read_info) = 0;
	
	//static void setDfmSnapshot(const AosDfmSnapshotObjPtr &d) {smSnapshot= d;}
	//static AosDfmSnapshotObjPtr getSnapshot() {return smSnapshot;}
};
#endif
