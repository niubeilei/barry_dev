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
// 01/03/2010: Copy by Brian 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ShortMsgUtil_StringOpr_h  
#define Aos_ShortMsgUtil_StringOpr_h 
 
//定义自定义数据类型  
typedef unsigned char AosUChar_t;  
typedef unsigned char *AosUCharPtr_t;  

class AosShortMsgUtil
{
	public:
		AosShortMsgUtil();
		~AosShortMsgUtil();

		void 			ltrim(
						AosUCharPtr_t outbuff, 
						AosUCharPtr_t inbuff);
		
		void 			rtrim(
						AosUCharPtr_t outbuff,
						AosUCharPtr_t inbuff);
		
		void 			alltrim(
						AosUCharPtr_t outbuff,
						AosUCharPtr_t inbuff);
	 	
		void 			substr(
						AosUCharPtr_t outbuff,
						AosUCharPtr_t inbuff,
						int start,
						int length);
		
		int 			instr(
						int start,
						AosUCharPtr_t inbuff,
						AosUCharPtr_t strfind);
		
		void 			replace(
						AosUCharPtr_t outbuff,
						AosUCharPtr_t inbuff,
						AosUCharPtr_t strfind,
						AosUCharPtr_t strreplace);
		
		void 			str2hex(
						AosUCharPtr_t outbuff,
						AosUCharPtr_t inbuff);
		
		void 			hex2str(
						AosUCharPtr_t outbuff,
						AosUCharPtr_t inbuff);
	
		int 			strconv(
						AosUCharPtr_t outbuff,
						const AosUCharPtr_t inbuff,
						AosUCharPtr_t to_Charset,
						AosUCharPtr_t from_Charset);
		
		void 			pduToTel(
						AosUCharPtr_t pSrcCode, 
						AosUCharPtr_t pDstCode);
		
		void 			telToPdu(
						AosUCharPtr_t pSrcCode, 
						AosUCharPtr_t pDstCode,
						AosUCharPtr_t pAreaCode);
};
#endif
