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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtTask_H
#define AOS_JQLStatement_JqlStmtTask_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtTask : public AosJqlStatement
{
private:
	//data from JQLParser
	OmnString		mTaskName;
	AosExprList*  	mInputDataSetNames; 
	AosExprList*  	mOutputDataSetNames; 
	AosExprList*  	mDataProcsNames; 
	AosExprList*  	mActionNames; 
	bool			mMapTaskUseSingleFlag;

public:
	AosJqlStmtTask();
	~AosJqlStmtTask();

	//getter/setters
	void setTaskName(OmnString name);
	void setInputDataSetNames(AosExprList *input_dataset_names);
	void setOutputDataSetNames(AosExprList *output_dataset_names);
	void setDataProcsNames(AosExprList *data_procs_names);
	void setActionNames(AosExprList *action_names);
	void setMapTaskUseSignle() {mMapTaskUseSingleFlag = true;}

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createTask(const AosRundataPtr &rdata);  
	bool showTasks(const AosRundataPtr &rdata);   
	bool describeTask(const AosRundataPtr &rdata);
	bool dropTask(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

	bool composeXml(                    
			const AosRundataPtr &rdata,            
			const OmnString &tagname,              
			const OmnString &child_tagname,        
			const OmnString &attr_name,            
			const AosXmlTagPtr &doc,               
			vector<OmnString> &fields,             
			map<OmnString, OmnString> &alias_name,
			int* idx);
};

#endif
