////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This file is automatically generated by the TorturerGen facility.
//
// Modification History:
// 7/18/2007 : Created by TorturerGen Facility
//////////////////////////////////////////////////////////////////////////
#include "AutoTorturing/src/QoS/InstGen_qos_protorule.h"

#include "CliTorturer/CliTorturer.h"
#include "Tester/TestMgr.h"
#include "Torturer/Parm.h"
#include "Torturer/ParmIncls.h"
#include "Torturer/ParmInstGen.h"
#include "Torturer/ParmInt.h"
#include "Torturer/ParmStr.h"
#include "Util/DynArray.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlParser/XmlItem.h"



qos_protocolrule_InstGen::qos_protocolrule_InstGen(const OmnString &name)
:
AosParmInstGen(name)
{
}

qos_protocolrule_InstGen::~qos_protocolrule_InstGen()
{
}


bool
qos_protocolrule_InstGen::nextStr(OmnString &value,
                 		const AosGenTablePtr &data,
				        const AosGenRecordPtr &record,
						bool &isCorrect,
						const bool correctOnly,
						const bool selectFromRecord,
						AosParmReturnCode &rcode,
						OmnString &errmsg)
{
	if (!mInstGen)
	{
		OmnAlarm << "mInstGen is null" << enderr;
		return false;
	}

     qos_protocolrule* inst = OmnNew qos_protocolrule;
    if (mInstance) OmnDelete mInstance; 
    mInstance = inst;
    qos_protocolrule_InstGenPtr ptr = (qos_protocolrule_InstGen *) mInstGen.getPtr();

	return ptr->nextPtr(inst, data, record, isCorrect, correctOnly, 
			selectFromRecord, rcode, errmsg);
}


bool
qos_protocolrule_InstGen::nextPtr(
				  qos_protocolrule* inst,
				 const AosGenTablePtr &data,
				 const AosGenRecordPtr &record,
				 bool &isCorrect,
				 const bool correctOnly,
				 const bool selectFromRecord,
				 AosParmReturnCode &rcode,
				 OmnString &errmsg)
{
	    bool __correct;
	isCorrect = true;

	
    int mMember0;
    if (!mParmGens[0]->nextInt(mMember0, data, record, __correct, correctOnly, selectFromRecord, rcode, errmsg))
    {
        OmnAlarm << "Failed to generate member: "
            << mParmGens[0]->getName() << enderr;
        isCorrect = false;
        return 0;
    }
    isCorrect = isCorrect && __correct;

    int mMember1;
    if (!mParmGens[1]->nextInt(mMember1, data, record, __correct, correctOnly, selectFromRecord, rcode, errmsg))
    {
        OmnAlarm << "Failed to generate member: "
            << mParmGens[1]->getName() << enderr;
        isCorrect = false;
        return 0;
    }
    isCorrect = isCorrect && __correct;


	inst->protocol = mMember0;
		inst->priority = mMember1;
	return true;
}


bool
qos_protocolrule_InstGen::nextInst(qos_protocolrule_InstGen &inst,
				  const AosGenTablePtr &data,
				 const AosGenRecordPtr &record,
				 bool &isCorrect,
				 const bool correctOnly,
				 const bool selectFromRecord,
				 AosParmReturnCode &rcode,
				 OmnString &errmsg)
{
	    if (!mParmGens[0]->nextInt(mMember0, data, record, isCorrect, correctOnly, selectFromRecord, 
            rcode, errmsg)) 
    {
        OmnAlarm << "Failed to generate member: "
            << mParmGens[0]->getName() << enderr;
        return 0;
    }
    if (!mParmGens[1]->nextInt(mMember1, data, record, isCorrect, correctOnly, selectFromRecord, 
            rcode, errmsg)) 
    {
        OmnAlarm << "Failed to generate member: "
            << mParmGens[1]->getName() << enderr;
        return 0;
    }


	return true;
}


AosParmPtr
qos_protocolrule_InstGen::createInstance(const OmnXmlItemPtr &def, OmnVList<AosGenTablePtr> &tables)
{
    qos_protocolrule_InstGenPtr inst = OmnNew qos_protocolrule_InstGen(getName());
	if (!inst->config(def, "", tables, false))
	{
		OmnAlarm << "Failed to config the object" << enderr;
		return 0;
	}
	
	return inst;         
}


bool
qos_protocolrule_InstGen::registerInstanceGen(const OmnString &name)
{
	qos_protocolrule_InstGenPtr inst = OmnNew qos_protocolrule_InstGen(name);
	if (!AosParmInstGen::registerInstanceGen(inst))
	{
		OmnAlarm << "Failed to register Instance Generator: " 
			<< "qos_protocolrule_InstGen" << enderr;
		return false;
	}

	return true;
}


bool
qos_protocolrule_InstGen::getCrtValue(AosParmInstGenPtr &value) const
{
	value = mLastInstGen;
	return true;
}


bool 	
qos_protocolrule_InstGen::getCrtValueAsArg(OmnString &value) const
{
	value = "inst";
	return true;
}


AosParmInstGenPtr 
qos_protocolrule_InstGen::clone() const
{
	qos_protocolrule_InstGenPtr ptr = OmnNew qos_protocolrule_InstGen;	
	copyData(ptr.getPtr());
	return ptr.getPtr();
}

