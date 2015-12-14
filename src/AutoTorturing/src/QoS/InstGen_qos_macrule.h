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
//
// Modification History:
// 7/18/2007 : Created by TorturerGen Facility
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AutoTorturing_src_QoS_InstGen_qos_macrule_h_h
#define Aos_AutoTorturing_src_QoS_InstGen_qos_macrule_h_h

#include "CliTorturer/Ptrs.h"
#include "Torturer/ParmInstGen.h"
#include "Torturer/ReturnCode.h"
#include "Torturer/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "Util/Ptrs.h"
#include "XmlParser/Ptrs.h"
#include "Util/Ptrs.h"
#include "Ptrs.h"

#include "aosApi.h"

struct qos_macrule;


class qos_macrule_InstGen : public AosParmInstGen
{
	OmnDefineRCObject;

     OmnString mMember0;
     int mMember1;


	AosParmStrPtr	mMember1Gen;
	AosParmIntPtr	mMember2Gen;

public:
	qos_macrule_InstGen():AosParmInstGen("NoName") {}
	qos_macrule_InstGen(const OmnString &name);
	~qos_macrule_InstGen();

	virtual AosParmInstGenPtr clone() const;
	virtual bool nextStr(
				 	OmnString &value,
				 	const AosGenTablePtr &data,
				 	const AosGenRecordPtr &record,
				 	bool &isCorrect,
					const bool correctOnly,
				 	const bool selectFromRecord,
				 	AosParmReturnCode &rcode,
				 	OmnString &errmsg);

	bool		 nextPtr(
					struct qos_macrule* inst,
					const AosGenTablePtr &data,
				 	const AosGenRecordPtr &record,
				 	bool &isCorrect,
					const bool correctOnly,
				 	const bool selectFromRecord,
				 	AosParmReturnCode &rcode,
				 	OmnString &errmsg);
	bool	   nextInst(qos_macrule_InstGen &inst,
				  const AosGenTablePtr &data,
				 const AosGenRecordPtr &record,
				 bool &isCorrect,
				 const bool correctOnly,
				 const bool selectFromRecord,
				 AosParmReturnCode &rcode,
				 OmnString &errmsg);
	virtual AosParmType getParmType() const {return eAosParmType_InstGen;}

	virtual AosParmPtr createInstance(
							const OmnXmlItemPtr &def,
							OmnVList<AosGenTablePtr> &tables); 
	static bool 	registerInstanceGen(const OmnString &name);
	virtual bool 	getCrtValue(AosParmInstGenPtr &value) const;
	virtual bool 	getCrtValueAsArg(OmnString &value) const;
};

#endif

