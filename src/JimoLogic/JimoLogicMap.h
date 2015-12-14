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
// 08/08/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogic_JLMap_h
#define AOS_JimoLogic_JLMap_h

#include "JimoProg/Ptrs.h"
#include "JimoLogic/JimoLogic.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "AosConf/JobTask.h"  
#include "AosConf/Job.h"
#include "AosConf/DataSet.h"
#include <vector>

class AosJLMap: public AosJimoLogic
{
	OmnDefineRCObject;

public:
	AosJLMap(
			const boost::shared_ptr<AosConf::DataSet> &inputds, 
			const AosXmlTagPtr &mapNode, 
			const OmnString &tableName,
			map<OmnString, AosXmlTagPtr> &table_fileds);
	AosJLMap(int version);
	~AosJLMap();

	virtual AosJimoPtr cloneJimo()const {return 0;}
};
#endif

