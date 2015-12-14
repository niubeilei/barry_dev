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
// 07/27/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_TaskUtil_MapTaskCreator_h
#define AOS_TaskUtil_MapTaskCreator_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/LogicTaskObj.h"
#include "TaskUtil/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

#include <set>
#include <vector>

class AosMapTaskCreator : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
public:
	AosMapTaskCreator();
	~AosMapTaskCreator();

	// Ketty 2013/12/26
	static bool checkConfigStatic(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &logic_task_conf,
			set<OmnString> &data_col_ids);

	static bool	create(
			const AosRundataPtr &rdata,
			const AosJobObjPtr &job,
			const AosLogicTaskObjPtr &logic_task,
			const AosXmlTagPtr &logic_task_conf);

	// Ketty 2014/08/21
	//static AosXmlTagPtr getDataColConf(
	//		const AosRundataPtr &rdata,
	//		const AosXmlTagPtr &datacol_conf,
	//		const AosXmlTagPtr &dataset_conf);

private:
	static bool checkInputDatasetConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &conf);
	
	static bool checkDataCollectorConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &logic_task_conf,
			set<OmnString> &collector_ids);

	static bool checkMapTaskActionsConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &logic_task_conf,
			set<OmnString> &data_col_ids);

	// Ketty 2014/08/21
	static AosXmlTagPtr getDataColConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &orig_datacol_confs,
			const AosXmlTagPtr &dataset_conf);

	static AosXmlTagPtr getInputDatasetsTag(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &conf);

	static AosXmlTagPtr getMapTaskActionsConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &conf);

	static AosXmlTagPtr createTaskDoc(
			const AosRundataPtr &rdata,
			const AosJobObjPtr &job,
			const OmnString &logic_task_id,
			const OmnString &new_task_id,
			const OmnString &task_sdoc_objid,
			const int max_num_thrd,
			const int physical_id,
			const OmnString &dataset_conf,
			//const AosXmlTagPtr &data_col_conf,
			const OmnString &data_col_conf,
			const AosXmlTagPtr &task_action_conf);

	//static OmnString getDataColConfStr(const AosXmlTagPtr data_col_conf);

	static int getPhysicalId(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &dataset_conf_unit);

	// Ketty 2014/08/21
	static int getCubeIdFromDataset(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &datasets_conf);
	
	// Ketty 2014/08/21
	//static bool getGroupDataCol(
	//		const OmnString &grp_type,  // Ketty 2014/08/14
	//		const AosXmlTagPtr &data_col_tags,
	//		const AosXmlTagPtr &datacol_conf); 
};

#endif

#endif
