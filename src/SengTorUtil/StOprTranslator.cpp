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
// This class simulates Access Operations. In the security model, accesses
// to docs may be translated into User Defined Operations, such as:
// 		Creating Invoice
// 		Approve Request
// This class is used to convert doc accesses into User Defined Operations.
//
// Modification History:
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/StOprTranslator.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Conds/Condition.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTransDirectMap.h"
#include "SengTorUtil/StOprTransDocAttr.h"
#include "XmlUtil/SeXmlParser.h"
#include "ValueSel/ValueRslt.h"
#include "ValueSel/ValueSel.h"

static AosStOprTranslatorPtr  	sgTranslators[AosStOprTranslator::eMax];
static OmnMutex             	sgLock;
static int sgOprSeqno = 100;
static OmnString sgEntryTagId = "torturer_spec_tag";

static AosStOprTransDirectMap	sgOprTransDirectMap(true);
static AosStOprTransDocAttr		sgOprTransDocAttr(true);

AosStOprTranslator::AosStOprTranslator(const E type, const bool regflag)
:
mType(type)
{
	aos_assert(isValid(type));
	if (regflag)
	{
		AosStOprTranslatorPtr translator(this, false);
		bool rslt = registerTranslator(translator);
		aos_assert(rslt);
	}
}


AosStOprTranslator::~AosStOprTranslator()
{
}


bool
AosStOprTranslator::registerTranslator(const AosStOprTranslatorPtr &translator)
{
	sgLock.lock();
	if (sgTranslators[translator->mType])
	{
		sgLock.unlock();
		OmnAlarm << "Translator already registered: " << translator->mType << enderr;
		return false;
	}

	sgTranslators[translator->mType] = translator;
	sgLock.unlock();
	return true;
}


AosStOprTranslatorPtr
AosStOprTranslator::getTranslator(const AosStOprTranslator::E type)
{
	aos_assert_r(isValid(type), 0);
	sgLock.lock();
	AosStOprTranslatorPtr translator = sgTranslators[type];
	sgLock.unlock();
	aos_assert_r(translator, 0);
	return translator->clone();
}


bool
AosStOprTranslator::addOperation(
		const AosXmlTagPtr &xml_ctnr, 
		const AosStContainerPtr &st_ctnr, 
		const AosSengTestThrdPtr &thread)
{
	// This function randomly adds a new operation translator to both
	// 'xml_ctnr' and 'st_ctnr'. Note that the caller should save 
	// 'xml_ctnr' on the server.
	aos_assert_r(st_ctnr, false);
	aos_assert_r(xml_ctnr, false);
	AosXmlTagPtr oprtag = xml_ctnr->getFirstChild(AOSTAG_OPR_DEFS);
	if (!oprtag)
	{
		// No AOSTAG_OPR_DEFS yet, add it
		oprtag = xml_ctnr->addNode1(AOSTAG_OPR_DEFS);
		aos_assert_r(oprtag, false);
	}

	// Get a random SecOpr
	AosSecOpr::E opr_id= AosStUtil::getSecOpr();
	OmnString opr_str = AosSecOpr::toString(opr_id);
	AosXmlTagPtr secoprtag = oprtag->getFirstChild(opr_str);
	if (!secoprtag)
	{
		// The subtag for 'opr_id' is not there yet. Create it.
		secoprtag = oprtag->addNode1(opr_str);
	}

	OmnString id;
	sgLock.lock();
	int oprSeqno = sgOprSeqno++;
	id << oprSeqno;
	sgLock.unlock();

	// Randomly determine the type
	E type = getRandType();
	aos_assert_r(isValid(type), false);
	AosStOprTranslatorPtr pp = getTranslator(type);
	aos_assert_r(pp, false);
	pp->setOprSeqno(oprSeqno);

	// Randomly determine the contents.
	OmnString cond_str = pp->setRandContents(thread->getRundata());
	aos_assert_r(cond_str != "", false);

	// Add the newly created operation to the XML doc
	AosXmlTagPtr newopr_doc = AosXmlParser::parse(cond_str AosMemoryCheckerArgs);
	aos_assert_r(newopr_doc, false);
	newopr_doc->setAttr(sgEntryTagId, id);

	// Add a special tag for testing purpose only
	bool rslt = secoprtag->addNode(newopr_doc);
	aos_assert_r(rslt, false);

	// Add the newly created operation to the StContainer 
	rslt = st_ctnr->addTranslator(opr_str, pp);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosStOprTranslator::generateRandCond(const AosRundataPtr &rdata)
{
	// Each operation translator is associated with a condition.
	// This function randomly generates a condition.
	AosConditionObjPtr cond ;//= AosCondRandUtil::pickCond(rdata);
	aos_assert_r(cond, false);
	AosRandomRulePtr rule = OmnNew AosRandomRule();
	OmnString docstr = "";//cond->getXmlStr("cond", AosRandomRule::eDftLevel, 
			//"", "", rule, rdata);
	mCondSdoc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	return true;
}


OmnString
AosStOprTranslator::determineOperation(
		const AosStDocPtr &local_doc,
		const AosSecOpr::E opr_id, 
		const AosSengTestThrdPtr &thread)
{
	/*
	 * Chen Ding, 2014/10/19
	 * Cannot use Conditions here. Need to think about it.
	 *
	// This function converts a Doc Access into a User Defined
	// Operation. 
	// It does not care which doc being accessed. This is
	// normally used to convert the accesses to a container into
	// user defined operations. Example:
	// 		Container: "Customers"
	// 		Add Member: Operation is "Creating Customer"
	//
	// This means that eDirectMap directly maps an SecOpr to a user
	// defined operation:
	// 		sec_opr1	user_operation1
	// 		sec_opr2	user_operation2
	// 		...
	// The map is defined by 'mUserOperation'.
	//
	// Note that 'sec_opr' serves as the key, which means that the same
	// 'sec_opr' should map to no more than one user operation.
	AosRundataPtr rdata = thread->getRundata();
	aos_assert_r(mCondSdoc, "");
	if (!AosCondition::evalCondStatic(mCondSdoc, thread->getRundata())) return "";
	AosXmlTagPtr tag = mCondSdoc->getFirstChild(AOSTAG_CONDITION_THEN);
	aos_assert_r(tag, "");
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, tag, rdata);
	aos_assert_r(rslt, "");
	OmnString vv;
	vv = value.getValueStr1();
	return vv;
	*/
	OmnNotImplementedYet;
	return "";
}


bool 
AosStOprTranslator::removeOperation(
		const OmnString &opr_id,
		const AosStContainerPtr &st_ctnr,
		const AosXmlTagPtr &xml_ctnr,
		const AosSengTestThrdPtr &thread)
{
	// There should be an entry in 'xml_ctnr'. It removes that entry from
	// 'xml_ctnr', and then from 'st_ctnr'. 
	//
	// In order to make it easier to locate the entry, the torturer 
	// adds a unique ID. 
	aos_assert_r(st_ctnr, false);
	aos_assert_r(xml_ctnr, false);
	AosXmlTagPtr oprtag = xml_ctnr->getFirstChild(AOSTAG_OPR_DEFS);
	aos_assert_r(oprtag, false);
	AosXmlTagPtr secoprtag = oprtag->getFirstChild(opr_id);
	aos_assert_r(secoprtag, false);

	AosXmlTagPtr record = secoprtag->getFirstChild();
	while (record)
	{
		int id = record->getAttrInt(sgEntryTagId, -1);
		if (id == mOprSeqno)
		{
			secoprtag->removeNode(record);
			if (secoprtag->getNumSubtags() == 0)
			{
				oprtag->removeNode(secoprtag->getTagname(), false, false);
				if (oprtag->getNumSubtags() == 0)
				{
					xml_ctnr->removeNode(oprtag->getTagname(), false, false);
				}
			}

			AosStOprTranslatorPtr thisptr(this, false);
			st_ctnr->removeTranslator(opr_id, thisptr);
			return true;
		}
		record = secoprtag->getNextChild();
	}
	OmnAlarm << "Translator not found!" << enderr;
	return false;
}


/*
bool 
AosStOprTranslator::checkOperation(
		const AosStContainerPtr &st_ctnr,
		const AosXmlTagPtr &xml_ctnr,
		const AosSengTestThrdPtr &thread)
{
	// This function checks whether the operation can be recognized.
	// Since it is a direct map, we will randomly pick an access
	// operation, and do the operation. 
	bool allowed;
	bool rslt = thread->doOperation(allowed, xml_ctnr);
	aos_assert_r(rslt, false);
	return true;
}
*/


bool
AosStOprTranslator::pickCommand(
		OmnString &cmd, 
		OmnString &value_sel, 
		const AosSengTestThrdPtr &thread)
{
	// This function picks the command and value_sel based on 
	// this class's current contents. An operation translator
	// has a condition. The condition is stored in 'mCond'
	// and its XML is 'mCondSdoc'. It asks the condition to
	// generate the command that can make the condition
	// to be true.
	cmd = "";
	value_sel = "";
	aos_assert_r(mCond, false);
	aos_assert_r(mCondSdoc, false);
	cmd = "";//mCond->generateCommand(mCondSdoc, thread->getRundata());
	if (cmd != "")
	{
		AosXmlTagPtr then_tag = mCondSdoc->getFirstChild(AOSTAG_CONDITION_THEN);
		aos_assert_r(then_tag, false);
		AosXmlTagPtr valuesel = then_tag->getFirstChild();
		value_sel = valuesel->toString();
		return true;
	}

	// This means the condition is not able to generate a command
	// to make the condition to be true.
	cmd = "";
	value_sel = "";
	return true;
}

