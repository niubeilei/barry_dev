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
// 2014/10/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GenericObjs_RandomData_CreateRandomData_h
#define Aos_GenericObjs_RandomData_CreateRandomData_h

#include "GenericObjs/GenericObjUtil/GenericMethod.h"
#include "GenericObjs/GenericObjUtil/Ptrs.h"
#include "DataRecord/RecordParmList.h"
#include "DataRecord/Ptrs.h"
#include "Random/RandomUtil.h"
#include <vector>
#include <map>
#include "SEUtil/ParmNames.h"
#include "SEUtil/ParmValues.h"



class AosMethodCreateRandomData : public AosGenericMethod
{
	OmnDefineRCObject;

protected:
	enum
	{
		eDefaultMin = 1,
		eDefaultMax = 1000*1000
	};

    map<OmnString, OmnString> mMapPhone;

public:
	AosMethodCreateRandomData(const int version);
	~AosMethodCreateRandomData();

	virtual bool	config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool	proc(
						AosRundata *rdata,
						const OmnString &obj_name, 
						const vector<AosGenericValueObjPtr> &parms);
private:
	virtual bool generateMapTorturer(
			            AosRundata *rdata,
			            const OmnString &data_name, 
			            const AosDataRecordObjPtr &parms);
	virtual bool generatePhoneNums(
			            AosRundata *rdata,
			            const OmnString &data_name, 
			            const vector<AosExprObjPtr> &parms);
	virtual bool generateCellNums(
			            AosRundata *rdata,
			            const OmnString &data_name, 
			            const vector<AosExprObjPtr> &parms);
	virtual bool createCellNumbers(
			            const int num,
                        const OmnString &separator,
						const vector<OmnString> &prefix_list,
						const OmnString &filename);
	virtual bool writeToFile(
			            const OmnString &phonenumber_list,
						const OmnString &filename);
	vector<OmnString> split(
			           OmnString &str,
					   const char c);
	virtual bool generateNormPhoneNums(
				        AosRundata *rdata,
				        const OmnString &data_name, 
				        const vector<AosExprObjPtr> &parms);
};
#endif

