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
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/TesterStatemachine.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SengTorturer/TesterLogin.h"
#include "SengTorturer/SengTesterXmlDoc.h"
#include "SengTorturer/SengTesterMgr.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorturer/SengTesterThrd.h"
#include "SengTorturer/SengTesterFileMgr.h"
#include "SengTorturer/StUtil.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"


AosStatemachineTester::AosStatemachineTester(const bool regflag)
:
AosSengTester(AosSengTester_Statemachine, AosTesterId::eStatemachine, regflag)
{
	// setDefaultWeights();
}


AosStatemachineTester::~AosStatemachineTester()
{
}


bool 
AosStatemachineTester::test()
{
	// This is the virtual function called from AosSengTester. It does just one
	// test. The caller will loop over the tries.
	AosRundataPtr rdata = mThread->getRundata(); 

	switch (OmnRandom::percent(
				mWtCreateStatemachine, 
				mWtRunStatemachine,
				mWtModifyStatemachine,
				mWtAccessCheck,
				mWtStopStatemachine))
	{
	case 1:
		 return createStatemachine();	 

	case 2:
		 return runStatemachine();

	case 3: 
		 return modifyStatemachine();

	case 4: 
		 return statemachineAccessCheck();

	case 5: 
		 return stopStatemachine();

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosStatemachineTester::createStatemachine()
{
	// A statemachine consists of a number of nodes. Each node
	// has enter actions, exit actions, and transition actions.
	// 	<statemachine 
	// 		AOSTAG_LOG_CTNR_OBJID="xxx"
	// 		AOSTAG_INST_CTNR_OBJID="xxx"
	// 		AOSTAG_STMCTYPE="xxx"
	// 		AOSTAG_STATE_DFT_STATUS="xxx"
	// 		AOSTAG_CREATE_CONTAINER="true|false"
	// 		AOSTAG_STMC_FINISHED_FLAG="true|false"
	// 		AOSTAG_STMC_STOPPED_FLAG="true|false">
	// 		<AOSTAG_INSTDOC_SELECTOR ...>
	// 			// A value selector that determines the instance doc objid
	// 		</AOSTAG_INSTDOC_SELECTOR>
	// 		<AOSTAG_INSTANCEDOC_CREATOR>
	// 			// A smart doc to create instance doc
	// 		</AOSTAG_INSTANCEDOC_CREATOR>
	// 		<AOSTAG_INIT_STATE_SEL>
	// 			<cond ...>
	// 				<action .../>
	// 				<action .../>
	// 				...
	// 			</cond>
	// 			...
	// 		</AOSTAG_INIT_STATE_SEL>
	// 		<AOSTAG_STMC_STOP_ACTIONS>
	// 			<action .../>
	// 			<action .../>
	// 			...
	// 		</AOSTAG_STMC_STOP_ACTIONS>
	// 		<AOSTAG_STATES>
	// 			<state_id 
	// 				AOSTAG_STATE_ID="xxx"
	// 				AOSTAG_STATE_NAME="xxx"
	// 				AOSTAG_BLOCK_NORMAL_EVENT_PROC="true|false"
	// 				AOSTAG_TRANSIT_ON_ENTER="state_id">
	// 				<AOSTAG_OPERATIONS>	// Define operation processing
	// 					<opr_id ...>
	// 						<cond ...>
	// 							<action .../>
	// 							<action .../>
	// 							...
	// 						</cond>
	// 					</opr_id ...>
	// 					...	 // Other operations
	// 				</AOSTAG_OPERATIONS>
	// 				<AOSTAG_CREATION_ACTIONS>
	// 					<action .../>
	// 					<action .../>
	// 					...
	// 				</AOSTAG_CREATION_ACTIONS>
	// 				<AOSTAG_ENTER_ACTIONS>
	// 					<action .../>
	// 					<action .../>
	// 					...
	// 				</AOSTAG_ENTER_ACTIONS>
	// 				<AOSTAG_EDGE_ACTIONS>
	// 					<to_state_id ...>
	// 						<action .../>
	// 						<action .../>
	// 						...
	// 					</to_state_id>
	//					...
	//				</AOSTAG_EDGE_ACTIONS>
	// 			</state_id>
	// 			...
	// 		</AOSTAG_STATES>
	// 	</statemachine>
	//
	return false;
}


bool
AosStatemachineTester::runStatemachine()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosStatemachineTester::modifyStatemachine()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosStatemachineTester::statemachineAccessCheck()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosStatemachineTester::stopStatemachine()
{
	OmnNotImplementedYet;
	return false;
}

