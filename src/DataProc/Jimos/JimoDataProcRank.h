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
// 2013/07/23 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_Jimos_JimoDataProcRank_h
#define Aos_DataProc_Jimos_JimoDataProcRank_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataProcObj.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include <hash_map>
using namespace std;


class AosJimoDataProcRank : public AosJimo 
{
	OmnDefineRCObject;

private:
	OmnString				mVersion;
	OmnString				mJimoName;
	OmnString				mKeyFieldName;
	int						mKeyFieldIdx;
	AosIILType				mIILType;
	AosConditionObjPtr		mFilter;
	AosDataAssemblerObjPtr	mIILAssembler;

public:
	AosJimoDataProcRank();
	AosJimoDataProcRank(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &jimo_doc, 
			const OmnString &version);
	~AosJimoDataProcRank();
	
	AosJimoDataProcRank(const AosJimoDataProcRank &rhs);

	// AosJimo Interface
	virtual bool run(	const AosRundataPtr &rdata);
	virtual AosJimoType::E getType() const {return AosJimoType::eDataProc;}
	virtual OmnString getVersion() const {return mVersion;}
	virtual AosJimoPtr cloneJimo(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)  const;
	virtual OmnString toString() const;
	virtual OmnString getJimoName() const {return mJimoName;}

	virtual bool config(const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc);

	virtual bool serializeTo(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual bool serializeFrom(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual void * getMethod(const AosRundataPtr &rdata, 
						const OmnString &name, 
						AosMethodId::E &method_id);

private:
	bool init(			const AosRundataPtr &rdata, 
						const AosXmlTagPtr &jimo_doc);

	bool registerMethods();
};
#endif
