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
// 02/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GicCodeGen/GicCodeGen.h"

#include "alarm_c/alarm.h"
#include "GicCodeGen/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


static AosGicCodeGenPtr		sgGicGens[AosGicGenType::eMax];
static OmnString			sgNames[AosGicGenType::eMax];
static OmnMutex				sgLock;

AosGicCodeGen::AosGicCodeGen(
		const OmnString &name,
		const AosGicGenType::E type, 
		const bool regflag)
:
mType(type)
{
	if (regflag)
	{
		AosGicCodeGenPtr thisptr(this, false);
		registerGicCodeGen(thisptr, name);
	}
}


bool
AosGicCodeGen::registerGicCodeGen(const AosGicCodeGenPtr &gic_gen, const OmnString &name)
{
	OmnScreen << "Register GicCodeGen: " << name << ":" << gic_gen->mType << endl;
    aos_assert_r(AosGicGenType::isValid(gic_gen->mType), false);

	sgLock.lock();
    if (sgGicGens[gic_gen->mType])
	{
		OmnAlarm << "GicCodeGen already registered: " << gic_gen->mType << enderr;
		sgLock.unlock();
		return false;
	}
    sgGicGens[gic_gen->mType] = gic_gen;
	sgLock.unlock();
	
	bool rslt = AosGicGenType::addName(name, gic_gen->mType);
	if (!rslt)
	{
		OmnAlarm << "Failed adding gic_gen name: " << name << enderr;
		return false;
	}
	return true;
}


bool
AosGicCodeGen::checkRegistrations()
{
	sgLock.lock();
	for (int i=AosGicGenType::eInvalid+1; i<AosGicGenType::eMax; i++)
	{
		if (!sgGicGens[i])
		{
			OmnAlarm << "Missing Gic Gen Registration: " << i << enderr;
		}
	}
	sgLock.unlock();
	return true;
}


bool 
AosGicCodeGen::generateCodeStatic(
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const AosXmlTagPtr &gic_def,
		const AosXmlTagPtr &gic,
		AosHtmlCode &code, 
		const AosRundataPtr &rdata)
{
	// This function generates the code for the gic 'gic'. 'gic_def' is in the 
	// form:
	// 		<gic_codegen>
	// 			<codegen type="xxx" .../>
	// 			<codegen type="xxx" .../>
	// 			...
	// 		</gic_codegen>
	//
	aos_assert_rr(vpd, rdata, false);
	aos_assert_rr(obj, rdata, false);
	aos_assert_rr(gic_def, rdata, false);

	OmnString code_str;
	AosXmlTagPtr gen = gic_def->getFirstChild();
	int guard = eMaxCodegens;
	while (guard-- && gen)
	{
		OmnString type = gen->getAttrStr(AOSTAG_TYPE);
		AosGicGenType::E id = AosGicGenType::toEnum(type);
		if (!AosGicGenType::isValid(code))
		{
			AosSetError(rdata, "invalid_gicgen_type") << ": " << gic_def;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		iAosGicCodeGenPtr instance = sgGicGens[code];
		if (!instance)
		{
			OmnAlarm << "GIC Code gen not registered: " << type << enderr;
		}
		else
		{
			instance->generateCode(vpd, obj, gen, gic, code_str, rdata);
		}

		gen = gic_def->getNextChild();
	}
	
	code->appendHtml(code_str);
	return true;
}


AosGicCodeGenPtr
AosGicCodeGen::getGicGen(const OmnString &type, cosnt AosRundataPtr &rdata)
{
	AosGicGenType::E code = AosGicGenType::toEnum(type);
	if (!AosGicGenType::isValid(code))
	{
		AosSetError(rdata, "invalid_gicgen_type") << ": " << gic_def;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return sgGicGens[code];
}

