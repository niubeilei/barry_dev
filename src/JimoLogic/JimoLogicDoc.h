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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogic_JimoLogicDoc_h
#define AOS_JimoLogic_JimoLogicDoc_h

#include "JimoProg/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "JimoLogic/JimoLogic.h"
#include "Thread/Ptrs.h"
#include "AosConf/DataSet.h" 
#include "SEUtil/JqlTypes.h"
#include <vector>
#include <map>

class AosJLDoc: public AosJimoLogic
{
public:
	AosJLDoc(const boost::shared_ptr<AosConf::DataSet> &inputds, 
			list<string> &fields, const string &tablename,
			map<OmnString, AosXmlTagPtr> &table_fileds,
			JQLTypes::OpType op=JQLTypes::eCreate);
	AosJLDoc(int version);
	~AosJLDoc();

	virtual AosJimoPtr cloneJimo()const {return 0;}
};
#endif

