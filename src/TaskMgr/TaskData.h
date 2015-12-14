////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskMgr_TaskData_h
#define AOS_TaskMgr_TaskData_h

#include "SEInterfaces/TaskDataObj.h"


class AosTaskData : public AosTaskDataObj
{
	OmnDefineRCObject;

protected:
	AosTaskDataType::E	mType;
	OmnString			mName;

public:
	AosTaskData(
		const AosTaskDataType::E type, 
		const OmnString &name, 
		const bool flag);
	~AosTaskData();

	static bool	staticInit();
	static AosTaskDataObjPtr createTaskData(
								const AosXmlTagPtr &def,
								const AosRundataPtr &rdata);
	static AosTaskDataObjPtr serializeFromStatic(
								const AosXmlTagPtr &xml,
								const AosRundataPtr &rdata);

	static AosTaskDataObjPtr serializeFromBuffStatic(
								const AosBuffPtr &buff,
								const AosRundataPtr &rdata);

	virtual AosTaskDataType::E	getTaskDataType() const {return mType;};
	virtual OmnString			getFileName() const {return "";}
	virtual int					getPhysicalId() const {return -1;}

	//Jozhi 2014-03-10 for snapshot
	virtual u64					getSnapShotId() const { return 0; }
	virtual u32					getVirtualId() const { return 0; }
	virtual set<OmnString>		getIILNames() const { set<OmnString> iilnames; return iilnames; }
	virtual AosDocType::E		getDocType() const { return AosDocType::eNormalDoc; }
	virtual u64					getTaskDocid() const { return 0; }
	virtual bool				getIsCommit() const { return false; }
	
	virtual bool				isDataExist(const AosRundataPtr &rdata);

	virtual AosDataCacherObjPtr createDataCacher(
								const AosDataRecordObjPtr &record,
								const AosRundataPtr &rdata);
	
	virtual AosDataScannerObjPtr createDataScanner(const AosRundataPtr &rdata);

	virtual bool isTheSameTaskData(
								const AosTaskDataObjPtr &task_data,
								const AosRundataPtr &rdata);

	virtual OmnString getStrKey(const AosRundataPtr &rdata);

private:
	bool	registerTaskData(
				const AosTaskDataType::E type, 
				const OmnString &name, 
				const AosTaskData *data);

protected:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
	bool	serializeTo(
				AosXmlTagPtr &xml,
				const AosRundataPtr &rdata);
	bool 	serializeFrom(
				const AosXmlTagPtr &xml,
				const AosRundataPtr &rdata);

	bool	serializeTo(
				AosBuffPtr &buff,
				const AosRundataPtr &rdata);

	bool 	serializeFrom(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata);
};
#endif
