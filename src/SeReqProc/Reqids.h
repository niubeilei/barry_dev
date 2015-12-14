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
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_SeReqids_h
#define Omn_ReqProc_SeReqids_h

#include "Util/String.h"



class AosSeReqid
{
public:
	enum E 
	{
		eInvalid,

		eAddFriend,
		eAddFollower,
		eAddMember,
		eAllowanceTask,
		eApplyFriend,
		eAppendManualOrder,
		eBatchDelete,
		eCheckCloudId,
		eCheckLogin,
		eCounter,
		eCreateAccessRecord,
		eCreateObjByTpl,
		eCreateOrder,
		eCreateSuperUser,
		eCreateTalkGroup,
		eCreateUser,
		eCreateUserDomain,
		eComposerTN,
		eConvertSqlData,
		eCopyDocsFromCtnr,
		eDownload,
		eDenyFriend,
		eExportData,
		eForum,
		eFinishIMSession,
		eGetAccessRecord,
		eGetOnlineUser,
		eGetOnlineFriend,
		eGetDomain,
		eGetNewId,
		eGetCloudId,
		eIncrementAttr,	
		eInviteFriend,
		eModifyObjByTpl,
		eMoveManualOrder,
		eModifyAttr,
		eProcOnlineStatus,
		eRegisterHook,
		eRemoveMember,
		eRemoveFriend,
		eRemoveFriendGroup,
		eRemoveFollower,
		eRemoveManualOrder,
		eRemoveMBlogMsg,
		eRemoveTalkGroup,
		eSendInstantMsg,
		/////////////////////
		eAddSender,
		eRemoveSender,
		eAddFollowerToAcct,
		eRemoveFollowerFromAcct,
		eSendMsgByAccount,
		eSendMsgToUser,
		/////////////////////
		eSendMicroBlogMsg,
		eSwapManualOrder,
		eUserLogOut,
		eModifyAccessRecord,
		eCreateMsgContainer,
		eDeleteMsgContainer,
		eDeleteMsg,
		eCreateInbox,
		eMoveMsg,
		eNotifyFriend,
		eCreateMsg,
		eAddSchedule,
		eGetLanguage,
		eSendShortMsg,
		eShmConfirm,
		eAliPay,
		eCreateComment,
		eLgEsvr,
		eVote,
		eGetUserdocByCld,
		eCreateLogEntry,
		ePublishUrl,
		eResolveUri,
		eResolveUrl,
		eRebuildVersion,
		eRebuildLog,
		eGetServerTime,
		eRunSmartdoc,
		eThumbNail,
		eUploadImgReq,
		eUploadFileReq,
		eCreateObj,
		eCreateLog,
		eDataSync,
		eLogin,
		eSendMail,
		eModifyNode,
		eRetrieveLog, 
		eAddVersion, 
		eStopServer, 
		eOneKeyCopy,
		eRecentVpd,
		eResumeReq,
		eSmartReply,	// Ketty 2011/11/03
		eCheckUnique,
		eModifyPassword,
		eScanObjid,
		eIncrementInlineSearch,	//ken 2011/12/08
		eCreateOprArd,			//Chen Ding, 2011/12/26
		eGetUserOprArd,			//Chen Ding, 2011/12/26
		eDocLockReq,
		eProcEcg,
		eCreateEcgImg,
		eContecEcgProc,
		eUpdateLoginInfo,
		eGetServerInfo,
		eRunCommand,
		eSendToMsgService,
		eCheckIILEntry,		// Ken Lee, 2013/08/20
		eRunJQL,			// Andy Zhang, 2013/10/17
		eRunIndexMgr,		// felicia, 2014/01/20
		eJqlLogin,			// Jozhi, 2015/01/28
		eRunSql,			// Jozhi, 2015/01/29

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e, OmnString &errmsg);
};
#endif

