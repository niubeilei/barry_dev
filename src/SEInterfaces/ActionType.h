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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActionType_h
#define Aos_SdocAction_ActionType_h

#include "Util/String.h"

#define AOSACTTYPE_ADDATTR      			"addattr"
#define AOSACTTYPE_CALCSUM					"calcsum"
#define AOSACTTYPE_COMPOSESTR				"composestr"
#define AOSACTTYPE_COUNT_OCCU				"countoccur"
#define AOSACTTYPE_CREATE_DOC_BY_TEMPLATE	"createdocbytmpl"
#define AOSACTTYPE_CREATEDOC				"createdoc"
#define AOSACTTYPE_CREATELOG				"createlog"
#define AOSACTTYPE_DATAGENERATE				"datagenerate"
#define AOSACTTYPE_DELETECOUNTER    		"deletecounter"
#define AOSACTTYPE_LIMITED					"limited"
#define AOSACTTYPE_MODIFYATTR   			"modifyattr"
#define AOSACTTYPE_MODIFYCOUNTER    		"modifycounter"
#define AOSACTTYPE_MODIFYNODE				"modnode"
#define AOSACTTYPE_MODIFYRUNDATAATTR    	"modifyrundataattr"
#define AOSACTTYPE_REMOVEATTR  			    "removeattr"
#define AOSACTTYPE_REMOVEDOC    			"removedoc"
#define AOSACTTYPE_SEQNO        			"seqno"
#define AOSACTTYPE_SETATTRFROMDB			"setattrfromdb"
#define AOSACTTYPE_TRASHDOC	    			"trashdoc"
#define AOSACTTYPE_UPDATECOUNTER			"updatecounter"
#define AOSACTTYPE_VERCODE					"avercode"
#define AOSACTTYPE_START_STMC				"start_stmc"
#define AOSACTTYPE_STOP_STATEMACHINE		"stop_stmc"
#define AOSACTTYPE_TRANSSTATE				"trans_state"
#define AOSACTTYPE_SETRDATA_WORKSTR			"setrd_workstr"
#define AOSACTTYPE_RUNDLL					"rundll"			// Ken Lee 2012/02/23
#define AOSACTTYPE_ADDTIMER					"addtimer"			// Ken Lee 2012/02/29
#define AOSACTTYPE_CANCELTIMER				"canceltimer"		// Ken Lee 2012/02/29
#define AOSACTTYPE_SENDMAIL					"sendmail"
#define AOSACTTYPE_ADDTEXT					"addtext"	
#define AOSACTTYPE_RUNSDOC					"runsdoc"			// Ken Lee 2012/03/29	
#define AOSACTTYPE_SETATTR					"setattr"			// Chen Ding, 04/12/2012
#define AOSACTTYPE_SELECTDOC				"selectdoc"			// Chen Ding, 04/12/2012
#define AOSACTTYPE_SETRUNDATA_DOC			"setrdatadoc"		// Chen Ding, 04/12/2012
#define AOSACTTYPE_SETRUNDATA_VALUE			"setrdataval"		// Chen Ding, 04/12/2012
#define AOSACTTYPE_SENDDATA					"senddata"
#define AOSACTTYPE_LATINLOAD				"latinload"			// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINSTORE				"latinstore"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINFILTER				"latinfilter"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINDISTINCT			"latindistinct"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINEACHGEN				"latineachgen"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINSTREAM				"latinstream"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINSAMPLE				"latinsample"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINJOIN				"latinjoin"			// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINCOGROUP				"latincogroup"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINGROUP				"latingroup"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINCROSS				"latincross"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINORDER				"latinorder"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINLIMIT				"latinlimit"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINUNION				"latinunion"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_LATINSPLIT				"latinsplit"		// Chen Ding, 05/02/2012
#define AOSACTTYPE_RUNQUERY					"runquery"			// Chen Ding, 05/02/2012
#define AOSACTTYPE_FILTERDATA				"filterdata"		// Chen Ding, 05/06/2012
#define AOSACTTYPE_JOINIIL					"joiniil"			// Chen Ding, 05/11/2012
#define AOSACTTYPE_CREATEIMPORTDATA			"createimportdata"	// Ken Lee, 2012/05/11
#define AOSACTTYPE_IMPORTDATA				"importdata"		// Ken Lee, 2012/05/11
#define AOSACTTYPE_SLEEP					"sleep"				// Ketty , 2012/05/25
#define AOSACTTYPE_DELETEIIL				"deleteiil"			// Chen Ding, 05/26/2012
#define AOSACTTYPE_BATCHDELDOCS				"batchdeldocs"		// Chen Ding, 05/26/2012
#define AOSACTTYPE_MODIFYSTR				"modifystr"			// Chen Ding, 05/31/2012
#define AOSACTTYPE_SPLITFILE				"splitfile"			// Chen Ding, 06/02/2012
#define AOSACTTYPE_UNICOM_MODIFYRCD			"unicom_modrcd"		// Chen Ding, 06/04/2012
#define AOSACTTYPE_DATACOMBINER				"datacombiner"		// Xu Lei, 06/19/2012
#define AOSACTTYPE_RECORDCOMPOSER			"recordcomp"		// Chen Ding, 06/20/2012
#define AOSACTTYPE_GROUPBYPROC				"groupbyproc"		// Chen Ding, 06/20/2012
#define AOSACTTYPE_REMOVEWS					"rmvws"				// Brian Zhang, 06/21/2012
#define AOSACTTYPE_JOINIILMULTITHRD			"joiniilmulthrd"	// Chen Ding, 07/06/2012
#define AOSACTTYPE_DELETEIILENTRY			"deleteiilentry"	// Chen Ding, 07/06/2012
#define AOSACTTYPE_DATAJOIN					"datajoin"			// Ken Lee, 2012/07/30
#define AOSACTTYPE_SUMUNIQ					"sumuniq"			// Ken Lee, 2012/07/30
#define AOSACTTYPE_DELETEDOC    			"deletedoc"
#define AOSACTTYPE_SCANNERDATA    			"scannerdata"
#define AOSACTTYPE_SCAN_FILE				"scanfile"
#define AOSACTTYPE_SENDCOMMAND				"sendcommand"		// Chen Ding, 09/04/2012
#define AOSACTTYPE_GENREPORTDOC				"genreportdoc"		// Jozhi Peng 09/10/2012
#define AOSACTTYPE_SQLSERVERTODB			"sqlservertodb"		// Jozhi Peng 09/10/2012
#define AOSACTTYPE_MODIFYQUERY				"modifyquery"		//Jackie, 11/27/2012
#define AOSACTTYPE_RUNJOB					"runjob"		// Jozhi Peng 09/10/2012
#define AOSACTTYPE_ADDCOUNTERIILENTRY		"addcounteriilentry"

#define AOSACTTYPE_STRIILBATCHADD			"striilbatchadd"		
#define AOSACTTYPE_STRIILBATCHDEL			"striilbatchdel"		
#define AOSACTTYPE_STRIILBATCHINC			"striilbatchinc"		
#define AOSACTTYPE_STRIILBATCHDEC			"striilbatchdec"		
#define AOSACTTYPE_MERGEFILE				"mergefile"		
#define AOSACTTYPE_IMPORTDOC_FIXED			"importdoc_fixed"
#define AOSACTTYPE_IMPORTDOC_CSV			"importdoc_csv"
#define AOSACTTYPE_DELETE_GROUPDOC			"delete_groupdoc"
#define AOSACTTYPE_IMPORTDOC_NORMAL			"importdoc_normal"
#define AOSACTTYPE_IILBATCHOPR              "iilbatchopr"		
#define AOSACTTYPE_IILBATCHOPRMERGE         "iilbatchoprmerge"		
#define AOSACTTYPE_IILPATTERNOPR            "iilpatternopr"		
#define AOSACTTYPE_IILPATTERNOPR2			"iilpatternopr2"		
#define AOSACTTYPE_REBUILDBITMAPOPR         "rebuildbitmapopr"		
#define AOSACTTYPE_CREATEFILES              "actcreatefiles"    // Andy zhang, 05/22/2013 
#define AOSACTTYPE_SNAPSHOT					"snapshot"		
#define AOSACTTYPE_CREATEHITIIL				"createhitiil"		
#define AOSACTTYPE_UNCREATEFILE				"uncreatefile"		
#define AOSACTTYPE_PINGSERVICE				"pingservice"	    //fpei 2014/06/24	
#define AOSACTTYPE_IMPORTDOC2				"importdoc2"		// linda, 2012/05/28
#define AOSACTTYPE_IMPORTBINARYDOC			"importbinarydoc"	// Jozhi, 2013/06/06
#define AOSACTTYPE_IMPORTUNICOMFILE			"importunicomfile"	// Jozhi, 2013/06/19
#define AOSACTTYPE_IMPUTDATAFILEFORMAT		"importdatafileformat"	// Jozhi, 2013/06/19
#define AOSACTTYPE_SETATTRS					"setattrs"				// Ken Lee, 2013/08/05
#define AOSACTTYPE_STARTSERVER				"startserver"			// Jozhi, 2013/08/28
#define AOSACTTYPE_CREATECLUSTER			"createcluster"			// Jozhi, 2013/08/28
#define AOSACTTYPE_STOPSERVER				"stopserver"			// Jozhi, 2013/08/28
#define AOSACTTYPE_ADDSERVER				"addserver"				// Jozhi, 2013/08/28
#define AOSACTTYPE_ADDIILENTRY				"addiilentry"			// Ken Lee, 2013/08/22
#define AOSACTTYPE_SENDURL					"sendurl"				// Jackie, 2013/09/04
#define AOSACTTYPE_IMPORTSTATDOC			"importstatdoc"		
#define AOSACTTYPE_CREATEDATASETDOC			"createdatasetdoc"		

#define AOSACTTYPE_DATAENGINE_SCAN_SINGLE	"dataengine_scan_singlercd"		// Ketty, 2013/12/24
#define AOSACTTYPE_DATAENGINE_SCAN2			"dataengine_scan2"				// Barry, 2015/02/07
#define AOSACTTYPE_DATAENGINE_SCAN_MULTI	"dataengine_scan_multircd"		// Ketty, 2013/12/24
#define AOSACTTYPE_DATAENGINEJOIN           "dataengine_join"
#define AOSACTTYPE_DATAENGINEJOIN2          "dataengine_join2"
#define AOSACTTYPE_DATAENGINEJOINNEW        "dataengine_join_new"

#define AOSACTTYPE_CREATETASKFILE           "createtaskfile"		// Ketty 2014/08/20 

#define AOSACTTYPE_JIMOTABLE_BATCHOPR		"jimotable_batchopr"	// Ken Lee, 2014/09/24
#define AOSACTTYPE_INSTALL					"install"	// Jozhi, 2014/11/17
#define AOSACTTYPE_QUERYEXPORTFILE			"query_export_file"	// Jozhi, 2014/11/17

class AosActionType
{
public:
	enum E
	{
		eInvalid,

		eAddAttr,
		eSeqno,
		eComposeStr,
		eCreateDoc,
		eModifyAttr,
		eLimited,
		eRemoveAttr,
		eRemoveDoc,
		eSetAttrFromDb,
		eVerCode,

		eTrashDoc,
		eUpdateCounter,
		eModifyCounter,
		eAddCounterIILEntry,
		eModifyRundataAttr,
		eModifyNode,
		eDeleteCounter,
		eEmailModule,
		eCreateLog,
		eShmModule,
		eDataGenerate,

		eCreateDocByTemplate,		// Chen Ding, 12/27/2011
		eCalcSum,					// Chen Ding, 12/30/2011
		eCountOccur,				// Chen Ding, 12/31/2011
		eStartStmc,					// Chen Ding, 01/03/2012
		eStopStmc,					// Chen Ding, 01/03/2012
		eTransState,				// Chen Ding, 01/03/2012
		eSetRdataWorkStr,			// Chen Ding, 01/06/2012
		eRunDLL,					// Ken Lee, 2012/02/23
		eAddTimer,					// Ken Lee, 2012/02/29
		eCancelTimer,				// Ken Lee, 2012/02/29
		eSendMail,
		eAddText,
		eRunSdoc,					// Ken Lee, 2012/03/29
		eSetAttr,					// Chen Ding, 04/12/2012
		eSelectDoc,					// Chen Ding, 04/12/2012
		eSetRundataDoc,				// Chen Ding, 04/12/2012
		eSetRundataValue,			// Chen Ding, 04/12/2012
		eSendData,
		eRunQuery,

		eLatinLoad,					// Chen Ding, 05/02/2012
		eLatinStore,				// Chen Ding, 05/02/2012
		eLatinFilter,				// Chen Ding, 05/02/2012
		eLatinDistinct,				// Chen Ding, 05/02/2012
		eLatinEachGen,				// Chen Ding, 05/02/2012
		eLatinStream,				// Chen Ding, 05/02/2012
		eLatinSample,				// Chen Ding, 05/02/2012
		eLatinJoin,					// Chen Ding, 05/02/2012
		eLatinCogroup,				// Chen Ding, 05/02/2012
		eLatinGroup,				// Chen Ding, 05/02/2012
		eLatinCross,				// Chen Ding, 05/02/2012
		eLatinOrder,				// Chen Ding, 05/02/2012
		eLatinLimit,				// Chen Ding, 05/02/2012
		eLatinUnion,				// Chen Ding, 05/02/2012
		eLatinSplit,				// Chen Ding, 05/02/2012
		eFilterData,				// Chen Ding, 05/06/2012
		
		eCreateImportData,			// Ken Lee, 2012/05/11
		eImportData,				// Ken Lee, 2012/05/11
		eJoinIIL,					// Chen Ding, 05/11/2012
		eSleep,						// Ketty 2012/05/25
		eDeleteIIL,					// Chen Ding, 05/26/2012
		eBatchDelDocs,				// Chen Ding, 05/26/2012
		eModifyStr,					// Chen Ding, 05/31/2012
		eSplitFile,					// Chen Ding, 06/02/2012

		eUnicomModifyRcd,			// Chen Ding, 06/04/2012
		eDataCombiner,				// Xu Lei, 06/19/2012
		eRecordComposer,			// Chen Ding, 06/20/2012
		eGroupbyProc,				// Chen Ding, 06/20/2012
		eRemoveWS,					// Brian Zhang, 06/21/2012
		eJoinIILMultiThrd,			// Chen Ding, 07/06/2012
		eDeleteIILEntry,			// Chen Ding, 07/06/2012
		eDataJoin,					// Ken Lee, 2012/07/30
		eSumUniq,
		eDeleteDoc,					// Jackie zhao 08/09/2012
		eScannerData,
		eScanFile,					// Chen Ding, 08/29/2012
		eSendCommand,				// Chen Ding, 09/04/2012
		eGenReportDoc,				// Chen Ding, 09/04/2012
		eSqlServerToDb,				// Chen Ding, 09/04/2012
		eModifyQuery,
		eRunJob,					// Chen Ding, 09/04/2012

		eStrIILBatchAdd,			// Ken Lee, 2012/11/29
		eStrIILBatchDel,			// Ken Lee, 2012/11/29
		eStrIILBatchInc,			// Ken Lee, 2012/11/29
		eStrIILBatchDec,			// Ken Lee, 2012/11/29
		eMergeFile,					// Felicia, 2013/05/02
		eImportDocFixed,			// Ken Lee, 2013/12/30
		eImportDocCSV,				// Ken Lee, 2013/12/30
		eDeleteGroupDoc,				// Ken Lee, 2013/12/30
		eIILBatchOpr,				// Felicia, 2013/05/02
		eIILPatternOpr,				// Felicia, 2014/08/18
		eIILPatternOpr2,			// Felicia, 2014/08/18
		eCreateFiles,               // Andy zhang, 05/22/2013

		eSnapShot,					// Felicia, 2013/05/22
		eIILBatchOprMerge,			// Ken Lee, 2013/05/22
		eCreateHitIIL,				// felicia, 2013/05/24
		eUnCreateFile,				// Andy zhang, 2013/06/04
		eImportDoc2,				// linda, 2013/05/28
		eImportBinaryDoc,			// Jozhi, 2013/06/06
		eImportUnicomFile,			// Jozhi, 2013/06/19
		eImputDataFileFormat,	
		eSetAttrs,					// Ken Lee, 2013/08/05
		eStartServer,				// Jozhi, 2013/08/28
		eCreateCluster,				// Jozhi, 2013/08/28
		eStopServer,				// Jozhi, 2013/08/28
		eAddServer,					// Jozhi, 2013/08/28
		eAddIILEntry,				// Ken Lee, 2013/08/22
		eSendUrl,					// Jackie, 2013/09/04
		
		eScanDataEngineSingleRcd,	// Ketty 2013/12/24
		eScanDataEngine2,			// Barry 2015/02/07
		eScanDataEngineMultiRcd,	// Ketty 2013/12/24
		
		eJimoAction,				// Chen Ding, 2013/12/14
		eJoinDataEngine,
		eJoinDataEngineNew,
		eJoinDataEngine2,
		eImportStatDoc,
		eCreateDatasetDoc,
		eRebuildBitmapOpr,			// Shawn, 2013/12/05
		eImportDocNormal,	
		ePingService,	            // fpei, 2014/06/24
		
		eCreateTaskFile,			// Ketty 2014/08/20

		eJimoTableBatchOpr,			// Ken Lee, 2014/09/25
		eInstall,					// Jozhi, 2014/11/17
		eQueryExportFile,			// Jozhi, 2014/11/19

		eMax
	};

	static E toEnum(const OmnString &name);
	static bool isValid(const E code) {return code>eInvalid && code<eMax;}
	static bool addName(const OmnString &name, const E e, OmnString &errmsg);
	static OmnString toString(const E e);
	static bool check();
};
#endif

