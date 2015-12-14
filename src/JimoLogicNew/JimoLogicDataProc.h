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
// 2015/05/25 Created by Arvin
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicDataProc_h
#define AOS_JimoLogicNew_JimoLogicDataProc_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProc : virtual public AosJimoLogicNew
{
protected:
	OmnString 			mDataProcName;
	vector<OmnString>	mInputs;

public:

	AosJimoLogicDataProc(const int version);
	~AosJimoLogicDataProc();
	
	virtual AosJimoLogicType::E getJimoLogicType() const {return AosJimoLogicType::eDataProc;}
	virtual OmnString getLogicName() {return "";}

/*	
	virtual bool getOutputsFromDataProc(
						AosRundata* rdata,
						AosJimoProgObj *jimo_prog,
						const vector<OmnString> &procNames,
						vector<OmnString> &outputs);
*/
};
#endif

