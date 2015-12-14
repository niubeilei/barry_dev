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
// 2015/05/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicDatasetTable_h
#define AOS_JimoLogicNew_JimoLogicDatasetTable_h

#include "JimoLogicNew/JimoLogicDatasetTable.h"
#include "JimoLogicNew/JimoLogicDataset.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDatasetTable : virtual public AosJimoLogicDataset
{
	OmnDefineRCObject;

protected:
	//select type
	OmnString		    			mType;
	vector<AosExprObjPtr> 			mNameValueList;  //save a list of name:value which have been parsed
    OmnString           			mDatasetName;    //save dataSetDir name
	OmnString           			mTableName;
public:
	AosJimoLogicDatasetTable(const int version);
	~AosJimoLogicDatasetTable();

	virtual bool parseJQL(
			AosRundata *rdata,
			AosJimoParserObj *jimo_parser,
			AosJimoProgObj *prog,
			bool &parsed,
			bool dft = false);


	// AosGenericObj interface
	virtual bool run(AosRundata *rdata,
					AosJimoProgObj* job,
					//const OmnString &verb_name,
					OmnString &statements_str,
					bool isparser);

    virtual bool compileJQL(
                    AosRundata *rdata,
                    AosJimoProgObj *prog);

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;



private:
	bool createDatasetTableDoc(
			AosRundata *rdata);

};
#endif

