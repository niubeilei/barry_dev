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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2012/05/11	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSnapShot.h"

#include "Alarm/Alarm.h"
//#include "API/AosApiG.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SmartDoc/SmartDoc.h"
#include "UtilData/ModuleId.h"
#include "Util/AppendFile.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosActSnapShot::AosActSnapShot(const bool flag)
:
AosSdocAction(AOSACTTYPE_SNAPSHOT, AosActionType::eSnapShot, flag)
{
}


AosActSnapShot::AosActSnapShot(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_STRIILBATCHINC, AosActionType::eStrIILBatchInc, false)
{
	bool rslt = config(def, rdata);
	if(!rslt) OmnThrowException(rdata->getErrmsg());
}


AosActSnapShot::~AosActSnapShot()
{
	//OmnScreen << "act snap shot deleted" << endl;
}


bool
AosActSnapShot::config(
        const AosXmlTagPtr &def,
        const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	mSnapShotType = def->getAttrStr("snapshot_type");
	aos_assert_r(mSnapShotType != "", false);

	mModeId = def->getAttrStr(AOSTAG_MODULEID);
	aos_assert_r(mModeId != "", false);

	mKey = def->getAttrStr(AOSTAG_KEY);
	aos_assert_r(mKey != "", false);

	if (mSnapShotType == "commit")
	{
		AosXmlTagPtr files_tag = def->getFirstChild("files");
		aos_assert_r(files_tag, false);

		AosXmlTagPtr file_tag = files_tag->getFirstChild(true);
		aos_assert_r(file_tag, false);
		
		mFileIds.clear();
		mPhysicalIds.clear();
		while(file_tag)
		{
			u64 fileId = file_tag->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
			aos_assert_r(fileId != 0, false);

			int physicalId = file_tag->getAttrInt(AOSTAG_PHYSICALID, -1);
			aos_assert_r(physicalId != -1, false);

			mFileIds.push_back(fileId);
			mPhysicalIds.push_back(physicalId);

			file_tag = files_tag->getNextChild();
		}
	}

	return true;
}

bool
AosActSnapShot::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if(!task)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mTask = task;

	bool rslt = true;
	if (mSnapShotType == "create")
	{
		if (mModeId == "iil")
		{
			rslt = createIILSnapShot(rdata);
		}
		else if(mModeId == "doc")
		{
			rslt = createDocSnapShot(rdata);
		}
		else
		{
			OmnAlarm << "Invalid type : " << mModeId << enderr;
			return false;
		}
		aos_assert_r(rslt, false);
	}
	else if(mSnapShotType == "commit")
	{
		OmnTagFuncInfo << endl;
		rslt = commitSnapShot(rdata);	
		aos_assert_r(rslt, false);

		// deleteFile
		rslt = deleteFiles(rdata);
		aos_assert_r(rslt, false);
	}
	else
	{
		OmnAlarm << " Invalid snap shot type : " << mSnapShotType << enderr;
		return false;
	}
	

	AosActionObjPtr thisptr(this, true);
	mTask->actionFinished(thisptr, rdata);
	mTask = 0;

	return true;
}

bool
AosActSnapShot::createIILSnapShot(const AosRundataPtr &rdata)
{
	int virtual_id = AosGetCubeId(mKey);
		
	OmnString key;
	key << virtual_id << "__" << AosModuleId::toStr(AosModuleId::eIIL);

	AosTaskDataObjPtr task_data = mTask->getOutPut(AosTaskDataType::eOutPutSnapShot, key, rdata); 
	if (!task_data)
	{
		//create snapshot
		u32 snap_id = 0;
		bool rslt = false; //AosIILClientObj::getIILClient()->createSnapShot(mKey, snap_id, virtual_id, false, rdata);
		aos_assert_r(rslt, false);

		AosTaskDataObjPtr taskData = AosTaskDataObj::createTaskDataOutPutSnapShot(
				virtual_id, snap_id);

		aos_assert_r(snap_id != 0, false);
		aos_assert_r(taskData, false);

		mTask->updateTaskOutPut(taskData, rdata);                                     
	}

	return true;
		
}

bool
AosActSnapShot::createDocSnapShot(const AosRundataPtr &rdata)
{
return true;
	vector<u32> virtualIds;
	bool rslt = AosGetLocalVirtualIds(virtualIds); 
	aos_assert_r(rslt, false);
	aos_assert_r(virtualIds.size() > 0, false);

	for(u32 i=0; i<virtualIds.size(); i++)
	{
		OmnString key;
		
		// ?????????
		key << virtualIds[i] << "__" << AosModuleId::toStr(AosModuleId::eDoc);

		AosTaskDataObjPtr task_data =  mTask->getOutPut(AosTaskDataType::eOutPutSnapShot, key, rdata); 
		if (!task_data)
		{
			//create snapshot
			u32 snap_id = 0;

			// ????????
			//bool rslt = AosIILClientObj::getIILClient()->createSnapShot(mKey, snap_id, virtual_id, rdata);
			//aos_assert_r(rslt, false);

			AosTaskDataObjPtr taskData = AosTaskDataObj::createTaskDataOutPutSnapShot(
					virtualIds[i], snap_id);

			aos_assert_r(snap_id != 0, false);
			aos_assert_r(taskData, false);

			mTask->updateTaskOutPut(taskData, rdata);                                     
		}
	}

	return true;
}

/*
bool
AosActSnapShot::createSnapShot(
		const u32 virtual_id,
		const AosRundataPtr &rdata)
{
	u32 snap_id = 0;
	AosTaskDataObjPtr taskData;
	if (mModeId == "iil")
	{
		bool rslt = AosIILClientObj::getIILClient()->createSnapShot(mKey, snap_id, virtual_id, rdata);
		aos_assert_r(rslt, false);

		taskData = AosTaskDataObj::createTaskDataOutPutSnapShot(
				AosGetSelfServerId(), virtual_id, AosModuleId::eIIL, snap_id);
	}
	else if(mModeId == "doc")
	{
		//????????
		OmnAlarm << "Not Complete!" << enderr;
		taskData = AosTaskDataObj::createTaskDataOutPutSnapShot(
				AosGetSelfServerId(), virtual_id, AosModuleId::eDoc, snap_id);
	}

	aos_assert_r(snap_id != 0, false);
	aos_assert_r(taskData, false);

	mTask->updateOutPut(taskData, rdata);                                     
	
	return true;
}
*/
bool
AosActSnapShot::commitSnapShot(const AosRundataPtr &rdata)
{
	//commit snapshot
	bool rslt = true;

	OmnTagFuncInfo << endl;

	if (mModeId == "iil")
	{
		rslt = false; //AosIILClientObj::getIILClient()->commitSnapShot(mKey, rdata);
	}
	else if(mModeId == "doc")
	{
		//???????
		OmnAlarm << "Not Commplete" << enderr;
	}
	else
	{
		OmnAlarm << "Invalid type : " << mModeId << enderr;
		return false;
	}
	
	aos_assert_r(rslt, false);

	return true;
}

bool
AosActSnapShot::deleteFiles(const AosRundataPtr &rdata)
{
	if (mFileIds.size() == 0) return true;
		
	aos_assert_r(mFileIds.size() > 0, false);
	aos_assert_r(mFileIds.size() == mPhysicalIds.size(), false);

	bool rslt = true;
	bool svr_death;
	for(u32 i=0; i<mFileIds.size(); i++)
	{
		aos_assert_r(mFileIds[i] != 0, false);
		aos_assert_r(mPhysicalIds[i] != -1, false);
		rslt =  AosNetFileCltObj::deleteFileStatic(
			mFileIds[i], mPhysicalIds[i], svr_death, rdata.getPtr());
		aos_assert_r(rslt, false);
	}
	return true;
}

AosActionObjPtr
AosActSnapShot::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSnapShot(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

