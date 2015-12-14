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
#ifndef AOS_JimoLogic_JLIndex_h
#define AOS_JimoLogic_JLIndex_h

#include "JimoProg/Ptrs.h"
#include "JimoLogic/JimoLogic.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "AosConf/JobTask.h"  
#include "AosConf/Job.h"
#include "AosConf/DataSet.h"
#include "SEUtil/JqlTypes.h"
#include <vector>

class AosJLIndex: public AosJimoLogic
{
	OmnDefineRCObject;

public:
	AosJLIndex(
			const AosRundataPtr &rdata,
			const boost::shared_ptr<AosConf::DataSet> &inputds, 
			const AosXmlTagPtr &def,
			const string &tablename,
			map<OmnString, AosXmlTagPtr> &table_fileds,
			JQLTypes::OpType op=JQLTypes::eCreate);
	AosJLIndex(int version);
	~AosJLIndex();

	virtual AosJimoPtr cloneJimo()const {return 0;}
};
#endif

