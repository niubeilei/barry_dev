////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/07/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoDataset_JimoDataset_h
#define Aos_JimoDataset_JimoDataset_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include "SEInterfaces/GenericObj.h"
#include <vector>
#include <hash_map>
using namespace std;


class AosJimoDataset : public AosGenericObj
{
protected:

public:
	AosJimoDataset(
		const int version,
		const AosJimoType::E type);
	~AosJimoDataset();
	virtual OmnString getObjType() const {return 0;}
	virtual bool proc(
					AosRundata *rdata, 
					const OmnString &verb,
					const OmnString &obj_name, 
					const vector<AosGenericValueObjPtr> &parms){return true;}

	//arvin 2015/04/20
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false)
	{
		OmnShouldNeverComeHere;  
		return false;
	}
	
	virtual bool setJimoName(const OmnString &name)
	{
		OmnShouldNeverComeHere;  
		return false;	
	}

	virtual OmnString getJimoName() const
	{
		OmnShouldNeverComeHere;  
		return "";
	}

	virtual OmnString getObjType(AosRundata *rdata)
	{
		OmnShouldNeverComeHere;  
		return "";
	}


};

#endif
