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
#ifndef AOS_TaskUtil_ReduceTaskCreator_h
#define AOS_TaskUtil_ReduceTaskCreator_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/LogicTaskObj.h"
#include "TaskUtil/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

#include <set>
#include <vector>
#include <list>
#include <map>

class AosReduceTaskCreator : virtual public OmnRCObject
{
	OmnDefineRCObject;
public:
	struct ReduceConf
	{
		OmnString 		mType;
		AosXmlTagPtr	mOutputDataset;
	};

protected:
	OmnString		mType;
	list<AosXmlTagPtr> 			mInfoList;
	
public:
	AosReduceTaskCreator();
	AosReduceTaskCreator(const OmnString &tt);
	~AosReduceTaskCreator();

	void setInfoList(list<AosXmlTagPtr> infoList);
	list<AosXmlTagPtr> getInfoList();
	void initInfoList();

	static bool checkConfigStatic(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &logic_task_conf,
		set<OmnString> &data_col_ids);

	static bool checkColIdConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &reduce_tags,
		set<OmnString> &data_col_ids);

	static bool parse(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &logic_task_conf,
		map<OmnString, ReduceConf> &reduce_task_type);

	static bool dataColConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &reduce_tags,
		const OmnString &type,
		map<OmnString, ReduceConf> &reduce_task_type);

	static AosXmlTagPtr create(
		const AosRundataPtr &rdata,
		map<OmnString, ReduceConf> &reduce_map,
		const int physical_id,
		const AosXmlTagPtr &datacol_tag,
		list<AosXmlTagPtr> &info_list);

	static  AosXmlTagPtr create(
		const AosRundataPtr &rdata,
		const int physical_id,
		const AosXmlTagPtr &datacol_tag,
		list<AosXmlTagPtr> &info_list);

	static bool	create(
			const AosRundataPtr &rdata,
			const AosJobObjPtr &job,
			const AosLogicTaskObjPtr &logic_task,
			const AosXmlTagPtr &logic_task_conf);

	static AosXmlTagPtr create(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &dataproc_tag);

	static bool modifyReduceTaskDoc(
			const AosRundataPtr &rdata,
			AosXmlTagPtr &reduce_task_doc,
			const AosJobObjPtr &job,
			const OmnString &logic_task_id,
			const OmnString &new_task_id,
			const int physical_id,
			const int max_num_thrd,
			const OmnString snapshotStr);

	static AosXmlTagPtr getOutputDatasetsTag(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf,
		const OmnString &name);

protected:
	virtual bool		serializeTo(
							AosXmlTagPtr &xml,
							const AosRundataPtr &rdata) = 0;
	virtual bool		serializeFrom(
							const AosXmlTagPtr &xml,
							const AosRundataPtr &rdata) = 0;
};

#endif

#endif
