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
// 01/23/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/AllSeReqProcs.h"

#include "Thread/Mutex.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/AddFriend.h"
#include "SeReqProc/AddFollower.h"
#include "SeReqProc/AddMember.h"
#include "SeReqProc/AddVersion.h"
#include "SeReqProc/AliPay.h"
#include "SeReqProc/AllowanceTask.h"
#include "SeReqProc/AppendManualOrder.h"
#include "SeReqProc/ApplyFriend.h"
#include "SeReqProc/BatchDelete.h"
#include "SeReqProc/CheckLogin.h"
#include "SeReqProc/CheckCloudId.h"
#include "SeReqProc/ComposerTN.h"
#include "SeReqProc/CopyDocsFromCtnr.h"
#include "SeReqProc/Counter.h"
#include "SeReqProc/CreateAccessRecord.h"
#include "SeReqProc/CreateComment.h"
#include "SeReqProc/CreateLogEntry.h"
#include "SeReqProc/CreateSuperUser.h"
#include "SeReqProc/CreateUser.h"
#include "SeReqProc/CreateUserDomain.h"
#include "SeReqProc/CreateOrder.h"
#include "SeReqProc/CreateTalkGroup.h"
#include "SeReqProc/CreateInbox.h"
#include "SeReqProc/CreateMsgContainer.h"
#include "SeReqProc/CreateObj.h"
#include "SeReqProc/CreateObjByTpl.h"
#include "SeReqProc/CreateMsg.h"
#include "SeReqProc/ConvertSqlData.h"
#include "SeReqProc/DataSync.h"
#include "SeReqProc/DeleteMsgContainer.h"
#include "SeReqProc/DeleteMsg.h"
#include "SeReqProc/Download.h"
#include "SeReqProc/DocLockReq.h"
#include "SeReqProc/DenyFriend.h"
#include "SeReqProc/ExportData.h"
#include "SeReqProc/Forum.h"
#include "SeReqProc/FinishIMSession.h"
#include "SeReqProc/GetAccessRecord.h"
#include "SeReqProc/GetCloudId.h"
#include "SeReqProc/GetDomain.h"
#include "SeReqProc/GetLanguage.h"
#include "SeReqProc/GetNewId.h"
#include "SeReqProc/GetServerTime.h"
#include "SeReqProc/GetOnlineUser.h"
#include "SeReqProc/GetOnlineFriend.h"
#include "SeReqProc/GetUserdocByCld.h"
#include "SeReqProc/IncrementAttr.h"
#include "SeReqProc/Login.h"
#include "SeReqProc/LoginToEsvr.h"
#include "SeReqProc/ModifyAccessRecord.h"
#include "SeReqProc/ModifyNode.h"
#include "SeReqProc/ModifyObjByTpl.h"
#include "SeReqProc/MoveManualOrder.h"
#include "SeReqProc/MoveMsg.h"
#include "SeReqProc/ModifyAttr.h"
#include "SeReqProc/NotifyFriend.h"
#include "SeReqProc/OneKeyCopy.h"
#include "SeReqProc/ProcOnlineStatus.h"
#include "SeReqProc/PublishUrl.h"
#include "SeReqProc/RebuildLog.h"
#include "SeReqProc/RebuildVersion.h"
#include "SeReqProc/RegisterHook.h"
#include "SeReqProc/RecentVpd.h"
#include "SeReqProc/RemoveFollower.h"
#include "SeReqProc/RemoveFriend.h"
#include "SeReqProc/RemoveFriendGroup.h"
#include "SeReqProc/RemoveManualOrder.h"
#include "SeReqProc/RemoveMember.h"
#include "SeReqProc/RemoveMicroBlogMsg.h"
#include "SeReqProc/RemoveTalkGroup.h"
#include "SeReqProc/ResolveUri.h"
#include "SeReqProc/ResolveUrl.h"
#include "SeReqProc/RetrieveLog.h"
#include "SeReqProc/RunSmartdoc.h"
#include "SeReqProc/SendInstantMsg.h"
#include "SeReqProc/SendMail.h"
#include "SeReqProc/SendMicroBlogMsg.h"
#include "SeReqProc/AddSender.h"
#include "SeReqProc/RemoveSender.h"
#include "SeReqProc/AddFollowerToAcct.h"
#include "SeReqProc/RemoveFollowerFromAcct.h"
#include "SeReqProc/SendMsgByAccount.h"
#include "SeReqProc/SendMsgToUser.h"
#include "SeReqProc/SendShortMsg.h"
#include "SeReqProc/ShmConfirm.h"
#include "SeReqProc/SmartReply.h"
#include "SeReqProc/StopServer.h"
#include "SeReqProc/SwapManualOrder.h"
#include "SeReqProc/UploadFileReq.h"
#include "SeReqProc/UploadImgReq.h"
#include "SeReqProc/UserLogOut.h"
#include "SeReqProc/Vote.h"
#include "SeReqProc/CheckUnique.h"
#include "SeReqProc/ModifyPassword.h"
#include "SeReqProc/IncrementInlineSearch.h"	//ken 2011/12/08
#include "SeReqProc/ScanObjid.h"
#include "SeReqProc/CreateEcgImg.h"
#include "SeReqProc/UpdateLoginInfo.h"
#include "SeReqProc/GetServerInfo.h"
#include "SeReqProc/RunCommand.h"
#include "SeReqProc/CheckIILEntry.h"			// Ken lee, 2013/08/20
#include "SeReqProc/RunJQL.h"					// Andy Zhang, 2013/10/17
#include "SeReqProc/RunIndexMgr.h"				// Felicia, 2014/01/20
#include "SeReqProc/JqlLogin.h"					// Jozhi, 2015/01/29
#include "SeReqProc/RunSql.h"					// Jozhi, 2015/01/29

// #include "CustomizedProcs/Contec/ContecEcgProc.h"
// #include "CustomizedProcs/Contec/ProcEcg.h"
// #include "CustomizedProcs/Contec/SendToMsgService.h"
AosSeRequestProcPtr	sgSeRequestProcs[AosSeReqid::eMax];
AosStr2U32_t 		sgReqidMap;

AosAllSeReqProcs gAosAllSeReqProcs;

AosAllSeReqProcs::AosAllSeReqProcs()
{
// Chen Ding, 2013/02/09
// static AosProcEcg gAosProcEcg(true);
// static AosSendToMsgService gAosSendToMsgService(true);
static AosAddFollower				sgAddFollower(true);
static AosAddFriend					sgAddFriend(true);
static AosAddMember					sgAddMember(true);
static AosAddVersion				sgAddVersion(true);
static AosAllowanceTask				sgAllowanceTask(true);
static AosAliPay					sgAliPay(true);
static AosAppendManualOrder			sgAppendManualOrder(true);
static AosApplyFriend				sgApplyFriend(true);
static AosBatchDelete				sgBatchDelete(true);
static AosCheckCloudId				sgCheckCloudId(true);
static AosCheckLogin				sgCheckLogin(true);
static AosComposerTN				sgComposerTN(true);
static AosConvertSqlData            sgConvertSqlData(true);
static AosCopyDocsFromCtnrProc		sgCopyDocsFromCtnr(true);
static AosCounter					sgCounter(true);
static AosCreateAccessRecord		sgCreateAccessRecord(true);
static AosCreateComment				sgCreateComment(true);
static AosCreateInbox               sgCreateInbox(true);
static AosCreateLogEntry			sgCreateLogEntry(true);
static AosCreateMsg					sgCreateMsg(true);
static AosCreateMsgContainer        sgCreateMsgContainer(true);
static AosCreateObj            		sgCreateObj(true);
static AosCreateObjByTpl            sgCreateObjByTpl(true);
static AosCreateOrder				sgCreateOrder(true);
static AosCreateSuperUser           sgCreateSuperUser(true);
static AosCreateTalkGroup			sgCreateTalkGroup(true);
static AosCreateUser                sgCreateUser(true);
static AosCreateUserDomain          sgCreateUserDomain(true);
static AosDataSync					sgDataSync(true);
static AosDeleteMsg                 sgDeleteMsg(true);
static AosDeleteMsgContainer		sgDeleteMsgContainer(true);
static AosDenyFriend 				sgDenyFriend(true);
static AosDownload					sgDownload(true);
static AosDocLockReq				sgDocLockReq(true);
static AosExportData				sgExportData(true);
static AosFinishIMSession			sgFinishIMSession(true);
static AosForum						sgForum(true);
static AosGetAccessRecord			sgGetAccessRecord(true);
static AosGetCloudId				sgGetCloudId(true);
static AosGetDomain					sgGetDomain(true);
static AosGetLanguage				sgGetLanguage(true);
static AosGetOnlineFriend			sgGetOnlineFriend(true);
static AosGetOnlineUser				sgGetOnlineUser(true);
static AosGetServerTime				sgGetServerTime(true);
static AosGetUserdocByCld           sgGetUserdocByCld(true);
static AosIncrementAttr				sgIncrementAttr(true);
static AosLogin						sgLogin(true);
static AosLoginToEsvr				sgLoginToEsvr(true);
static AosModifyAccessRecord    	sgModifyAccessRecord(true);
static AosModifyAttr				sgModifyAttr(true);
static AosModifyNode				sgModifyNode(true);
static AosModifyObjByTpl			sgModifyObjByTpl(true);
static AosMoveManualOrder			sgMoveManualOrder(true);
static AosMoveMsg					sgMoveMsg(true);
static AosNotifyFriend				sgNotifyFriend(true);
static AosOneKeyCopy				sgOneKeyCopy(true);
static AosProcOnlineStatus			sgProcOnlineStatus(true);
static AosPublishUrl				sgPublishUrl(true);
static AosRebuildLog				sgRebuildLog(true);
static AosRebuildVersion			sgRebuildVersion(true);
static AosRecentVpd					sgRecentVpd(true);
static AosRegisterHook  			sgRegisterHook(true);
static AosRemoveFollower			sgRemoveFollower(true);
static AosRemoveFriend				sgRemoveFriend(true);
static AosRemoveFriendGroup			sgRemoveFriendGroup(true);
static AosRemoveManualOrder 		sgRemoveManualOrder(true);
static AosRemoveMember				sgRemoveMember(true);
static AosRemoveMicroBlogMsg		sgRemoveMicroBlogMsg(true);
static AosRemoveTalkGroup			sgRemoveTalkGroup(true);
static AosResolveUri				sgResoldUri(true);
static AosResolveUrl				sgResoldUrl(true);
static AosRetrieveLog				sgRetrieveLog(true);
static AosRunSmartdoc				sgRunSmartdoc(true);
static AosSendInstantMsg			sgSendInstantMsg(true);
static AosSendMail					sgSendMail(true);
static AosSendMicroBlogMsg			sgSendMicroBlogMsg(true);
static AosAddSender					sgAddSender(true);
static AosRemoveSender				sgRemoveSender(true);
static AosAddFollowerToAcct			sgAddFollowerToAcct(true);
static AosRemoveFollowerFromAcct	sgRemoveFollowerFromAcct(true);
static AosSendMsgByAccount			sgSendMsgByAccount(true);
static AosSendMsgToUser				sgSendMsgToUser(true);
static AosSendShortMsg				sgSendShortMsg(true);
static AosShmConfirm				sgShmConfirm(true);
static AosSmartReply				sgSmartReply(true);	// Ketty 2011/11/03
static AosStopServer				sgStopServer(true);
static AosSwapManualOrder   		sgSwapManualOrder(true);
static AosUploadFileReq				sgUpdateFileReq(true);
static AosUploadImgReq				sgUpdateImgReq(true);
static AosUserLogOut				sgUserLogOut(true);
static AosVote						sgVote(true);
static AosCheckUnique				sgCheckUnique(true);
static AosModifyPassword			sgModifyPassword(true);
static AosIncrementInlineSearch		sgIncrementInlineSearch(true);	//ken 2011/12/08
static AosScanObjid					sgScanObjid(true); 
// static AosContecEcgProc				sgContecProcEcg(true); 
static AosCreateEcgImg				sgCreateEcgImg(true); 
static AosUpdateLoginInfo			sgUpdateLoginInfo(true); 
static AosGetServerInfo1			sgGetServerInfo(true); 
static AosRunCommand				sgRunCommand(true); 		// Chen Ding, 05/18/2012
static AosCheckIILEntry				sgCheckIILEntry(true); 		// Ken Lee, 2013/08/20
static AosRunJQL					sgRunJQL(true); 			// Andy Zhang, 2013/10/17
static AosRunIndexMgr				sgRunIndexMgr(true);		// Felicia, 2014/01/20
static AosJqlLogin					sgJqlLogin(true);			// Jozhi 2015/01/29
static AosRunSql					sgRunSql(true);				// Jozhi 2015/01/29

}



