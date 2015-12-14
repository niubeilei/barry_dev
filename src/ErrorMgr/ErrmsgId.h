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
// 08/22/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ErrorMgr_ErrmsgId_h
#define Aos_ErrorMgr_ErrmsgId_h

#include "Util/String.h"


class AosErrmsgId
{
public:
	enum E
	{
		eInvalid, 
		eOk,

		eAccessDenied = 10,
		eCondSyntaxError,
		eContainerNotFound,
		eDataNull,
		eDocAlreadyDeleted,

		eDocNotFound,
		eDocidHasNoDoc,
		eDocidIsNull,
		eDocidIsInvalid,
		eDocidMismatch,

		eEntryNotUnique,
		eErrorFormat,
		eExceptionCreateUserDomain,
		eExceptionCreateUserAcct,
		eExceptionCreateArcd,

		eExceptionCreateFile,
		eExceptionCreateLoginfo,
		eExceptionCreateSeLog,
		eExceptionCreateSysdoc,
		eExceptionCreateTerm,

		eExceptionCreateDoc,
		eExceptionCreateIIL,
		eFailedAddValueToIIL,
		eFailedAllocatingSpace,		//Ketty 2011/09/07
		eFailedCreateDir,			//Ketty 2011/09/07

		eFailedCreateIIL,
		eFailedCreateIILName,
		eFailedCreateLogFile,
		eFailedCreatingFile,
		eFailedCreatingGuestUser,

		eFailedCreatingRootAcct,
		eFailedCreatingSysRoot,
		eFailedCreatingUnknownUser,
		eFailedLoadDoc,
		eFailedOpenFile,
		
		eFailedOpenLogFile,
		eFailedReadingIdxFile,
		eFailedRetrieveArcd,
		eFailedRetrieveData,
		eFailedRetrieveDoc,

		eFailedRetrieveDocid,
		eFailedRetrieveDocByCloudid,
		eFailedRetrieveDocByDocid,
		eFailedRetrieveDocByObjid,
		eFailedRetrieveLog,

		eFailedRetrieveLogFile,
		eFailedRetrieveSite,
		eFailedRetrieveSystemDoc,
		eFailedRetrieveUserAcct,
		eFailedSave,

		eFailedToDoAction,
		eFailedToRunSmartDoc,
		eFailedToRetrieveVersionDoc,
		eFailedAddProc,	
		eFileDeleted,		//Ketty 2011/09/07

		eGicSyntaxError,
		eIDGenNotDefined,
		eInternalError,
		eInvalidAging,
		eInvalidErrorId,

		eInvalidParm,
		eInvalidOperation,
		eInvalidObjid,
		eWrongOperation,
		eLogDftObj,

		eDoLogActionFailed,
		eLogContainerEmpty,
		eLogDirectoryEmpty,
		eLogNameIsNull,
		eLogNotExist,

		eLogInitFileFailed,
		eFailedModifyHashFile,
		eFailedDeleteFromIIL,
		eFailedDeleteLog,
		eFailedReadingLogEntry,

		ePoisonFailed,
		eLogNotFound,
		eLogTooLong,
		eLogTypeMismatch,
		eExceptionCreateLogEntry,

		eFailedToGetLogEntry,
		eFailedSaveToLogFile,
		eFailedDeleteOldestFile,
		eMissingLogid,
		eMissMatch,

		eFailedRetrieveSeqIIL,
		eRetrieveLogidsFailed,
		eMissingCloudid,
		eMissingCondition,
		eMissingContainer,

		eMissingDoc,
		eMissingDocCreator,
		eMissingDocid,
		eMissingFieldname,
		eMissingFilename,

		eMissingIILSelector,
		eMissingMValues,
		eMissingNextState,
		eMissingParentContainer,
		eMissingObject,

		eMissingObjid,
		eMissingQuery,
		eMissingQueryRslt,
		eMissingReferenceDoc,
		eMissingSiteid,

		eMissingSmartDoc,
		eMissingTags,
		eMissingUserDoc,
		eMissingValueSelector,
		eMissingVersion,

		eMissingVersionDoc,
		eMissingOperation,
		eMissingReceiver,
		eMissingContents,
		eMissingVirtualCtnrId,

		eMissingWorkingDoc,
		eMissingResp,
		eMissingValue,
		eMissingAttrname,
		eMissingType,

		eNoDocidByObjid,
		eNoDocs,
		eNoError,
		eNoObjectSpecified, 
		eNotAccessRecord, 

		eNotDefined, 
		eNotDocCreator,
		eNotUserAccount, 
		eNotSystemDoc, 
		eQueryIncorrect,

		eParseXmlFailed,
		eSendShortMsgFailed,
		eQueryFailed,
		eObjidIsNull,
		eObjidHasNoDocid,

		eObjidMismatch,
		eObjidSyntaxError,
		eObjidTooLong,
		eObjidTooShort,
		eAttrError,

		eRunOutOfMemory,
		eSmartDocIncorrect,
		eSmartDocSyntaxError,
		eStatemachineSyntaxError,
		eTooManyTerms,

		eTTLError,
		eValueIncorrect,
		eValueIsXml,
		eValueNotFound,
		eFailedGetValue,
		
		eUnrecognizedTermType,
		eUnrecognizedVpdType,
		eInvalidEventType,
		eDataTooLong,					// Chen Ding, 2014/11/09
		eFailedCreatingDatalet,			// Chen Ding, 2014/11/09
		eInvalidBSONField,				// Barry Niu, 2014/11/14
		eInvalidBSONFieldNameType,		// Barry Niu, 2014/11/15

		eMissingDataProcName,			// Barry Niu, 2014/11/12
		eTorturerNotFound,				// Levi, 	  2014/11/13
		eFailedCreatingDataField,		// Chen Ding, 2014/11/17

		eMissingDataFieldConfig,		// Chen Ding, 2014/11/17
		eJimoNameEmpty,					// Chen Ding, 2014/11/17
		eVirtualFieldMissingEntries,	// Chen Ding, 2014/11/17
		eFieldTooLong,					// Chen Ding, 2014/12/04
		eInvalidDataType,				// Chen Ding, 2014/12/04
		eInvalidEscapeMethod,			// Chen Ding, 2014/12/04
		eInvalidConfig,					// Chen Ding, 2014/12/05
		eInvalidFunction,				// Chen Ding, 2014/12/05
		eInvalidPackage,				// Chen Ding, 2014/12/05
		eFuncNotFound,					// Chen Ding, 2014/12/05
		eUnrecognizedJimoCallID,		// Chen Ding, 2014/12/05
		eJimoCallPackageNotFound,		// Chen Ding, 2014/12/06
		eMissingPackageID,				// Chen Ding, 2014/12/06
		eInvalidEndpoint,				// Chen Ding, 2014/12/06
		eEndpointNotFound,				// Chen Ding, 2014/12/06
		eMissingConfig,					// Chen Ding, 2014/12/06
		eInvalidAddr,					// Chen Ding, 2014/12/06
		eInvalidMessage,				// Chen Ding, 2014/12/13
		eFailedMakingCall,				// Chen Ding, 2014/12/13

		//BlobSE	by White 2014-12-23
		eDataletDoesNotExist,
		eActiveLogFileAppendFailed,
		eReadDocFailed,
		eDataInconsistent,
		eMemcpyError,
		eCreateNewActiveLogFile,
		eDataTooBig,
		eConstructEntry,
		eConstructHeader,
		eConstructHeaderCache,
		eAppendHeaderChangeLog,
		eSaveDelta,
		eRedoHeaderChange,
		eReadHeaderFromFile,
		eGetPrevEntry,
		eReadFileToBuff,
		eGetPrevActiveChangeLogFile,
		eResetBuf,
		eCloseHeaderChangeLogFile,
		eWriteHeaderFile,

		eMaxErrorCode
	};

private:
	static OmnString		mNames[eMaxErrorCode+1];

public:
	static E toEnum(const OmnString &id);
	static bool init();
	static bool isValid(const E id) {return id > eInvalid && id < eMaxErrorCode;}
	static OmnString getErrmsg(E id)
	{
		if (id <= eInvalid || id >= eMaxErrorCode) id = eInvalidErrorId;
		return mNames[id];
	}
};
#endif
