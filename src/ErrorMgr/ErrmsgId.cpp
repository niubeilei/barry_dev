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
// 08/27/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ErrorMgr/ErrmsgId.h"


OmnString AosErrmsgId::mNames[AosErrmsgId::eMaxErrorCode+1];

AosErrmsgId::E 
AosErrmsgId::toEnum(const OmnString &id)
{
	for (int i=0; i<eMaxErrorCode; i++)
	{
		if (mNames[i] == id) return (E) i;
	}

	return eInvalid;
}


bool 
AosErrmsgId::init()
{
	mNames[eInvalid] 							= "invalid";
	mNames[eMaxErrorCode] 						= "max";

	mNames[eAccessDenied]						= "access_denied";
	mNames[eAttrError]							= "attribute_error";

	mNames[eContainerNotFound]					= "ctnr_not_found";
	mNames[eCondSyntaxError]					= "cond_syn_error";

	mNames[eErrorFormat]					    = "error_format";
	mNames[eDataNull]					        = "Data_null";
	mNames[eDocAlreadyDeleted]					= "doc_already_del";
	mNames[eDocidHasNoDoc]						= "docid_no_doc";
	mNames[eDocidIsNull]						= "docid_is_null";
	mNames[eDocidIsInvalid]						= "docid_is_invalid";
	mNames[eDocidMismatch]						= "docid_mismatch";
	mNames[eDocNotFound]						= "doc_not_found";

	mNames[eEntryNotUnique]						= "entry_not_unique";
	mNames[eExceptionCreateUserDomain]			= "exp_crt_userdm";
	mNames[eExceptionCreateUserAcct]			= "exp_crt_useracct";
	mNames[eExceptionCreateArcd]				= "exp_crt_arcd";
	mNames[eExceptionCreateFile]				= "exp_crt_file";
	mNames[eExceptionCreateLoginfo]				= "exp_crt_loginfo";
	mNames[eExceptionCreateSeLog]				= "exp_crt_selog";
	mNames[eExceptionCreateSysdoc]				= "exp_crt_sysdoc";
	mNames[eExceptionCreateTerm]				= "exp_crt_term";
	mNames[eExceptionCreateDoc]					= "exp_crt_doc";
	mNames[eExceptionCreateIIL]					= "exp_crt_iil";

	mNames[eFailedAddValueToIIL]				= "failed_addval_iil";
	mNames[eFailedAllocatingSpace]				= "failed_alocate_space";	//Ketty 2011/09/07
	mNames[eFailedCreateDir]					= "failed_create_dir";	//Ketty 2011/09/07
	mNames[eFailedCreateIIL]					= "failed_createiil";
	mNames[eFailedCreateIILName]				= "failed_createiilname";
	mNames[eFailedCreateLogFile]				= "failed_createlogfile";
	mNames[eFailedCreatingFile]					= "failed_createfile";
	mNames[eFailedCreatingGuestUser]			= "failed_crt_guestuser";
	mNames[eFailedCreatingRootAcct]				= "failed_crt_rootacct";
	mNames[eFailedCreatingSysRoot]				= "failed_crt_sysroot";
	mNames[eFailedCreatingUnknownUser]			= "failed_crt_unknown_usr";
	mNames[eFailedLoadDoc]						= "failed_load_doc";
	mNames[eFailedModifyHashFile]				= "failed_modhashfile";
	mNames[eFailedOpenFile]						= "failed_openfile";
	mNames[eFailedOpenLogFile]					= "failed_openlogfile";
	mNames[eFailedReadingIdxFile]				= "failed_read_idxfile";
	mNames[eFailedRetrieveArcd]					= "failed_retarcd";
	mNames[eFailedRetrieveData]					= "failed_ret_data";
	mNames[eFailedRetrieveDoc]					= "failed_retdoc";
	mNames[eFailedRetrieveDocid]				= "failed_retdocid";
	mNames[eFailedRetrieveDocByCloudid]			= "failed_retdoc_cid";
	mNames[eFailedRetrieveDocByDocid]			= "failed_retdoc_bdid";
	mNames[eFailedRetrieveDocByObjid]			= "failed_retdoc_boid";
	mNames[eFailedRetrieveLog]					= "failed_ret_log";
	mNames[eFailedRetrieveLogFile]				= "failed_ret_logfile";
	mNames[eFailedRetrieveSite]					= "failed_ret_site";
	mNames[eFailedRetrieveSystemDoc]			= "failed_ret_sysdoc";
	mNames[eFailedRetrieveUserAcct]				= "failed_ret_useracct";
	mNames[eFailedSave]							= "failed_save";
	mNames[eLogTypeMismatch]					= "log_type_missmath";
	mNames[eExceptionCreateLogEntry]			= "create_log_exception";
	mNames[eFailedToGetLogEntry]				= "get_log_entry_failed";
	mNames[eFailedToDoAction]		     		= "failed_to_do action";
	mNames[eFailedToRunSmartDoc]		   		= "failed_to_run_smartdoc";
	mNames[eLogTypeMismatch]			   		= "log_type_missmath";
	mNames[eExceptionCreateLogEntry]	   		= "create_log_exception";
	mNames[eFailedToGetLogEntry]		   		= "get_log_entry_failed";
	mNames[eFailedAddProc]						= "failed_add_proc";
	mNames[eFileDeleted]						= "file_has_deleted";		//Ketty 2011/09/07

	mNames[eGicSyntaxError]						= "gic_syntaxerror";

	mNames[eIDGenNotDefined]					= "idgen_not_dfd";
	mNames[eInternalError]						= "internal_error";
	mNames[eInvalidAging]						= "invalid_aging";
	mNames[eInvalidErrorId]						= "invalid_errorid";
	mNames[eInvalidParm]						= "invalid_parm";
	mNames[eInvalidOperation]					= "invalid_operatiion";
	mNames[eWrongOperation]						= "wrong_operatiion";

	mNames[eLogContainerEmpty]					= "logctnr_empty";
	mNames[eLogDirectoryEmpty]					= "logdir_empty";
	mNames[eLogNameIsNull]						= "logname_null";
	mNames[eLogNotExist]						= "log_not_exist";
	mNames[eLogInitFileFailed]					= "log_init_file_failed";
	mNames[eFailedSaveToLogFile]				= "save_log_failed";
	mNames[eFailedDeleteOldestFile]				= "oldest_log_file_delete_failed";
	mNames[eFailedSaveToLogFile]				= "save_log_failed";
	mNames[eFailedDeleteOldestFile]				= "oldest_log_file_delete_failed";
	mNames[eDoLogActionFailed]					= "log_action_failed";
	mNames[eRetrieveLogidsFailed]				= "failed_retrieve logids";

	mNames[eMissingLogid]						= "missing_logid";
	mNames[eMissingCloudid]						= "missing_cid";
	mNames[eMissingCondition]					= "missing_cond";
	mNames[eMissingContainer]					= "missing_ctnr";
	mNames[eMissingDoc]							= "missing_doc";
	mNames[eMissingDocCreator]					= "missing_doc_crt";
	mNames[eMissingDocid]						= "missing_docid";
	mNames[eMissingFieldname]					= "missing_fieldname";
	mNames[eMissingFilename]					= "missing_fname";
	mNames[eMissingIILSelector]					= "missing_iilsel";
	mNames[eMissingMValues]						= "missing_mvalues";
	mNames[eMissingNextState]					= "missing_next_st";
	mNames[eMissingObject]	 			        = "missing_Object";
	mNames[eMissingObjid]	 			        = "missing_Objid";
	mNames[eMissingParentContainer]				= "missing_pctnr";
	mNames[eMissingQuery]						= "missing_query";
	mNames[eMissingQueryRslt]					= "missing_queryrslt";
	mNames[eMissingReferenceDoc]				= "missing_ref_doc";
	mNames[eMissingSiteid]						= "missing_siteid";
	mNames[eMissingSmartDoc]					= "missing_sdoc";
	mNames[eMissingTags]						= "missing_tags";
	mNames[eMissingUserDoc]						= "missing_userdoc";
	mNames[eMissingValueSelector]				= "missing_valsel";
	mNames[eMissingVersion]						= "missing_version";
	mNames[eMissingVersionDoc]					= "missing_verdoc";
	mNames[eMissingResp]						= "missing_response";
	mNames[eMissingOperation]					= "missing_operation";
	mNames[eMissingContents]					= "missing_contents";
	mNames[eMissingReceiver]					= "missing_receiver";
	mNames[eMissingResp]						= "missing_response";
	mNames[eMissingOperation]					= "missing_operation";
	mNames[eMissingContents]					= "missing_contents";
	mNames[eMissingReceiver]					= "missing_receiver";
	mNames[eMissingValue]						= "missing_value";
	mNames[eMissingVirtualCtnrId]				= "missing_vcid";
	mNames[eMissingWorkingDoc]					= "missing_wkdoc";
	mNames[eFailedToRetrieveVersionDoc]			= "failed_retrieve_verdoc";
	mNames[eMissingResp]						= "missing_response";
	mNames[eMissingAttrname]					= "missing_attrname";
	mNames[eMissMatch]							= "mismatch";

	mNames[eNoDocidByObjid]						= "no_docid_by_objid";
	mNames[eNoDocs]								= "no_docs";
	mNames[eNoError]							= "no_error";
	mNames[eNoObjectSpecified]					= "no_obj_specified";
	mNames[eNotAccessRecord]					= "not_arcd";
	mNames[eNotDefined]							= "not_defined";
	mNames[eNotDocCreator]						= "not_doccreator";
	mNames[eNotUserAccount]						= "not_user_acct";
	mNames[eNotSystemDoc]						= "not_sysdoc";

	mNames[eQueryIncorrect]						= "query_incorrect";
	mNames[eParseXmlFailed]						= "xml_parse_failed";
	mNames[eSendShortMsgFailed] 				= "send_short_message_failed";
	mNames[eParseXmlFailed]						= "xml_parse_failed";
	mNames[eSendShortMsgFailed] 				= "send_short_message_failed";
	mNames[eQueryFailed]						= "query_failed";

	mNames[eObjidIsNull]						= "objid_null";
	mNames[eObjidHasNoDocid]					= "objid_hasnodocid";
	mNames[eObjidMismatch]						= "objid_mismatch";
	mNames[eObjidSyntaxError]					= "objid_synerr";

	mNames[eRunOutOfMemory]						= "outof_memory";

	mNames[eSmartDocIncorrect]					= "sdoc_incorrect";
	mNames[eSmartDocSyntaxError]				= "sdoc_synerror";
	mNames[eStatemachineSyntaxError]			= "sm_syntax_error";

	mNames[eTooManyTerms]						= "too_many_terms";
	mNames[eTTLError]							= "ttl_error";
	
	mNames[eValueIncorrect]						= "value_incorrect";
	mNames[eValueIsXml]							= "value_is_xml";
	mNames[eValueNotFound]						= "value_not_found";
	mNames[eFailedGetValue]						= "failed_get_value";

	mNames[eUnrecognizedTermType]				= "unrcg_termtype";
	mNames[eUnrecognizedVpdType]				= "unrcg_vpdtype";
	mNames[eInvalidEventType]					= "valid_event_type";

	return true;
}

