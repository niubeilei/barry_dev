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
// 11/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/AllActions.h"

#include "Actions/ActAddAttr.h"
#include "Actions/ActCreateDoc.h"
#include "Actions/ActCreateLog.h" 
#include "Actions/ActComposeStr.h"
#include "Actions/ActSeqno.h"
#include "Actions/ActRemoveAttr.h"
#include "Actions/ActModifyAttr.h"
#include "Actions/ActLimited.h"
#include "Actions/ActRemoveDoc.h"
#include "Actions/ActVerCode.h"
#include "Actions/ActTrashDoc.h"
#include "Actions/ActSetAttrFromDb.h"
#include "Actions/ActUpdateCounter.h"  
#include "Actions/ActModifyCounter.h" 
#include "Actions/ActModifyNode.h"
#include "Actions/ActModifyRundataAttr.h" 
#include "Actions/ActDeleteCounter.h"  
#include "Actions/ActDataGenerate.h"
#include "Actions/ActCalcSum.h"			
#include "Actions/ActCountOccurences.h"			
#include "Actions/ActSetRdataWorkStr.h"			
#include "Actions/ActRunDLL.h"
#include "Actions/ActRunSdoc.h"
#include "Actions/ActAddTimer.h"
#include "Actions/ActCancelTimer.h"
#include "Actions/ActSendMail.h"
#include "Actions/ActAddText.h"
#include "Actions/ActSendData.h"
#include "Actions/ActFilterData.h"
#include "Actions/ActCreateImportData.h"
#include "Actions/ActDeleteIIL.h"
#include "Actions/ActBatchDelDocs.h"
#include "Actions/ActModifyStr.h"				// Chen Ding, 06/01/2012
#include "Actions/ActSleep.h"
#include "Actions/ActSplitFile.h"				// Chen Ding, 06/16/2012
#include "Actions/ActRemoveWS.h"				// Chen Ding, 06/16/2012
#include "Actions/ActDeleteIILEntry.h"			// Chen Ding, 06/16/2012
#include "Actions/ActStrIILBatchAdd.h"
#include "Actions/ActRebuildBitmapOpr.h"
#include "Actions/ActStrIILBatchInc.h"
#include "Actions/ActSetRundataValue.h"
#include "Actions/ActDeleteDoc.h"			    // Jackie zhao, 08/09/2012
#include "Actions/ActScannerData.h"
#include "Actions/ActRunQuery.h"
#include "Actions/ActSqlServerToDb.h"
#include "Actions/ActModifyQuery.h"
#include "Actions/ActRunJob.h"
#include "Actions/ActAddCounter2IILEntry.h"
#include "Actions/ActSetAttr.h"
#include "Actions/ActMergeFile.h"
#include "Actions/ActImportDocFixed.h"
//#include "Actions/ActImportDocCSV.h"
#include "Actions/ActDeleteGroupDoc.h"
#include "Actions/ActImportDocNormal.h"
#include "Actions/ActIILBatchOpr.h"
//#include "Actions/ActIILPatternOpr.h"
//#include "Actions/ActIILPattern2Opr.h"
#include "Actions/ActSnapShot.h"
#include "Actions/ActIILBatchOprMerge.h"
//#include "Actions/ActCreateHitIIL.h"
#include "Actions/ActUnCreatFile.h"
#include "Actions/ActImportDoc2.h"
#include "Actions/ActCreateFile.h"
#include "Actions/ActImportBinaryDoc.h"
#include "Actions/ActImportUnicomFile.h"
#include "Actions/ActImportDataFileFormat.h"
#include "Actions/ActStartServer.h"
#include "Actions/ActCreateCluster.h"
#include "Actions/ActStopServer.h"
#include "Actions/ActAddServer.h"
#include "Actions/ActSetAttrs.h"		// Ken Lee, 2013/08/05
#include "Actions/ActAddIILEntry.h"		// Ken Lee, 2013/08/22
#include "Actions/ActSendUrl.h"		// Jackie, 2013/09/04

#include "DataEngine/JoinDataEngine2.h"		

#include "DataEngine/ScanDataEngine2.h"		

#include "Actions/ActCreateDatasetDoc.h"		
#include "Actions/ActCreateTaskFile.h"		

//#include "Actions/ActJimoTableBatchOpr.h"		// Ken Lee, 2014/09/25
#include "Actions/ActInstall.h"					// Jozhi, 2014/11/17
#include "Actions/ActQueryExportFile.h"					// Jozhi, 2014/11/17


static bool 				sgInited = false;
static OmnMutex				sgLock;

bool
AosAllActions::init()
{
	static AosActAddAttr			sgActAddAttr(true);
	static AosActVerCode			sgActVerCode(true);
	static AosActModifyAttr			sgActModifyAttr(true);
	static AosActLimited			sgActLimited(true);
	static AosActRemoveAttr			sgActRemoveAttr(true);
	static AosActRemoveDoc			sgActRemoveDoc(true);
	static AosActSeqno				sgActSeqno(true);
	//static AosActCreateDoc			sgCreateDoc(true);
	//static AosActCreateLog			sgCreateLog(true); 			//tom 2011-08-08
	static AosActComposeStr			sgComposeStr(true);
	//static AosActTrashDoc			sgTrashDoc(true);
	static AosActSetAttrFromDb		sgSetAttrFromDb(true);
	static AosActUpdateCounter		sgActUpdateCounter(true); 	// Lynch 2011-06-13
	static AosActModifyCounter		sgActModifyCounter(true); 	// Lynch 2011-06-23
	static AosActModifyRundataAttr	sgActModifyRundataAttr(true); // Brian Zhang  2011-06-27
	//static AosActModifyNode         sgActModifyNode(true);
	static AosActDeleteCounter		sgActDeleteCounter(true); 	// Lynch 2011-06-23
	static AosActDataGenerate		sgActDataGenerate(true);
	static AosActCalcSum			sgActCalcSum(true);			// Chen Ding, 12/30/2011
	static AosActCountOccurences	sgCountOccur(true);			// Chen Ding, 12/31/2011
	static AosActRunDLL				sgRunDLL(true);				// Ken Lee, 2012/02/23
	static AosActRunSdoc			sgRunSdoc(true);			// Ken Lee, 2012/03/29
	static AosActAddTimer			sgAddTimer(true);			// Ken Lee, 2012/02/29
	static AosActCancelTimer		sgCancelTimer(true);		// Ken Lee, 2012/02/29
	//static AosActSendMail			sgSendMail(true);
	static AosActAddText			sgAddText(true);
	static AosActSendData           sgSendData(true);	
	static AosActFilterData         sgFilterData(true);			// Chen Ding, 05/06/2012	
	static AosActCreateImportData	sgCreateImportData(true);	// Ken Lee, 2012/05/12	
	static AosActDeleteIIL			sgDeleteIIL(true);			// Chen Ding, 05/26/2012	
	static AosActBatchDelDocs		sgBatchDelDocs(true);		// Chen Ding, 05/26/2012	
	//static AosActModifyStr			sgModifyStr(true);			// Chen Ding, 06/01/2012	
	static AosActSleep				sgActSleep(true);
	static AosActSplitFile			sgSplitFile(true);			// Chen Ding, 06/16/2012
	static AosActRemoveWS			sgActRemoveWS(true);		// Brian Zhang, 06/21/2012
	static AosActDeleteIILEntry		sgActDeleteIILEntry(true);	// Brian Zhang, 06/21/2012
	static AosActStrIILBatchAdd     sgActStrIILBatchAdd(true);
	static AosActStrIILBatchInc     sgActStrIILBatchInc(true);
	static AosActSetRundataValue    sgAosActSetRundataValue(true);
	static AosActDeleteDoc			sgActDeleteDoc(true);		// Jackie zhao, 08/09/2012
	//static AosActRunQuery			sgActRunQuery(true);
	static AosActSqlServerToDb		sgActSqlServerToDb(true);
	static AosActModifyQuery		sgActModifyQuery(true);
	static AosActRunJob				sgActRunJob(true);
	static AosActAddCounter2IILEntry sgActAddCounter2IILEntry(true);
	//static AosActSetAttr			sgActSetAttr(true);
	static AosActMergeFile			sgActMergeFile(true);
	static AosActImportDocFixed		sgActImportDocFixed(true);
	//static AosActImportDocCSV		sgActImportDocCSV(true);
	static AosActDeleteGroupDoc		sgActDeleteGroupDoc(true);
	static AosActImportDocNormal	sgActImportDocNormal(true);
	//static AosActIILBatchOpr		sgActIILBatchOpr(true);
	//static AosActIILPatternOpr		sgActIILPatternOpr(true);
	//static AosActIILPattern2Opr		sgActIILPattern2Opr(true);
	static AosActRebuildBitmapOpr	sgActRebuildBitmapOpr(true);
	static AosActSnapShot			sgActSnapShot(true);
	static AosActIILBatchOprMerge	sgActIILBatchOprMerge(true);
	//static AosActCreateHitIIL		sgActCreateHitIIL(true);
	static AosActUnCreatFile		sgActUnCreatFile(true);
	static AosActCreateFiles		sgAosActCreateFiles(true);
	static AosActImportBinaryDoc	sgAosActImportBinaryDoc(true);
	static AosActImportDataFileFormat sgAosActImportDataFileFormat(true);
	static AosActStartServer		sgAosActStartServer(true);
	static AosActCreateCluster 		sgAosActCreateCluster(true);
	static AosActStopServer			sgAosActStopServer(true);
	static AosActAddServer			sgAosActAddServer(true);
	//static AosActSetAttrs			sgAosActSetAttrs(true);			// Ken Lee, 2013/08/05
	static AosActAddIILEntry		sgAosActAddIILEntry(true);		// Ken Lee, 2013/08/22
	static AosActSendUrl			sgAosActSendUrl(true);		// Jackie, 2013/09/04
	
	static AosJoinDataEngine2 			sgAosJoinDataEngine2(true);	
	static AosScanDataEngine2 			sgActScanDataEngine2(true);		// barry, 2015/02/09/
	static AosActCreateDatasetDoc		sgAosActCreateDatasetDoc(true);	
	
	static AosActCreateTaskFile		sgAosActCreateTaskFile(true);	// Ketty 2014/08/22	


	//static AosActJimoTableBatchOpr	sgActJimoTableBatchOpr(true);	// Ken Lee, 2014/09/25
	static AosActInstall			sgActInstall(true);				// Jozhi, 2014/11/17/
	static AosActQueryExportFile	sgActQueryExportFile(true);		// Jozhi, 2014/11/19/


	if (!sgInited)
	{
		sgLock.lock();
		AosActionType::check();
		sgInited = true;
		sgLock.unlock();
	}
	return true;
}

