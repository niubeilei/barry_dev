////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ErrId.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Debug_ErrId_h
#define Omn_Debug_ErrId_h



class OmnErrId
{
public:
	enum E
	{
		eFirstValidEntry,

		eIncorrect,		// Do not use this value. Reserved by OmnAlarmMgr.
		eNoError,
		eInvalid,
		eUnknown,
		eGeneral,

		e200Ok,

		eAlarm,
		eAlarmInvalid,
		eAlarmMutexError,
		eAlarmSingletonError,
		eAlarmAssertFailed,
		eAlarmProgramError,
		eAlarmHeartbeatError,
		eAlarmConfigError,
		eAlarmNetworkError,
		eAlarmDatabaseError,
		eAlarmOutOfBound,
		eAlarmOutOfMemory,

		eAddSocketError,
		eAssertFail,

		eBuffTooBig,

		eCheckFail,
		eCommError,
		eCondVarError,
		eConfigError,
		eConnBuffMemoryError,
		eCreateLdeError,

		eDatabaseError,
		eDbError,
		eDNSNotConfigured,

		eEntryAlreadyExist,
		eException,

		eFailedToAddRepository,
		eFailedToAddToDatabase,
		eFailedToAddToQueue,
		eFailedToCreateMsgFromBuffer,
		eFailedToCreatePath,
		eFailedToCreateTimerSocket,
		eFailedToNegotiateCompAlg,
		eFailedToNegotiateEncryptAlg,
		eFailedToQueryDB,
		eFailedToRetrieveRealm,
		eFailedToRetrieveView,
		eFailedToSend,
		eFileError,

		eHousekeepingTimedout,

		eImageNotFound,
		eIncorrectMsg,
		eInfobusError,
		eInvalidDomainName,
		eInvalidLogId,
		eInvalidRecvDn,
		eInvalidRecverAddr,
		eInvalidScheduleId,
		eInvalidSocketError,
		eItemNotFound,

		eLdeAlreadyInVM,
		eListenerNull,
		eLoggerIsNull,

		eMemoryFailed,
		eMsgContentError,
		eMsgCreationError,
		eMsgRecvError,
		eMsgSendError,

		eNoDNSInfoAvailable,
		eNoMasterRealmImage,
		eNoStreamer,
		eNullPointer,

		eObjectAlreadyExists,
		eObjectAlreadyUpdated,
		eObjectNotFound,
		eOutOfBound,

		eParserError,
		eParserMissingBuffer,
		ePointedToLDEDoesNotExist,
		eProgError,

		eRealmNotDefined,
		eRegisterListenerError,
		eRepositoryIsNull,
		eRetransDroppedToNormal,
		eRetransError,
		eRPCFailed,

		eSendMsgError,
		eSerializeToError,
		eSerializeFromError,
		eShouldNeverComeToThisPoint,
		eShouldNeverHappen,
		eSingletonError,
		eStopReadingError,
		eSocketAcceptError,
		eSocketReadError,
		eSocketReadingTimeout,
		eSocketSelectError,
		eSocketTooBig,
		eStreamError,
		eStreamFailed,
		eStreamIdError,
		eStreamRecvError,
		
		eSynErr,

		eTimeout,
		eTooManyEventEntries,

		eUnclaimedMsg,
		eUnclaimedStream,
		eUnclaimedStreamMsg,
		eUnclaimedTcpPacket,
		eUnexpectedMsg,
		eUnknownException,
		eUnrecognizedEnumValue,
		eUnrecognizedFuncName,
		eUnrecognizedImageId,
		eUnrecognizedPDEType,

		eVmDoesNotExist,
		eVmPathDoesNotExist,

		eWarning,
		eWarnConfigError,
		eWarnDatabaseError,
		eWarnProgramError,
		eWarnCheckFail,
		eWarnFileError,
		eWarnCommError,
		eWarnSyntaxError,

		eSyntaxError,
 
		eIOError,
		eLastValidEntry				// Do not add any entry beyond this line

	};

	static const char * const 	toStr(const E c);
	static E verifyErrId(const E c)
	{
		return (c >= eFirstValidEntry && c < eLastValidEntry)? c:eInvalid;
	}
	static bool isValid(const E c)
	{
		return (c > eFirstValidEntry && c < eLastValidEntry);
	}
};
#endif

