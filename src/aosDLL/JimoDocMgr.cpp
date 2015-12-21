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
// 2014/01/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "aosDLL/JimoDocMgr.h"

#include "API/AosApi.h"
#include "aosDLL/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"

static AosJimoDocMgrPtr sgJimoDocMgr;
static OmnMutex			sgLock;

AosJimoDocMgr::AosJimoDocMgr()
{
	mLock = OmnNew OmnMutex();
}


bool
AosJimoDocMgr::init(AosRundata *rdata)
{
	sgLock.lock();
	if (sgJimoDocMgr)
	{
		sgLock.unlock();
		return true;
	}

	AosJimoDocMgrPtr mgr = OmnNew AosJimoDocMgr();
	mgr->createJimoDocs(rdata);
	sgJimoDocMgr = mgr;
	sgLock.unlock();
	return true;
}


bool
AosJimoDocMgr::createJimoDocs(AosRundata *rdata)
{
	createJimoDoc(rdata, "AosTermArithNew", "libQueryTermJimos.so", 1);
	createJimoDoc(rdata, "AosDataFieldU64New", "libDataFieldJimos.so", 1);	// Chen Ding, 2014/08/16
	createJimoDoc(rdata, "AosBFString", "libBSON.so", 1);					// Chen Ding, 2014/08/17
	createJimoDoc(rdata, "AosGenericVerbMgr", "libGenericVerbsJimos.so", 1);
	createJimoDoc(rdata, "AosGenericVerb", "libGenericVerbsJimos.so", 1);
	createJimoDoc(rdata, "AosGenericObjMgr", "libGenericObjsJimos.so", 1);
	createJimoDoc(rdata, "AosSystemGenObj", "libGenericObjsJimos.so", 1);
	createJimoDoc(rdata, "AosGenericMethodObj", "libGenericMethodJimos.so", 1);
	createJimoDoc(rdata, "AosVirtualFieldCompose", "libDataFieldJimos.so", 1);	// Chen Dign, 	2014/11/17
	createJimoDoc(rdata, "AosReplicatorMgr", "libReplicatorMgr.so", 1);			// White Wu, 	2014/11/19
	createJimoDoc(rdata, "AosHelloWorldJimoCalls", "libJimoCallHelloWorld.so", 1);	// Chen Ding, 	2014/12/18
	createJimoDoc(rdata, "AosRaftJimoCalls", "libJimoCallRaft.so", 1);	// Phil, 	2015/05/14
	createJimoDoc(rdata, "AosClusterMgr", "libClusterMgr.so", 1);                       // Yazong Ma,   2015/03/19
	createJimoDoc(rdata, "AosHelloWorldJimoCalls", "libJimoCallHelloWorld.so", 1);	// Chen Ding, 	2014/12/18
	createJimoDoc(rdata, "AosIDOServer", "libIDOServer.so", 1);                         // Chen Ding,   2015/03/28
	createJimoDoc(rdata, "AosIDOExample", "libIDOExamples.so", 1);                      // Chen Ding,   2015/03/28

	createJimoDoc(rdata, "AosAggrFuncMgr", "libAggrFuncJimos.so", 1);   // Levi, 2015/02/02
	createJimoDoc(rdata, "AosJimoLogicLoaddata", "libJimoLogicNew.so", 1);   // Levi, 2015/07/23
	createJimoDoc(rdata, "AosDocPackageJimoCalls", "libJimoCallDocPackage.so", 1);
	createJimoDoc(rdata, "AosIILPackageJimoCalls", "libJimoCallIILPackage.so", 1);	// Yazong Ma, 	2014/12/24
	createJimoDoc(rdata, "AosAggrFuncMgr", "libAggrFuncJimos.so", 1);	// Phil  2015/02/08
	createJimoDoc(rdata, "AosStatistics", "libStatisticsJimos.so", 1);	// Phil  2015/02/08
	createJimoDoc(rdata, "AosStatModel", "libStatisticsJimos.so", 1);	// Phil  2015/02/08
	createJimoDoc(rdata, "AosStatMeasure", "libStatisticsJimos.so", 1);	// Phil  2015/02/08
	createJimoDoc(rdata, "AosJimoParser", "libJimoParser.so", 1);		// Arvin  2015/04/20
	createJimoDoc(rdata, "AosJimoLogicCreateJob", "libJimoLogicNew.so", 1);		// Arvin  2015/04/20
	createJimoDoc(rdata, "AosJimoLogicCreateService", "libJimoLogicNew.so", 1);		// Arvin  2015/04/20
	createJimoDoc(rdata, "AosJimoLogicRunJob", "libJimoLogicNew.so", 1);		// Gavin 2015/07/24
	createJimoDoc(rdata, "AosJimoLogicDataProcStat", "libJimoLogicNew.so", 1);		// Bryant  2015/06/16
	createJimoDoc(rdata, "AosJimoLogicDataProcDistinct", "libJimoLogicNew.so", 1);		// Bryant  2015/07/22
	createJimoDoc(rdata, "AosJimoLogicDataProcMatch", "libJimoLogicNew.so", 1);		// Bryant  2015/07/23
	createJimoDoc(rdata, "AosJimoLogicDataProcCheckPoint", "libJimoLogicNew.so", 1);		// Bryant  2015/07/23
	createJimoDoc(rdata, "AosJimoLogicCreateIndex", "libJimoLogicNew.so", 1);		// Arvin  2015/04/20
	createJimoDoc(rdata, "AosJQLParserWrapper", "libJQLParserJimos.so", 1);		// Arvin  2015/04/22
    createJimoDoc(rdata, "AosJimoLogicDataProcMap", "libJimoLogicNew.so", 1);     //2015/04/25
    createJimoDoc(rdata, "AosJimoLogicCreateDoc", "libJimoLogicNew.so", 1);     //2015/04/27
    createJimoDoc(rdata, "AosJimoProgJob",       "libJimoProg.so", 1);     //2015/04/27
    createJimoDoc(rdata, "AosJimoProgService",       "libJimoProg.so", 1);     //2015/04/27
    createJimoDoc(rdata, "AosJimoProgMgr",       "libJimoProg.so", 1);     //2015/04/27
	createJimoDoc(rdata, "AosJimoLogicCreateDistMap","libJimoLogicNew.so",1);  	// Arvin  2015/05/18
	createJimoDoc(rdata, "AosIdoCounter", "libIDOJimos.so",1);  				// Chen Ding, 2015/06/01
	createJimoDoc(rdata, "AosJimoLogicDataProcUnion","libJimoLogicNew.so",1);    // xiafan  2015/05/25
	createJimoDoc(rdata, "AosJimoLogicDataProcIf","libJimoLogicNew.so",1);  	// Arvin  2015/05/26
	createJimoDoc(rdata, "AosJimoLogicDataProcIndex","libJimoLogicNew.so",1);  	// Arvin  2015/05/26
	createJimoDoc(rdata, "AosJimoLogicDataProcDoc","libJimoLogicNew.so",1);  	// Barry 2015/07/06
	createJimoDoc(rdata, "AosJimoLogicDataProcStr2StrMap","libJimoLogicNew.so",1);  	// Arvin  2015/05/27
	createJimoDoc(rdata, "AosJimoLogicDataProcStr2NumMap","libJimoLogicNew.so",1);  	// Arvin  2015/05/27
	createJimoDoc(rdata, "AosJimoLogicDataProcMinus","libJimoLogicNew.so",1); 		 	// Arvin  2015/05/27
	createJimoDoc(rdata, "AosJimoLogicDataProcSwitchCase","libJimoLogicNew.so",1);  	// Arvin  2015/05/27
	createJimoDoc(rdata, "AosJimoLogicDatasetDir","libJimoLogicNew.so",1);  	// rain  2015/05/28
	createJimoDoc(rdata, "AosJimoLogicDatasetFile","libJimoLogicNew.so",1);  	// rain  2015/05/30
	createJimoDoc(rdata, "AosJimoLogicDatasetTable","libJimoLogicNew.so",1);  	// xiafan  2015/08/26
	createJimoDoc(rdata, "AosJimoLogicDataProcRankOver","libJimoLogicNew.so",1);  	// Arvin  2015/05/30
	createJimoDoc(rdata, "AosJimoLogicDataProcWordCount","libJimoLogicNew.so",1);  	// Arvin  2015/06/01
	createJimoDoc(rdata, "AosJimoLogicDataProcWordCount2","libJimoLogicNew.so",1);  	// Arvin  2015/06/01
	createJimoDoc(rdata, "AosJimoLogicDatasetIIL","libJimoLogicNew.so",1);  	// rain  2015/06/01
	createJimoDoc(rdata, "AosJimoLogicDataProcIndexU64","libJimoLogicNew.so",1);  	// Arvin  2015/06/01
	createJimoDoc(rdata, "AosJimoLogicDataProcJoin","libJimoLogicNew.so",1);  	// Arvin  2015/06/01
	createJimoDoc(rdata, "AosJimoLogicSchemaCSV","libJimoLogicNew.so",1);  	// rain 2015/06/06
	createJimoDoc(rdata, "AosJimoLogicSchemaFixlength","libJimoLogicNew.so",1);  	// rain 2015/06/06
	createJimoDoc(rdata, "AosJimoLogicDataProcSelect","libJimoLogicNew.so",1);  	// Arvin  2015/06/02
	createJimoDoc(rdata, "AosJimoLogicDataProcSelectR1R2","libJimoLogicNew.so",1);  	// Barry 2015/07/06
	createJimoDoc(rdata, "AosJimoLogicDataProcSelectMax","libJimoLogicNew.so",1);  	// Barry 2015/06/29
	createJimoDoc(rdata, "AosJimoLogicDataProcSelectMin","libJimoLogicNew.so",1);  	// Barry 2015/06/29
	createJimoDoc(rdata, "AosJimoLogicDataProcSelectAverage","libJimoLogicNew.so",1);  	// Barry 2015/06/29
	createJimoDoc(rdata, "AosJimoLogicDataProcMerge","libJimoLogicNew.so",1);  	// barry 2015/06/03
	createJimoDoc(rdata, "AosJimoLogicDataProcPipe","libJimoLogicNew.so",1);  	// barry 2015/06/03
	createJimoDoc(rdata, "AosJimoLogicUserVar","libJimoLogicNew.so",1);  		// Young 2015/07/07
	createJimoDoc(rdata, "AosJimoLogicSetUserVar","libJimoLogicNew.so",1);  		// Young 2015/07/07
	createJimoDoc(rdata, "AosJimoLogicDataProcUpdateIndex","libJimoLogicNew.so",1);  	// barry 2015/06/03
	createJimoDoc(rdata, "AosJimoLogicCreateStatStream","libJimoLogicNew.so",1);  	// levi 2015/06/19
	createJimoDoc(rdata, "AosJimoLogicDataProcTest","libJimoLogicNew.so",1);  	// levi 2015/06/19
	createJimoDoc(rdata, "AosJimoLogicStartTorturer","libJimoLogicNew.so",1); 		// liuwei 2015/11/27
	createJimoDoc(rdata, "AosJimoLogicShowTorturer","libJimoLogicNew.so",1); 		// liuwei 2015/12/11
	createJimoDoc(rdata, "AosJimoLogicDataProcTransTorturer","libJimoLogicNew.so",1);  	// andy 2015/12/07
	createJimoDoc(rdata, "AosDocEngineJimoCallHandler","libDocEngineProcs.so",1);  	// Gavin 2015/06/19
	createJimoDoc(rdata, "AosDataletParserXml","libDataletParser.so",1);  	// Gavin 2015/06/19
	createJimoDoc(rdata, "AosEngineProcCreateDoc","libDocEngineProcs.so",1);  	// Gavin 2015/06/19
	createJimoDoc(rdata, "AosIndexEngine","libIndexEngine.so",1);  	// Chen Ding, 2015/06/20
	createJimoDoc(rdata, "AosDocStoreJimoCallHandler","libDocStoreProcs.so",1);  	// Chen Ding, 2015/06/20
	createJimoDoc(rdata, "AosStoreProcCreateSE","libDocStoreProcs.so",1);  	// xuqi, 2015/10/19
	createJimoDoc(rdata, "AosBlobSEAPI","libBlobSE.so",1);  	// Phil Pei, 2015/06/23
	createJimoDoc(rdata, "AosEngineProcCreateDoc","libDocEngineProcs.so",1);  	// Gavin 2015/06/19
	createJimoDoc(rdata, "AosEngineProcCreateDoc1","libDocEngineProcs.so",1);  	// Xuqi 2015/11/3
	createJimoDoc(rdata, "AosEngineProcGetDoc","libDocEngineProcs.so",1);  	// Gavin, 2015/06/24
	createJimoDoc(rdata, "AosEngineProcDeleteDoc","libDocEngineProcs.so",1);  	// Gavin, 2015/06/24
	createJimoDoc(rdata, "AosEngineProcModifyDoc","libDocEngineProcs.so",1);  	// Gavin, 2015/06/24
	createJimoDoc(rdata, "AosEngineProcIsRepairing","libDocEngineProcs.so",1);  	// xuqi, 2015/10/14
	createJimoDoc(rdata, "AosEngineProcCreateRootCtnr","libDocEngineProcs.so",1);  	// xuqi, 2015/10/15
	createJimoDoc(rdata, "AosEngineProcBindObjid","libDocEngineProcs.so",1);  	// xuqi, 2015/10/15
	createJimoDoc(rdata, "AosEngineProcCreateDocSafe3","libDocEngineProcs.so",1);  	// xuqi, 2015/10/15
	createJimoDoc(rdata, "AosEngineProcCreateDocSafe1","libDocEngineProcs.so",1);  	// xuqi, 2015/10/22
	createJimoDoc(rdata, "AosEngineProcBindCloudid","libDocEngineProcs.so",1);  	// xuqi, 2015/10/15
	createJimoDoc(rdata, "AosEngineProcWriteAccessDoc","libDocEngineProcs.so",1);  	// xuqi, 2015/10/22
	createJimoDoc(rdata, "AosEngineProcGetAccessRcd","libDocEngineProcs.so",1);  	// xuqi, 2015/10/27
	createJimoDoc(rdata, "AosEngineProcGetJQLDocs","libDocEngineProcs.so",1);  	// xuqi, 2015/11/6
	createJimoDoc(rdata, "AosEngineProcGetDocByCloudid","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/10
	createJimoDoc(rdata, "AosEngineProcGetDocidByCloudid","libDocEngineProcs.so",1);  	// Gavin, 2015/06/24
	createJimoDoc(rdata, "AosEngineProcGetDocidByObjid","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/10
	createJimoDoc(rdata, "AosEngineProcIncrementValue","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/10
	createJimoDoc(rdata, "AosEngineProcModifyAttrStr1","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/10
	createJimoDoc(rdata, "AosEngineProcModifyAttrStr","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/10
	createJimoDoc(rdata, "AosEngineProcIsCloudidBound","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcCreateBinaryDoc","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcDeleteObj","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcRetrieveBinaryDoc","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcProcDocLock","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcProcNotifyMsg","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcGetParentContainer","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcCreateJQLTable","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcGetDocByJQL","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcGetDocByKey1","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcGetDocByKey2","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcGetDocByKeys","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcMergeSnapshot","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcCreateSnapshot","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcCommitSnapshot","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcRollbackSnapshot","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcDeleteDocByDocid","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcCreateDocByTemplate1","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcDoesObjidExist","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcCloneDoc","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcGetDeletedDoc","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosEngineProcDeleteDocByObjid","libDocEngineProcs.so",1);  	// Xuqi, 2015/11/11
	createJimoDoc(rdata, "AosStoreProcGetDoc","libDocStoreProcs.so",1);  	// Gavin, 2015/06/24
	createJimoDoc(rdata, "AosStoreProcCreateDoc","libDocStoreProcs.so",1);  	// Gavin, 2015/06/24
	createJimoDoc(rdata, "AosStoreProcDeleteDoc","libDocStoreProcs.so",1);  	// Gavin, 2015/06/24
	createJimoDoc(rdata, "AosStoreProcModifyDoc","libDocStoreProcs.so",1);  	// Gavin, 2015/06/24
	createJimoDoc(rdata, "AosStoreProcWriteAccessDatalet","libDocStoreProcs.so",1);  	// Gavin, 2015/06/24
	createJimoDoc(rdata, "AosJimoLogicIf","libJimoLogicNew.so",1);  			// arvin, 2015/07/07
	createJimoDoc(rdata, "AosJimoLogicDataProcDoc","libJimoLogicNew.so",1);  	// Arvin  2015/06/02
	createJimoDoc(rdata, "AosJimoLogicDataProcSyncher","libJimoLogicNew.so",1);   // Andy, 2015/11/06
	createJimoDoc(rdata, "AosJimoLogicDataProcOnNetOffNet", "libJimoLogicNew.so", 1);   // Barry, 2015/12/13
	createJimoDoc(rdata, "AosDatasetConverterMatrix","libDatasetConverters.so",
			                1, "dataset_converter", "matrix");                      // Chen Ding, 2015/09/11
	createJimoDoc(rdata, "AosJPSCCheckIndexesAndDocs","libJimoProgCheckers.so",1);  	// Arvin  2015/08/21
	createJimoDoc(rdata, "AosDatasetConvertMatrix","libDatasetConverters.so",1,   //xuqi 2015/0912
						 "dataset_converter", "matrix");
	createJimoDoc(rdata, "AosDatasetSyncher","libDatasetJimos.so",1,   			// Young, 2015/10/12
						 "dataset_syncher", "syncher");
	createJimoDoc(rdata, "AosQueryFormatCSV","libQueryFormat.so",
							1, "query_format", "csv");                            //Barry 2015/09/12
	createJimoDoc(rdata, "AosQueryIntoLocalFile","libQueryInto.so",
							1, "query_into", "local_file");                       //Barry 2015/09/12
							
	createJimoDoc(rdata, "AosSyncherExample","libSynchers.so",
							1, "synchers", "syncher_example");                    //Chen Ding, 2015/09/16

	createJimoDoc(rdata, "AosSyncEngineJimoCallHandler","libSyncEngine.so",
							1, "SyncEngine", "sync_engine_jimocall_handler");                    //Chen Ding, 2015/09/16

	return true;
}


AosXmlTagPtr
AosJimoDocMgr::getJimoDocStatic(
		AosRundata *rdata,
		const OmnString &classname,
		const int version)
{
	if (!sgJimoDocMgr) init(rdata);
	aos_assert_rr(sgJimoDocMgr, rdata, 0);
	return sgJimoDocMgr->getJimoDoc(rdata, classname, version);
}


AosXmlTagPtr
AosJimoDocMgr::getJimoDoc(
		AosRundata *rdata,
		const OmnString &classname,
		const int version)
{
	mLock->lock();
	OmnString key = classname;
	key << "_" << version;
	itr_t itr = mDocMap.find(key);
	if (itr == mDocMap.end())
	{
		mLock->unlock();
		return 0;
	}

	AosXmlTagPtr doc = itr->second;
	mLock->unlock();
	return doc;
}


AosXmlTagPtr
AosJimoDocMgr::createJimoDoc(
		AosRundata *rdata,
		const OmnString &classname,
		const OmnString &libname,
		const int version,
		const OmnString &jimo_namespace,
		const OmnString &jimo_name)
{
	// 	<jimo AOSTAG_CURRENT_VERSION="xxx"
	// 			zky_classname="xxx"
	// 			zky_otype=AOSOTYPE_JIMO
	// 			zky_objid=AosObjid::composeJimoDocObjid(classname)
	// 		<versions>
	// 			<version_1>libname</version_1>
	// 			<version_2>libname</version_2>
	// 			...
	// 		</versions>
	// 	</jimo>
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"" << version << "\" "
		<< "zky_classname=\"" << classname << "\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" ";
		if (jimo_namespace != "" && jimo_name != "")
		{
			jimostr << "namespace=\"" << jimo_namespace << "\" "
				    << "name=\"" << jimo_name << "\" ";
		}
	jimostr	<< "zky_objid=\"" << AosObjid::composeJimoDocObjid(classname) << "\">"
		<< "<versions>"
		<< 		"<version_1>" << libname << "</version_1>"
		<< "</versions>"
		<< "</jimo>";


	AosXmlTagPtr doc = AosStr2Xml(rdata, jimostr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	OmnString key = classname;
	key << "_" << version;
	mDocMap[key] = doc;

	if (jimo_namespace != "" && jimo_name != "")
	{
		key = jimo_namespace;
		key << "." << jimo_name << "_" << version;
		key.toLower();
		mDocMap[key] = doc;
	}

	return doc;
}


