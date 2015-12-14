////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_ssl.h
// Description:
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, 
//    this list of conditions and the following disclaimer in the documentation 
//    and/or other materials provided with the distribution.
// 
// 3. All manuals,brochures,user guides mentioning features or use of this software 
//    must display the following acknowledgement:
// 
//   This product includes software developed by Cavium Networks
// 
// 4. Cavium Networks' name may not be used to endorse or promote products 
//    derived from this software without specific prior written permission.
// 
// 5. User agrees to enable and utilize only the features and performance 
//    purchased on the target hardware.
// 
// This Software,including technical data,may be subject to U.S. export control 
// laws, including the U.S. Export Administration Act and its associated 
// regulations, and may be subject to export or import regulations in other 
// countries.You warrant that You will comply strictly in all respects with all 
// such regulations and acknowledge that you have the responsibility to obtain 
// licenses to export, re-export or import the Software.
// TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND 
// WITH ALL FAULTS AND CAVIUM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
// EITHER EXPRESS,IMPLIED,STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE,
// INCLUDING ITS CONDITION,ITS CONFORMITY TO ANY REPRESENTATION OR DESCRIPTION, 
// OR THE EXISTENCE OF ANY LATENT OR PATENT DEFECTS, AND CAVIUM SPECIFICALLY 
// DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES OF TITLE, MERCHANTABILITY, 
// NONINFRINGEMENT,FITNESS FOR A PARTICULAR PURPOSE,LACK OF VIRUSES, ACCURACY OR
// COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO 
// DESCRIPTION. THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE 
// SOFTWARE LIES WITH YOU.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#ifndef _CAVIUM_SSL_H_
#define _CAVIUM_SSL_H_

//#ifndef CSP1_KERNEL


/*! \enum SslVersion SSL/TLS protocol version */
typedef enum {VER_TLS = 0, VER3_0 = 1} SslVersion;

#ifdef MC2
/*! \enum Rc4Type RC4 cipher type */
typedef enum {UNSUPPORTED_RC4 = -1, RC4_128 = 8, RC4_EXPORT_40 = 9, RC4_EXPORT_56 = 11} Rc4Type;

/*! \enum DesType  (3)DES cipher type */
typedef enum {UNSUPPORTED_DES = -1, DES = 12, DES_EXPORT_40 = 13, DES3_192 = 14} DesType;
#else
/*! \enum Rc4Type RC4 cipher type */
typedef enum {UNSUPPORTED_RC4 = -1, RC4_128 = 0, RC4_EXPORT_40 = 1, RC4_EXPORT_56 = 7} Rc4Type;

/*! \enum DesType  (3)DES cipher type */
typedef enum {UNSUPPORTED_DES = -1, DES = 0, DES_EXPORT_40 = 1, DES3_192 = 2} DesType;
#endif
/*! \enum MessageType  SSL/TLS Record Type */
typedef enum {CHANGE_CIPHER_SPEC = 0, ALERT = 1, HANDSHAKE = 2, APP_DATA = 3} MessageType;

/*! \enum MasterSecretReturn  SSL/TLS Record Type */
typedef enum {NOT_RETURNED = 0, RETURN_ENCRYPTED = 1} MasterSecretReturn;

/*! \enum MasterSecretInput  Master secret location */
typedef enum {READ_FROM_CONTEXT = 0, INPUT_ENCRYPTED = 1} MasterSecretInput;

/*! \enum ClientFinishMessageOutput  predicted ClientFinished message format */
typedef enum {RETURN_CFM_ENCRYPTED = 0, RETURN_CFM_UNENCRYPTED = 1} ClientFinishMessageOutput;

/*! \enum ServerFinishMessageOutput  ServerFinished message format */
typedef enum {RETURN_SFM_ENCRYPTED = 0, RETURN_SFM_UNENCRYPTED = 1} ServerFinishMessageOutput;

/*! \enum SslPartyType server or client */
typedef enum {SSL_SERVER = 0, SSL_CLIENT = 1} SslPartyType;


/* SSLv2 specific Context Offsets */

#define OFFSET_SSL2_MASTER_SECRET			16

#define OFFSET_SSL2_3DES_SERVER_READ_SEQ		22
#define OFFSET_SSL2_3DES_SERVER_READ_KM			24
#define OFFSET_SSL2_3DES_SERVER_WRITE_SEQ		28
#define OFFSET_SSL2_3DES_SERVER_WRITE_KM		30

#define OFFSET_SSL2_RC4_SERVER_READ_SEQ			22
#define OFFSET_SSL2_RC4_SERVER_READ_KM			24
#define OFFSET_SSL2_RC4_SERVER_READ_STATE		26
#define OFFSET_SSL2_RC4_SERVER_WRITE_SEQ		60
#define OFFSET_SSL2_RC4_SERVER_WRITE_KM			62
#define OFFSET_SSL2_RC4_SERVER_WRITE_STATE		64



/*+****************************************************************************/
 /*! \ingroup MISC
 *
 * Csp1GetDmaMode
 * 
 * Returns the current DMA mode
 *
 *
 * \return DmaMode
 * \retval CAVIUM_DIRECT, CAVIUM_SCATTER_GATHER #DmaMode
 */
 /*-***************************************************************************/
DmaMode 
Csp1GetDmaMode(void);



/*+****************************************************************************/
 /*! \ingroup MISC
 *
 * Csp1GetDriverState
 * 
 * Function to check whether the driver handle is initialized or not.
 *
 *
 * \return driver_status
 * \retval 0  driver handle is ready.
 * \retval -1 driver handle is not initialized
 */
 /*-***************************************************************************/
int 
Csp1GetDriverState(void);

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1SetEncryptedMasterSecretKey
 *
 * Sets the key material for encryption of master secrets used by resume 
 * operations.
 *
 * \input 	key pointer to 48 bytes of key material
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1SetEncryptedMasterSecretKey(Uint8 *key);

#ifndef MC2

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Handshake
 *
 * Calculates the hashes needed by the SSL handshake.
 *
 * \input request_type 		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING #Csp1RequestType
 * \input context_handle	64-bit pointer to context (context_handle%8=0)
 * \input message_length 	size of input in bytes (0<=message_length<=2^16-1)
 * \input message 		pointer to length bytes of input
 *
 * \output md5_final_hash 	pointer to the 4-halfword handshake final result 
 * \output sha1_final_hash 	pointer to the 5-halfword handshake final result 
 * \output request_id 		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1Handshake(Csp1RequestType request_type,
	  Uint64 context_handle, 
	  Uint16 message_length, 
	  Uint8 *message, 
	  Uint8 *md5_final_hash, 
	  Uint8 *sha1_final_hash,
	  Uint32 *request_id);


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1HandshakeStart
 *
 * Calculates the partial hashes needed by the SSL handshake.
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING #Csp1RequestType
 * \input context_handle	64-bit pointer to context (context_handle%8=0)
 * \input message_length	size of input in bytes (0<=message_length<=2^16-1)
 * \input message		pointer to length bytes of input
 *
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1HandshakeStart(Csp1RequestType request_type,
		   Uint64 context_handle, 
		   Uint16 message_length, 
		   Uint8 *message,
		   Uint32 *request_id);


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1HandshakeUpdate
 *
 * Calculates the partial hashes needed by the SSL handshake.
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit pointer to context (context_handle%8=0)
 * \input message_length	size of input in bytes (0<=message_length<=2^16-1)
 * \input message		pointer to length bytes of input
 *
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1HandshakeUpdate(n1_request_type request_type,
		Uint64 context_handle, 
		Uint16 message_length, 
		Uint8 *message,
		Uint32 *request_id);


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1HandshakeFinish
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit pointer to context (context_handle%8=0)
 * \input message_length	size of input in bytes (0<=message_length<=2^16-1)
 * \input message		pointer to length bytes of input
 *
 * \output md5_final_hash	pointer to the 4-word handshake final result 
 * \output sha1_final_hash	pointer to the 5-word handshake final result 
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1HandshakeFinish(n1_request_type request_type,
		Uint64 context_handle, 
		Uint16 message_length, 
		Uint8 *message, 
		Uint8 *md5_final_hash, 
		Uint8 *sha1_final_hash,
		Uint32 *request_id);

#endif /* MC2*/

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerFullRc4
 *
 * Does a full handshake on the server with RSA <= 2048. This entry point 
 * handles all the RC4 cases. The handshake message data for this request 
 * should include all handshake message data after (and including) the client 
 * hello message up until (but not including) the first finished message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input rc4_type		RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \if MC2
 * \input encrypt_premaster_secret	pointer to modlength-byte value.
 * \else
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \endif
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output client_finished_message	pointer to encrypted part of client finished message 
 * \output server_finished_message	pointer to encrypted part of server finished message 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *						returned encrypted master secret : don't care
 * \output request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerFullRc4(n1_request_type request_type,
		 Uint64 context_handle, 
		 Uint64 *key_handle, 
		 HashType hash_type, 
		 SslVersion ssl_version, 
		 Rc4Type rc4_type,
		 MasterSecretReturn master_secret_ret,
		 Uint16 modlength, 
		 Uint8 *encrypt_premaster_secret, 
		 Uint8 *client_random, 
		 Uint8 *server_random, 
		 Uint16 handshake_length, 
		 Uint8 *handshake, 
		 Uint8 *client_finished_message,
		 Uint8 *server_finished_message, 
		 Uint8 *encrypt_master_secret,
		 Uint32 *request_id);

#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerFullRc4Finish
 *
 * Does a full handshake on the server with RSA <= 2048. This entry point 
 * handles all the RC4 cases. The handshake data is accumulated prior to this 
 * request by calls to Handshake*, and this request appends the 
 * included handshake message data to the pre-existing handshake hash state.
 * The handshake message data for this request (previously hashed plus included 
 * messsage data) should include all handshake message data after (and 
 * including) the client hello message up until (but not including) the first 
 * finished message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input rc4_type		RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output client_finished_message	pointer to encrypted part of client finished message 
 * \output server_finished_message	pointer to encrypted part of server finished message 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerFullRc4Finish(n1_request_type request_type,
			   Uint64 context_handle, 
			   Uint64 *key_handle, 
			   HashType hash_type, 
			   SslVersion ssl_version, 
			   Rc4Type rc4_type, 
			   MasterSecretReturn master_secret_ret,
			   Uint16 modlength, 
			   Uint8 *encrypt_premaster_secret, 
			   Uint8 *client_random, 
			   Uint8 *server_random, 
			   Uint16 handshake_length, 
			   Uint8 *handshake, 
			   Uint8 *client_finished_message,
			   Uint8 *server_finished_message, 
			   Uint8 *encrypt_master_secret,
			   Uint32 *request_id);
#endif /* MC2*/

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerVerifyRc4
 *
 * Do much of the full handshake - up to the point of the 
 * verify - in the case when client authentication is required. This is used in 
 * a full handshake on the server. This entry point handles all the RC4 cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the client verify message.  
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input rc4_type		RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output verify_data		pointer to 36 bytes of verify data 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerVerifyRc4(n1_request_type request_type,
		   Uint64 context_handle, 
		   Uint64 *key_handle, 
		   HashType hash_type, 
		   SslVersion ssl_version, 
		   Rc4Type rc4_type, 
		   MasterSecretReturn master_secret_ret,
		   Uint16 modlength, 
		   Uint8 *encrypt_premaster_secret, 
		   Uint8 *client_random, 
		   Uint8 *server_random, 
		   Uint16 handshake_length, 
		   Uint8 *handshake, 
		   Uint8 *verify_data,
		   Uint8 *encrypt_master_secret,
		   Uint32 *request_id);

#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerVerifyRc4Finish
 *
 * Do much of the full handshake - up to the point of the 
 * verify - in the case when client authentication is required. This is used in 
 * a full handshake on the server. This entry point handles all the RC4 cases.
 *
 * Handshake data can be accumulated prior to this request by calls to 
 * Handshake*, and this request will append the included handshake 
 * message data to the pre-existing handshake hash state. The handshake message 
 * data for this request (previously hashed plus included messsage data) should 
 * include all handshake message data after (and including) the client hello 
 * message up until (but not including) the client verify message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input rc4_type		RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output verify_data		pointer to 36 bytes of verify data 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerVerifyRc4Finish(n1_request_type request_type,
			 Uint64 context_handle, 
			 Uint64 *key_handle, 
			 HashType hash_type, 
			 SslVersion ssl_version, 
			 Rc4Type rc4_type, 
			 MasterSecretReturn master_secret_ret,
			 Uint16 modlength, 
			 Uint8 *encrypt_premaster_secret, 
			 Uint8 *client_random, 
			 Uint8 *server_random, 
			 Uint16 handshake_length, 
			 Uint8 *handshake, 
			 Uint8 *verify_data,
			 Uint8 *encrypt_master_secret,
			 Uint32 *request_id);

#endif

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerFull3Des
 *
 * Does a full handshake on the server with RSA <= 2048. This entry point 
 * handles all the DES cases. The handshake message data for this request 
 * should include all handshake message data after (and including) the client 
 * hello message up until (but not including) the first finished message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input des_type		DES, DES_EXPORT_40 or DES3_192
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input clnt_fin_msg_out	RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input srvr_fin_msg_out	RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input client_pad_length	number of 64-bit words to pad above min
 * \input server_pad_length	number of 64-bit words to pad above min
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \if MC2
 * \input encrypt_premaster_secret	pointer to modlength-byte value.
 * \else
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \endif
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output client_finished_message	pointer to encrypted part of client finished message 
 * \output server_finished_message	pointer to encrypted part of server finished message 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerFull3Des(n1_request_type request_type,
		  Uint64 context_handle, 
		  Uint64 *key_handle, 
		  HashType hash_type, 
		  SslVersion ssl_version, 
		  DesType des_type, 
		  MasterSecretReturn master_secret_ret,
		  ClientFinishMessageOutput clnt_fin_msg_out,
		  ServerFinishMessageOutput srvr_fin_msg_out,
		  Uint16 client_pad_length,
		  Uint16 server_pad_length,
		  Uint16 modlength, 
		  Uint8 *encrypt_premaster_secret, 
		  Uint8 *client_random, 
		  Uint8 *server_random, 
		  Uint16 handshake_length, 
		  Uint8 *handshake, 
		  Uint8 *client_finished_message,
		  Uint8 *server_finished_message, 
		  Uint8 *encrypt_master_secret,
		  Uint32 *request_id);

#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerFull3DesFinish
 *
 * Does a full handshake on the server. This entry point 
 * handles all the DES cases. The handshake data is accumulated prior to this 
 * request by calls to Handshake*, and this request appends the 
 * included handshake message data to the pre-existing handshake hash state.
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until 
 * (but not including) the first finished message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input des_type		DES, DES_EXPORT_40 or DES3_192
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input clnt_fin_msg_out	RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input srvr_fin_msg_out	RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input client_pad_length	number of 64-bit words to pad above min
 * \input server_pad_length	number of 64-bit words to pad above min
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \if MC2
 * \input encrypt_premaster_secret	pointer to modlength-byte value.
 * \else
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \endif
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output client_finished_message	pointer to encrypted part of client finished message 
 * \output server_finished_message	pointer to encrypted part of server finished message 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id	Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerFull3DesFinish(n1_request_type request_type,
			Uint64 context_handle, 
	            	Uint64 *key_handle, 
			HashType hash_type, 
			SslVersion ssl_version, 
			DesType des_type, 
			MasterSecretReturn master_secret_ret,
			ClientFinishMessageOutput clnt_fin_msg_out,
			ServerFinishMessageOutput srvr_fin_msg_out,
			Uint16 client_pad_length,
			Uint16 server_pad_length,
			Uint16 modlength, 
			Uint8 *encrypt_premaster_secret, 
			Uint8 *client_random, 
			Uint8 *server_random, 
			Uint16 handshake_length, 
			Uint8 *handshake, 
			Uint8 *client_finished_message,
			Uint8 *server_finished_message, 
			Uint8 *encrypt_master_secret,
			Uint32 *request_id);
#endif

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerVerify3Des
 *
 * Do much of the full handshake - up to the point of the 
 * verify - in the case when client authentication is required. This is used in 
 * a full handshake on the server. This entry point handles all the DES/3DES 
 * cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the client verify message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input des_type		DES, DES_EXPORT_40, DES3_192
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \if MC2
 * \input encrypt_premaster_secret	pointer to modlength-byte value
 * \else
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \endif
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output verify_data		pointer to 36 bytes of verify data 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerVerify3Des(n1_request_type request_type,
			Uint64 context_handle, 
			Uint64 *key_handle, 
			HashType hash_type, 
			SslVersion ssl_version, 
			DesType des_type, 
			MasterSecretReturn master_secret_ret,
			Uint16 modlength, 
			Uint8 *encrypt_premaster_secret, 
			Uint8 *client_random, 
			Uint8 *server_random, 
			Uint16 handshake_length, 
			Uint8 *handshake, 
			Uint8 *verify_data,
			Uint8 *encrypt_master_secret,
			Uint32 *request_id);

#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerVerify3DesFinish
 *
 * Do much of the full handshake - up to the point of the 
 * verify - in the case when client authentication is required. This is used in 
 * a full handshake on the server. This entry point handles all the DES/3DES 
 * cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but not
 * including) the client verify message. Handshake data can be accumulated prior 
 * to this request by calls to Handshake*, and this request will append 
 * the included handshake message data to the pre-existing handshake hash state.
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input des_type		DES, DES_EXPORT_40 or DES3_192
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input modlength	size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output verify_data		pointer to 36 bytes of verify data 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerVerify3DesFinish(n1_request_type request_type,
			  Uint64 context_handle, 
			  Uint64 *key_handle, 
			  HashType hash_type, 
			  SslVersion ssl_version, 
			  DesType des_type, 
			  MasterSecretReturn master_secret_ret,
			  Uint16 modlength, 
			  Uint8 *encrypt_premaster_secret, 
			  Uint8 *client_random, 
			  Uint8 *server_random, 
			  Uint16 handshake_length, 
			  Uint8 *handshake, 
			  Uint8 *verify_data,
			  Uint8 *encrypt_master_secret,
			  Uint32 *request_id);

#endif

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerFullAes
 *
 * Does a full handshake on the server. This entry point 
 * handles all the AES cases. The handshake message data for this request 
 * should include all handshake message data after (and including) the client 
 * hello message up until (but not including) the first finished message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input aes_type		AES_128 or AES_256
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input clnt_fin_msg_out	RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input srvr_fin_msg_out	RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input client_pad_length	number of 128-bit words to pad above min
 * \input server_pad_length	number of 128-bit words to pad above min
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \if MC2
 * \input encrypt_premaster_secret	pointer to modlength-byte value.
 * \else
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \endif
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output client_finished_message	pointer to encrypted part of client finished message 
 * \output server_finished_message	pointer to encrypted part of server finished message 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerFullAes(n1_request_type request_type,
		 Uint64 context_handle, 
		 Uint64 *key_handle, 
		 HashType hash_type, 
		 SslVersion ssl_version, 
		 AesType aes_type, 
		 MasterSecretReturn master_secret_ret,
		 ClientFinishMessageOutput clnt_fin_msg_out,
		 ServerFinishMessageOutput srvr_fin_msg_out,
		 Uint16 client_pad_length,
		 Uint16 server_pad_length,
		 Uint16 modlength, 
		 Uint8 *encrypt_premaster_secret, 
		 Uint8 *client_random, 
		 Uint8 *server_random, 
		 Uint16 handshake_length, 
		 Uint8 *handshake, 
		 Uint8 *client_finished_message,
		 Uint8 *server_finished_message, 
		 Uint8 *encrypt_master_secret,
		 Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerFullAesFinish
 *
 * Does a full handshake on the server. This entry point 
 * handles all the aes cases. The handshake data is accumulated prior to this 
 * request by calls to Handshake*, and this request appends the 
 * included handshake message data to the pre-existing handshake hash state.
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until 
 * (but not including) the first finished message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input aes_type		AES_128 or AES_256
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input clnt_fin_msg_out	RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input srvr_fin_msg_out	RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input client_pad_length	number of 128-bit words to pad above min
 * \input server_pad_length	number of 128-bit words to pad above min
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output client_finished_message	pointer to encrypted part of client finished message 
 * \output server_finished_message	pointer to encrypted part of server finished message 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id	Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerFullAesFinish(n1_request_type request_type,
			   Uint64 context_handle, 
			   Uint64 *key_handle, 
			   HashType hash_type, 
			   SslVersion ssl_version, 
			   AesType aes_type, 
			   MasterSecretReturn master_secret_ret,
			   ClientFinishMessageOutput clnt_fin_msg_out,
			   ServerFinishMessageOutput srvr_fin_msg_out,
			   Uint16 client_pad_length,
			   Uint16 server_pad_length,
			   Uint16 modlength, 
			   Uint8 *encrypt_premaster_secret, 
			   Uint8 *client_random, 
			   Uint8 *server_random, 
			   Uint16 handshake_length, 
			   Uint8 *handshake, 
			   Uint8 *client_finished_message,
			   Uint8 *server_finished_message, 
			   Uint8 *encrypt_master_secret,
			   Uint32 *request_id);
#endif

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerVerifyAes
 *
 * Do much of the full handshake - up to the point of the 
 * verify - in the case when client authentication is required. This is used in 
 * a full handshake on the server. This entry point handles all the AES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the client verify message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input aes_type		AES_128 or AES_256
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \if MC2
 * \input encrypt_premaster_secret	pointer to modlength-byte value.
 * \else
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \endif
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output verify_data		pointer to 36 bytes of verify data 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerVerifyAes(n1_request_type request_type,
		   Uint64 context_handle, 
		   Uint64 *key_handle, 
		   HashType hash_type, 
		   SslVersion ssl_version, 
		   AesType aes_type, 
		   MasterSecretReturn master_secret_ret,
		   Uint16 modlength, 
		   Uint8 *encrypt_premaster_secret, 
		   Uint8 *client_random, 
		   Uint8 *server_random, 
		   Uint16 handshake_length, 
		   Uint8 *handshake, 
		   Uint8 *verify_data,
		   Uint8 *encrypt_master_secret,
		   Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaServerVerifyAesFinish
 *
 * Do much of the full handshake - up to the point of the 
 * verify - in the case when client authentication is required. This is used in 
 * a full handshake on the server. This entry point handles all the AES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the client verify message. Handshake data can be accumulated 
 * prior to this request by calls to Handshake*, and this request will 
 * append the included handshake message data to the pre-existing handshake 
 * hash state.
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit byte-pointer to context (context_handle%8=0)
 * \input key_handle		pointer to 64-bit key memory handle
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input aes_type		AES_128 or AES_256
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input modlength		size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 * \input encrypt_premaster_secret	pointer to modlength-byte value in integer format
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output verify_data		pointer to 36 bytes of verify data 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1RsaServerVerifyAesFinish(n1_request_type request_type,
			 Uint64 context_handle, 
			 Uint64 *key_handle, 
			 HashType hash_type, 
			 SslVersion ssl_version, 
			 AesType aes_type, 
			 MasterSecretReturn master_secret_ret,
			 Uint16 modlength, 
			 Uint8 *encrypt_premaster_secret, 
			 Uint8 *client_random, 
			 Uint8 *server_random, 
			 Uint16 handshake_length, 
			 Uint8 *handshake, 
			 Uint8 *verify_data,
			 Uint8 *encrypt_master_secret,
			 Uint32 *request_id);
#endif


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherFullRc4
 *
 * Do a full handshake without RSA operation. The pre-master secret is read
 * from the context and the rest of the handshake is completed. This is used
 * by both the server and the client. This entry point handles all the RC4
 * cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the first finished message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit pointer to context (context_handle%8=0)
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input rc4_type		RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input pre_master_length	size of premaster secret in bytes 
 *				(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 		TLS: 16<=modlength<=128, modlength%16=0)
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output client_finished_message	pointer to encrypted part of client finished message 
 * \output server_finished_message	pointer to encrypted part of server finished message 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherFullRc4(n1_request_type request_type,
		 Uint64 context_handle, 
		 HashType hash_type, 
		 SslVersion ssl_version, 
		 Rc4Type rc4_type, 
		 MasterSecretReturn master_secret_ret,
		 Uint16 pre_master_length, 
		 Uint8 *client_random, 
		 Uint8 *server_random, 
		 Uint16 handshake_length, 
		 Uint8 *handshake, 
		 Uint8 *client_finished_message,
		 Uint8 *server_finished_message, 
		 Uint8 *encrypt_master_secret,
		 Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherFullRc4Finish
 *
 * Do a full handshake without RSA operation. The pre-master secret is read
 * from the context and the rest of the handshake is completed. This is used
 * by both the server and the client. This entry point handles all the rc4
 * cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the first finished message. Handshake data can be accumulated 
 * prior to this request by calls to Handshake*, and this request will 
 * append the included handshake message data to the pre-existing handshake 
 * hash state.
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit pointer to context (context_handle%8=0)
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input rc4_type		RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input pre_master_length	size of premaster secret in bytes 
 *				(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 		TLS: 16<=modlength<=128, modlength%16=0)
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output client_finished_message	pointer to encrypted part of client finished message 
 * \output server_finished_message	pointer to encrypted part of server finished message 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherFullRc4Finish(n1_request_type request_type,
		   Uint64 context_handle, 
		   HashType hash_type, 
		   SslVersion ssl_version, 
		   Rc4Type rc4_type, 
		   MasterSecretReturn master_secret_ret,
		   Uint16 pre_master_length, 
		   Uint8 *client_random, 
		   Uint8 *server_random, 
		   Uint16 handshake_length, 
		   Uint8 *handshake, 
		   Uint8 *client_finished_message,
		   Uint8 *server_finished_message, 
		   Uint8 *encrypt_master_secret,
		   Uint32 *request_id);

#endif

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherVerifyRc4
 *
 * Do a full handshake - up to the point of the
 * verify operation. The pre-master secret is read from the context.
 * This is used by both the server and the client. This entry point
 * handles all the RC4 cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the client verify message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit pointer to context (context_handle%8=0)
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input rc4_type		RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input pre_master_length	size of premaster secret in bytes 
 *				(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 		TLS: 16<=modlength<=128, modlength%16=0)
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output verify_data		pointer to 36 bytes of verify data 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherVerifyRc4(n1_request_type request_type,
		  Uint64 context_handle, 
		  HashType hash_type, 
		  SslVersion ssl_version, 
		  Rc4Type rc4_type, 
		  MasterSecretReturn master_secret_ret,
		  Uint16 pre_master_length, 
		  Uint8 *client_random, 
		  Uint8 *server_random, 
		  Uint16 handshake_length, 
		  Uint8 *handshake, 
		  Uint8 *verify_data,
		  Uint8 *encrypt_master_secret,
		  Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherVerifyRc4Finish
 *
 * Do a full handshake - up to the point of the
 * verify operation. The pre-master secret is read from the context.
 * This is used by both the server and the client. This entry point
 * handles all the rc4 cases.
 *
 * The handshake message data for this request should include all handshake message
 * data after (and including) the client hello message up until (but not
 * including) the client verify message. Handshake data can be accumulated prior 
 * to this request by calls to Handshake*, and this request will append 
 * the included handshake message data to the pre-existing handshake hash state.
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit pointer to context (context_handle%8=0)
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input rc4_type		RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input pre_master_length	size of premaster secret in bytes 
 *				(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 		TLS: 16<=modlength<=128, modlength%16=0)
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output verify_data		pointer to 36 bytes of verify data 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherVerifyRc4Finish(n1_request_type request_type,
			 Uint64 context_handle, 
			 HashType hash_type, 
			 SslVersion ssl_version, 
			 Rc4Type rc4_type, 
			 MasterSecretReturn master_secret_ret,
			 Uint16 pre_master_length, 
			 Uint8 *client_random, 
			 Uint8 *server_random, 
			 Uint16 handshake_length, 
			 Uint8 *handshake, 
			 Uint8 *verify_data,
			 Uint8 *encrypt_master_secret,
			 Uint32 *request_id);
#endif


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherFull3Des
 *
 * Do a full handshake. The pre-master secret is read
 * from the context and the rest of the handshake is completed. This is used
 * by both the server and the client. This entry point handles all the DES/
 * 3DES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the first finished message. 
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit pointer to context (context_handle%8=0)
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input des_type		DES, DES_EXPORT_40 or DES3_192
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input clnt_fin_msg_out	RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input srvr_fin_msg_out	RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input client_pad_length	number of 64-bit words to pad above min
 * \input server_pad_length	number of 64-bit words to pad above min
 * \input pre_master_length	size of premaster secret in bytes 
 *	 			(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *	  			TLS: 16<=modlength<=128, modlength%16=0)
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output client_finished_message	pointer to encrypted part of client finished message 
 * \output server_finished_message	pointer to encrypted part of server finished message 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *					returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherFull3Des(n1_request_type request_type,
		  Uint64 context_handle, 
	          HashType hash_type, 
	          SslVersion ssl_version, 
		  DesType des_type, 
		  MasterSecretReturn master_secret_ret,
		  ClientFinishMessageOutput clnt_fin_msg_out,
		  ServerFinishMessageOutput srvr_fin_msg_out,
		  Uint16 client_pad_length,
		  Uint16 server_pad_length,
		  Uint16 pre_master_length, 
		  Uint8 *client_random, 
		  Uint8 *server_random, 
		  Uint16 handshake_length, 
		  Uint8 *handshake, 
		  Uint8 *client_finished_message,
		  Uint8 *server_finished_message, 
		  Uint8 *encrypt_master_secret,
		  Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherFull3DesFinish
 *
 * Do a full handshake. The pre-master secret is read
 * from the context and the rest of the handshake is completed. This is used
 * by both the server and the client. This entry point handles all the DES/
 * 3DES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the first finished message. Handshake data can be accumulated 
 * prior to this request by calls to Handshake*, and this request will 
 * append the included handshake message data to the pre-existing handshake 
 * hash state.
 *
 * \input request_type		CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle	64-bit pointer to context (context_handle%8=0)
 * \input hash_type		MD5_TYPE or SHA1_TYPE
 * \input ssl_version		VER3_0 or VER_TLS
 * \input des_type		DES, DES_EXPORT_40 or DES3_192
 * \input master_secret_ret	NOT_RETURNED or RETURN_ENCRYPTED
 * \input clnt_fin_msg_out	RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input srvr_fin_msg_out	RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input client_pad_length	number of 64-bit words to pad above min
 * \input server_pad_length	number of 64-bit words to pad above min
 * \input pre_master_length	size of premaster secret in bytes 
 *				(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 		TLS: 16<=modlength<=128, modlength%16=0)
 * \input client_random		pointer to 32 bytes of random data
 * \input server_random		pointer to 32 bytes of random data
 * \input handshake_length	size in bytes of the handshake message data
 * \input handshake		pointer to the handshake message data
 *
 * \output client_finished_message	pointer to encrypted part of client finished message 
 * \output server_finished_message	pointer to encrypted part of server finished message 
 * \output encrypt_master_secret	(master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *				returned encrypted master secret : don't care
 * \output request_id		Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherFull3DesFinish(n1_request_type request_type,
			Uint64 context_handle, 
			HashType hash_type, 
			SslVersion ssl_version, 
			DesType des_type, 
			MasterSecretReturn master_secret_ret,
			ClientFinishMessageOutput clnt_fin_msg_out,
			ServerFinishMessageOutput srvr_fin_msg_out,
			Uint16 client_pad_length,
			Uint16 server_pad_length,
			Uint16 pre_master_length, 
			Uint8 *client_random, 
			Uint8 *server_random, 
			Uint16 handshake_length, 
			Uint8 *handshake, 
			Uint8 *client_finished_message,
			Uint8 *server_finished_message, 
			Uint8 *encrypt_master_secret,
			Uint32 *request_id);
#endif


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherVerify3Des
 *
 * do a full handshake - up to the point of the
 * verify operation. The pre-master secret is read from the context.
 * This is used by both the server and the client. This entry point handles all 
 * the DES/3DES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the client verify message. 
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	des_type = DES, DES_EXPORT_40, DES3_192
 * \input	master_secret_ret = NOT_RETURNED or RETURN_ENCRYPTED
 * \input	pre_master_length = size of premaster secret in bytes 
 *		(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 TLS: 16<=modlength<=128, modlength%16=0)
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	verify_data = pointer to 36 bytes of verify data 
 * \output	encrypt_master_secret = (master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *		returned encrypted master secret : don't care
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherVerify3Des(n1_request_type request_type,
					Uint64 context_handle, 
			        HashType hash_type, 
			        SslVersion ssl_version, 
			        DesType des_type, 
				    MasterSecretReturn master_secret_ret,
			        Uint16 pre_master_length, 
			        Uint8 *client_random, 
			        Uint8 *server_random, 
			        Uint16 handshake_length, 
			        Uint8 *handshake, 
			        Uint8 *verify_data,
					Uint8 *encrypt_master_secret,
					Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherVerify3DesFinish
 *
 * do a full handshake - up to the point of the
 * verify operation. The pre-master secret is read from the context.
 * This is used by both the server and the client. This entry point handles all 
 * the DES/3DES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the client verify message. Handshake data can be accumulated 
 * prior to this request by calls to Handshake*, and this request will 
 * append the included handshake message data to the pre-existing handshake 
 * hash state.
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	des_type = DES, DES_EXPORT_40 or DES3_192
 * \input	master_secret_ret = NOT_RETURNED or RETURN_ENCRYPTED
 * \input	pre_master_length = size of premaster secret in bytes 
 *		(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 TLS: 16<=modlength<=128, modlength%16=0)
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	verify_data = pointer to 36 bytes of verify data 
 * \output	encrypt_master_secret = (master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *		returned encrypted master secret : don't care
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherVerify3DesFinish(n1_request_type request_type,
						  Uint64 context_handle, 
					      HashType hash_type, 
					      SslVersion ssl_version, 
					      DesType des_type, 
					      MasterSecretReturn master_secret_ret,
					      Uint16 pre_master_length, 
					      Uint8 *client_random, 
					      Uint8 *server_random, 
					      Uint16 handshake_length, 
					      Uint8 *handshake, 
					      Uint8 *verify_data,
					      Uint8 *encrypt_master_secret,
						  Uint32 *request_id);
#endif


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherFullAes
 *
 * Do a full handshake. The pre-master secret is read
 * from the context and the rest of the handshake is completed. This is used
 * by both the server and the client. This entry point handles all the
 * AES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the first finished message. 
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	aes_type = AES_128 or AES_256
 * \input	master_secret_ret = NOT_RETURNED or RETURN_ENCRYPTED
 * \input	clnt_fin_msg_out = RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input	srvr_fin_msg_out = RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input	client_pad_length = number of 128-bit words to pad above min
 * \input	server_pad_length = number of 128-bit words to pad above min
 * \input	pre_master_length = size of premaster secret in bytes 
 *		(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 TLS: 16<=modlength<=128, modlength%16=0)
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	encrypt_master_secret = (master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *		returned encrypted master secret : don't care
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherFullAes(n1_request_type request_type,
				 Uint64 context_handle, 
		    	 HashType hash_type, 
			     SslVersion ssl_version, 
			     AesType aes_type, 
			     MasterSecretReturn master_secret_ret,
				 ClientFinishMessageOutput clnt_fin_msg_out,
				 ServerFinishMessageOutput srvr_fin_msg_out,
			     Uint16 client_pad_length,
			     Uint16 server_pad_length,
			     Uint16 pre_master_length, 
			     Uint8 *client_random, 
			     Uint8 *server_random, 
			     Uint16 handshake_length, 
			     Uint8 *handshake, 
			     Uint8 *client_finished_message,
			     Uint8 *server_finished_message, 
				 Uint8 *encrypt_master_secret,
				 Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherFullAesFinish
 *
 * When not (RSA <= 2048), do a full handshake. The pre-master secret is read
 * from the context and the rest of the handshake is completed. This is used
 * by both the server and the client. This entry point handles all the
 * AES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the first finished message. Handshake data can be accumulated 
 * prior to this request by calls to Handshake*, and this request will 
 * append the included handshake message data to the pre-existing handshake 
 * hash state.
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	aes_type = AES_128 or AES_256
 * \input	master_secret_ret = NOT_RETURNED or RETURN_ENCRYPTED
 * \input	clnt_fin_msg_out = RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input	srvr_fin_msg_out = RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input	client_pad_length = number of 128-bit words to pad above min
 * \input	server_pad_length = number of 128-bit words to pad above min
 * \input	pre_master_length = size of premaster secret in bytes 
 *		(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 TLS: 16<=modlength<=128, modlength%16=0)
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	encrypt_master_secret = (master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *		returned encrypted master secret : don't care
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherFullAesFinish(n1_request_type request_type,
					   Uint64 context_handle, 
				       HashType hash_type, 
				       SslVersion ssl_version, 
				       AesType aes_type, 
				       MasterSecretReturn master_secret_ret,
					   ClientFinishMessageOutput clnt_fin_msg_out,
					   ServerFinishMessageOutput srvr_fin_msg_out,
				       Uint16 client_pad_length,
				       Uint16 server_pad_length,
				       Uint16 pre_master_length, 
				       Uint8 *client_random, 
				       Uint8 *server_random, 
				       Uint16 handshake_length, 
				       Uint8 *handshake, 
				       Uint8 *client_finished_message,
				       Uint8 *server_finished_message, 
					   Uint8 *encrypt_master_secret,
					   Uint32 *request_id);
#endif


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherVerifyAes
 *
 * Do a full handshake - up to the point of the
 * verify operation. The pre-master secret is read from the context.
 * This is used by both the server and the client. This entry point handles all 
 * the AES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the client verify message. 
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	aes_type = AES_128 or AES_256
 * \input	master_secret_ret = NOT_RETURNED or RETURN_ENCRYPTED
 * \input	pre_master_length = size of premaster secret in bytes 
 *		(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 TLS: 16<=modlength<=128, modlength%16=0)
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	verify_data = pointer to 36 bytes of verify data 
 * \output	encrypt_master_secret = (master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *		returned encrypted master secret : don't care
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherVerifyAes(n1_request_type request_type,
				   Uint64 context_handle, 
			       HashType hash_type, 
			       SslVersion ssl_version, 
			       AesType aes_type, 
			       MasterSecretReturn master_secret_ret,
			       Uint16 pre_master_length, 
			       Uint8 *client_random, 
			       Uint8 *server_random, 
			       Uint16 handshake_length, 
			       Uint8 *handshake, 
			       Uint8 *verify_data,
				   Uint8 *encrypt_master_secret,
				   Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1OtherVerifyAesFinish
 *
 * do a full handshake - up to the point of the
 * verify operation. The pre-master secret is read from the context.
 * This is used by both the server and the client. This entry point handles all 
 * the AES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the client hello message up until (but 
 * not including) the client verify message. Handshake data can be accumulated 
 * prior to this request by calls to Handshake*, and this request will 
 * append the included handshake message data to the pre-existing handshake 
 * hash state.
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	aes_type = AES_128 or AES_256
 * \input	master_secret_ret = NOT_RETURNED or RETURN_ENCRYPTED
 * \input	pre_master_length = size of premaster secret in bytes 
 *		(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 TLS: 16<=modlength<=128, modlength%16=0)
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	verify_data = pointer to 36 bytes of verify data 
 * \output	encrypt_master_secret = (master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *		returned encrypted master secret : don't care
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1OtherVerifyAesFinish(n1_request_type request_type,
						 Uint64 context_handle, 
					     HashType hash_type, 
				    	 SslVersion ssl_version, 
					     AesType aes_type, 
					     MasterSecretReturn master_secret_ret,
					     Uint16 pre_master_length, 
					     Uint8 *client_random, 
					     Uint8 *server_random, 
					     Uint16 handshake_length, 
				    	 Uint8 *handshake, 
				    	 Uint8 *verify_data,
					     Uint8 *encrypt_master_secret,
						 Uint32 *request_id);

#endif

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1FinishedRc4Finish
 *
 * Finish off the handshake hash and generate the finished messages for a full 
 * handshake. This is used in a full handshake with client authentication on 
 * either the client or the server. This follows RsaserverVerify or 
 * OtherVerify in that case. This entry point handles all the RC4 cases.
 *
 * The handshake hash context should include handshake messages from the
 * client hello message up until (but not including) the client verify message. 
 * This state should have been set up by a prior RsaserverVerifyRc4* or 
 * OtherVerifyRc4*. The handshake message data for this request should include 
 * the certificate verify message.
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1FinishedRc4Finish(n1_request_type request_type,
					  Uint64 context_handle, 
				      HashType hash_type, 
				      SslVersion ssl_version, 
				      Uint16 handshake_length, 
				      Uint8 *handshake, 
				      Uint8 *client_finished_message, 
				      Uint8 *server_finished_message,
					  Uint32 *request_id); 


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Finished3DesFinish
 *
 * Finish off the handshake hash and generate the finished messages for a full 
 * handshake. This is used in a full handshake with client authentication on 
 * either the client or the server. This follows RsaserverVerify or 
 * OtherVerify in that case. This entry point handles all DES/3DES 
 * cases.
 *
 * The handshake hash context should include handshake messages from the
 * client hello message up until (but not including) the client verify message. 
 * This state should have been set up by a prior RsaserverVerify3Des* or 
 * OtherVerify3Des*. The handshake message data for this request should include 
 * the certificate verify message.
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	clnt_fin_msg_out = RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input	srvr_fin_msg_out = RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input	client_pad_length = number of 64-bit words to pad above min
 * \input	server_pad_length = number of 64-bit words to pad above min
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1Finished3DesFinish(n1_request_type request_type,
					   Uint64 context_handle, 
				       HashType hash_type, 
				       SslVersion ssl_version, 
					   ClientFinishMessageOutput clnt_fin_msg_out,
					   ServerFinishMessageOutput srvr_fin_msg_out,
				       Uint16 client_pad_length,
				       Uint16 server_pad_length,
				       Uint16 handshake_length, 
				       Uint8 *handshake, 
				       Uint8 *client_finished_message, 
				       Uint8 *server_finished_message,
					   Uint32 *request_id); 


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1FinishedAesFinish
 *
 * Finish off the handshake hash and generate the finished messages for a full 
 * handshake. This is used in a full handshake with client authentication on 
 * either the client or the server. This follows RsaserverVerify or 
 * OtherVerify in that case. This entry point handles all the AES cases.
 *
 * The handshake hash context should include handshake messages from the
 * client hello message up until (but not including) the client verify message. 
 * This state should have been set up by a prior RsaserverVerifyAes* or 
 * OtherVerifyAes*. The handshake message data for this request should include 
 * the certificate verify message.
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	aes_type = AES_128 or AES_256
 * \input	clnt_fin_msg_out = RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input	srvr_fin_msg_out = RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input	client_pad_length = number of 128-bit words to pad above min
 * \input	server_pad_length = number of 128-bit words to pad above min
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1FinishedAesFinish(n1_request_type request_type,
					  Uint64 context_handle, 
				      HashType hash_type, 
				      SslVersion ssl_version, 
				      AesType aes_type, 
					  ClientFinishMessageOutput clnt_fin_msg_out,
					  ServerFinishMessageOutput srvr_fin_msg_out,
				      Uint16 client_pad_length,
				      Uint16 server_pad_length,
				      Uint16 handshake_length, 
				      Uint8 *handshake, 
				      Uint8 *client_finished_message, 
				      Uint8 *server_finished_message,
					  Uint32 *request_id);


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1ResumeRc4
 *
 * Completes a resume on either the client or the server. This entry point 
 * handles all the RC4 cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the most-recent client hello message up 
 * until (but not including) the first finished message. 
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	rc4_type = RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 * \input	master_secret_inp = READ_FROM_CONTEXT or INPUT_ENCRYPTED
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	encrypt_master_secret = pointer to 48-byte secret
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1ResumeRc4(n1_request_type request_type,
			  Uint64 context_handle, 
		      HashType hash_type, 
		      SslVersion ssl_version, 
		      Rc4Type rc4_type,
		      MasterSecretInput master_secret_inp,
		      Uint8 *client_random, 
		      Uint8 *server_random,
		      Uint8 *encrypt_master_secret,
		      Uint16 handshake_length, 
		      Uint8 *handshake, 
		      Uint8 *client_finished_message, 
		      Uint8 *server_finished_message,
			  Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1ResumeRc4Finish
 *
 * Completes a resume on either the client or the server. This entry point 
 * handles all the RC4 cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the most-recent client hello message up 
 * until (but not including) the first finished message. Handshake data can be 
 * accumulated prior to this request by calls to Handshake*, and this 
 * request will append the included handshake message data to the pre-existing
 * handshake hash state.
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	rc4_type = RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 * \input	master_secret_inp = READ_FROM_CONTEXT or INPUT_ENCRYPTED
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	encrypt_master_secret = pointer to 48-byte secret
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1ResumeRc4Finish(n1_request_type request_type,
					Uint64 context_handle, 
			    	HashType hash_type, 
			    	SslVersion ssl_version, 
			    	Rc4Type rc4_type, 
			    	MasterSecretInput master_secret_inp,
			    	Uint8 *client_random, 
			    	Uint8 *server_random, 
			    	Uint8 *encrypt_master_secret,
			    	Uint16 handshake_length, 
			    	Uint8 *handshake, 
			    	Uint8 *client_finished_message, 
			    	Uint8 *server_finished_message,
					Uint32 *request_id); 
#endif



/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Resume3Des
 *
 * Completes a resume on either the client or the server. This entry point 
 * handles all the DES/3DES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the most-recent client hello message up 
 * until (but not including) the first finished message. 
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	des_type = DES, DES_EXPORT_40 or DES3_192
 * \input	master_secret_inp = READ_FROM_CONTEXT or INPUT_ENCRYPTED
 * \input	clnt_fin_msg_out = RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input	srvr_fin_msg_out = RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input	client_pad_length = number of 64-bit words to pad above min(not applicable to MC2)
 * \input	server_pad_length = number of 64-bit words to pad above min(not applicable to MC2)
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	encrypt_master_secret = pointer to 48-byte secret
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1Resume3Des(n1_request_type request_type,
			   Uint64 context_handle, 
		       HashType hash_type, 
		       SslVersion ssl_version, 
		       DesType des_type, 
		       MasterSecretInput master_secret_inp,
			   ClientFinishMessageOutput clnt_fin_msg_out,
			   ServerFinishMessageOutput srvr_fin_msg_out,
		       Uint16 client_pad_length,
		       Uint16 server_pad_length,
		       Uint8 *client_random, 
		       Uint8 *server_random, 
		       Uint8 *encrypt_master_secret,
		       Uint16 handshake_length, 
	   	       Uint8 *handshake, 
		       Uint8 *client_finished_message, 
		       Uint8 *server_finished_message,
			   Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Resume3DesFinish
 *
 * Completes a resume on either the client or the server. This entry point 
 * handles all the DES/3DES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the most-recent client hello message up 
 * until (but not including) the first finished message. Handshake data can be 
 * accumulated prior to this request by calls to Handshake*, and this 
 * request will append the included handshake message data to the pre-existing
 * handshake hash state.
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	des_type = DES, DES_EXPORT_40 or DES3_192
 * \input	master_secret_inp = READ_FROM_CONTEXT or INPUT_ENCRYPTED
 * \input	clnt_fin_msg_out = RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input	srvr_fin_msg_out = RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input	client_pad_length = number of 64-bit words to pad above min
 * \input	server_pad_length = number of 64-bit words to pad above min
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	encrypt_master_secret = pointer to 48-byte secret
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1Resume3DesFinish(n1_request_type request_type,
					 Uint64 context_handle, 
			    	 HashType hash_type, 
			    	 SslVersion ssl_version, 
			    	 DesType des_type, 
			    	 MasterSecretInput master_secret_inp,
					 ClientFinishMessageOutput clnt_fin_msg_out,
					 ServerFinishMessageOutput srvr_fin_msg_out,
			    	 Uint16 client_pad_length,
			    	 Uint16 server_pad_length,
			    	 Uint8 *client_random, 
			    	 Uint8 *server_random, 
			    	 Uint8 *encrypt_master_secret,
			    	 Uint16 handshake_length, 
			    	 Uint8 *handshake, 
			    	 Uint8 *client_finished_message, 
			    	 Uint8 *server_finished_message,
					 Uint32 *request_id);
#endif


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1ResumeAes
 *
 * Completes a resume on either the client or the server. This entry point 
 * handles all the AES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the most-recent client hello message up 
 * until (but not including) the first finished message. 
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	aes_type = AES_128 or AES_256
 * \input	master_secret_inp = READ_FROM_CONTEXT or INPUT_ENCRYPTED
 * \input	clnt_fin_msg_out = RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input	srvr_fin_msg_out = RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input	client_pad_length = number of 128-bit words to pad above min(not applicable to MC2)
 * \input	server_pad_length = number of 128-bit words to pad above min(not applicable to MC2)
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	encrypt_master_secret = pointer to 48-byte secret
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1ResumeAes(n1_request_type request_type,
			  Uint64 context_handle, 
	    	  HashType hash_type, 
	    	  SslVersion ssl_version, 
	    	  AesType aes_type, 
		      MasterSecretInput master_secret_inp,
			  ClientFinishMessageOutput clnt_fin_msg_out,
			  ServerFinishMessageOutput srvr_fin_msg_out,
		      Uint16 client_pad_length,
		      Uint16 server_pad_length,
		      Uint8 *client_random, 
	    	  Uint8 *server_random, 
		      Uint8 *encrypt_master_secret,
		      Uint16 handshake_length, 
		      Uint8 *handshake, 
		      Uint8 *client_finished_message, 
		      Uint8 *server_finished_message,
			  Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1ResumeAesFinish
 *
 * Completes a resume on either the client or the server. This entry point 
 * handles all the AES cases.
 *
 * The handshake message data for this request should include all handshake 
 * message data after (and including) the most-recent client hello message up 
 * until (but not including) the first finished message. Handshake data can be 
 * accumulated prior to this request by calls to Handshake*, and this 
 * request will append the included handshake message data to the pre-existing
 * handshake hash state.
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	aes_type = AES_128 or AES_256
 * \input	master_secret_inp = READ_FROM_CONTEXT or INPUT_ENCRYPTED
 * \input	clnt_fin_msg_out = RETURN_CFM_ENCRYPTED or RETURN_CFM_UNENCRYPTED
 * \input	srvr_fin_msg_out = RETURN_SFM_ENCRYPTED or RETURN_SFM_UNENCRYPTED
 * \input	client_pad_length = number of 128-bit words to pad above min
 * \input	server_pad_length = number of 128-bit words to pad above min
 * \input	client_random = pointer to 32 bytes of random data
 * \input	server_random = pointer to 32 bytes of random data
 * \input	encrypt_master_secret = pointer to 48-byte secret
 * \input	handshake_length = size in bytes of the handshake message data
 * \input	handshake = pointer to the handshake message data
 *
 * \output	client_finished_message = pointer to encrypted part of client finished message 
 * \output	server_finished_message = pointer to encrypted part of server finished message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1ResumeAesFinish(n1_request_type request_type,
					Uint64 context_handle, 
			    	HashType hash_type, 
			    	SslVersion ssl_version, 
			    	AesType aes_type, 
			    	MasterSecretInput master_secret_inp,
					ClientFinishMessageOutput clnt_fin_msg_out,
					ServerFinishMessageOutput srvr_fin_msg_out,
			    	Uint16 client_pad_length,
			    	Uint16 server_pad_length,
			    	Uint8 *client_random, 
			    	Uint8 *server_random, 
			    	Uint8 *encrypt_master_secret,
			    	Uint16 handshake_length, 
			    	Uint8 *handshake, 
			    	Uint8 *client_finished_message, 
			    	Uint8 *server_finished_message,
					Uint32 *request_id);
#endif



/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1EncryptRecordRc4
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE 
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	ssl_party = SERVER or CLIENT
 * \input	message_type = CHANGE_CIPHER_SPEC, ALERT, HANDSHAKE, or APP_DATA
 * \input	message_length = size of message in bytes (0<=length<=2^14+1024)
 * \input	message = pointer to length-byte message 
 *
 * \output	record = pointer to (length + hash_size) bytes of encrypted record 
 * \output	request_id	Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1EncryptRecordRc4(n1_request_type request_type,
					 Uint64 context_handle, 
			HashType hash_type,  
			SslVersion ssl_version, 
			SslPartyType ssl_party,
			MessageType message_type,
			Uint16 message_length, 
			Uint8 *message, 
			Uint8 *record,
			Uint32 *request_id);


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1DecryptRecordRc4
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE 
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	ssl_party = SERVER or CLIENT
 * \input	message_type = CHANGE_CIPHER_SPEC, ALERT, HANDSHAKE, or APP_DATA
 * \input	record_length = size of record in bytes (0<=length<=2^14+1024)
 * \input	record = pointer to length-byte encrypted part of record 
 *
 * \output	message = pointer to (record length - hash size) bytes 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1DecryptRecordRc4(n1_request_type request_type,
					 Uint64 context_handle, 
					 HashType hash_type, 
					 SslVersion ssl_version, 
		 			 SslPartyType ssl_party,
					 MessageType message_type, 
					 Uint16 record_length, 
					 Uint8 *record, 
					 Uint8 *message,
					 Uint32 *request_id);


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1EncryptRecord3Des
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE 
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	ssl_party = SERVER or CLIENT
 * \input	message_type = CHANGE_CIPHER_SPEC, ALERT, HANDSHAKE, or APP_DATA
 * \input	pad_length = size of extra padding in 8-byte blocks
 * \input	message_length = size of input in bytes (0<=length<=2^14+1024)
 * \input	message = pointer to length-byte input message
 *
 * \output	record_length = pointer to length of the encrypted part of the record in bytes
 * \output	record = pointer to *record_length-byte output 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1EncryptRecord3Des(n1_request_type request_type,
					  Uint64 context_handle, 
					  HashType hash_type, 
					  SslVersion ssl_version, 
					  SslPartyType ssl_party,
					  MessageType message_type, 
					  Uint16 pad_length,
					  Uint16 message_length, 
					  Uint8 *message, 
					  Uint16 *record_length, 
					  Uint8 *record,
					  Uint32 *request_id);


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1DecryptRecord3Des
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE 
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	ssl_party = SERVER or CLIENT
 * \input	message_type = CHANGE_CIPHER_SPEC, ALERT, HANDSHAKE, or APP_DATA
 * \input	record_length = size of the encrypted part of the input record in bytes 
 *		(length%8=0, 0<=length<=2^14+1024)
 * \input	record = pointer to length-byte encrypted part of the input record
 *
 * \output	message_length = pointer to length in bytes of the decrypted message
 * \output	message = pointer to *message_length-byte output 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1DecryptRecord3Des(n1_request_type request_type,
					  Uint64 context_handle, 
					  HashType hash_type, 
					  SslVersion ssl_version, 
					  SslPartyType ssl_party,
					  MessageType message_type,
					  Uint16 record_length, 
					  Uint8 *record, 
					  Uint16 *message_length, 
					  Uint8 *message,
					  Uint32 *request_id);


#ifndef MC2
/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1DecryptRecord3DesRecover
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE 
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	ssl_party = SERVER or CLIENT
 * \input	message_type = CHANGE_CIPHER_SPEC, ALERT, HANDSHAKE, or APP_DATA
 * \input	record_length = size of the encrypted part of the input record in bytes 
 *		(length%8=0, 0<=length<=2^14+1024)
 * \input	record = pointer to length-byte encrypted part of the input record
 *
 * \output	message_length = pointer to length in bytes of the decrypted message
 * \output	message = pointer to *message_length-byte output, 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1DecryptRecord3DesRecover(n1_request_type request_type,
							 Uint64 context_handle, 
							 HashType hash_type, 
							 SslVersion ssl_version,
				 			 SslPartyType ssl_party,
							 MessageType message_type,
							 Uint16 record_length, 
							 Uint8 *record, 
							 Uint16 *message_length, 
							 Uint8 *message,
							 Uint32 *request_id);
#endif


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1EncryptRecordAes
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE (MD5 hash_size = 16, SHA1 hash_size = 20)
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	ssl_party = SERVER or CLIENT
 * \input	aes_type = AES_128 or AES_256
 * \input	message_type = CHANGE_CIPHER_SPEC, ALERT, HANDSHAKE, or APP_DATA
 * \input	pad_length = size of extra padding in 8-byte blocks
 * \input	message_length = size of input in bytes (0<=length<=2^14+1024)
 * \input	message = pointer to length-byte input
 *
 * \output	record_length = pointer to length of the encrypted part of the record in bytes
 * \output	record = pointer to *record_length-byte output, 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1EncryptRecordAes(n1_request_type request_type,
					 Uint64 context_handle, 
					 HashType hash_type, 
					 SslVersion ssl_version, 
		 			 SslPartyType ssl_party,
					 AesType aes_type, 
					 MessageType message_type,
					 Uint16 pad_length,
					 Uint16 message_length, 
					 Uint8 *message, 
					 Uint16 *record_length, 
					 Uint8 *record,
					 Uint32 *request_id);



/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1DecryptRecordAes
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE 
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	ssl_party = SERVER or CLIENT
 * \input	aes_type = AES_128 or AES_256
 * \input	message_type = CHANGE_CIPHER_SPEC, ALERT, HANDSHAKE, or APP_DATA
 * \input	record_length = size of input in bytes (length%16=0, 0<=length<=2^14+1024)
 * \input	record = pointer to length-byte input
 *
 * \output	message_length = pointer to length in bytes of the decrypted message
 * \output	message = pointer to *message_length-byte output
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1DecryptRecordAes(n1_request_type request_type,
					 Uint64 context_handle, 
					 HashType hash_type, 
					 SslVersion ssl_version, 
					 SslPartyType ssl_party,
					 AesType aes_type, 
					 MessageType message_type,
					 Uint16 record_length, 
					 Uint8 *record, 
					 Uint16 *message_length, 
					 Uint8 *message,
					 Uint32 *request_id);


#ifndef MC2

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1DecryptRecordAesRecover
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	hash_type = MD5_TYPE or SHA1_TYPE (MD5 hash_size = 16, SHA1 hash_size = 20)
 * \input	ssl_version = VER3_0 or VER_TLS
 * \input	aes_type = AES_128 or AES_256
 * \input	ssl_party = SERVER or CLIENT
 * \input	message_type = CHANGE_CIPHER_SPEC, ALERT, HANDSHAKE, or APP_DATA
 * \input	record_length = size of input in bytes (length%16=0, 0<=length<=2^14+1024)
 * \input	record = pointer to length-byte input
 *
 * \output	message_length = pointer to length in bytes of the decrypted message
 * \output	message = pointer to *message_length-byte output
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1DecryptRecordAesRecover(n1_request_type request_type,
			    Uint64 context_handle, 
			    HashType hash_type, 
			    SslVersion ssl_version, 
			    SslPartyType ssl_party,
			    AesType aes_type, 
			    MessageType message_type,
			    Uint16 record_length, 
			    Uint8 *record, 
			    Uint16 *message_length, 
			    Uint8 *message,
			    Uint32 *request_id);

#endif





/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaSsl20ServerFullRc4
 *
 * Does a full SSL2.0 handshake on the server with RSA <= 2048 bits. 
 *
 *
 * Supported ciphers
 *	SSL_CK_RC4_128_WITH_MD5
 *	SSL_CK_RC4_128_EXPORT40_WITH_MD5
 *
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit byte-pointer to context (context_handle%8=0)
 * \input	key_handle = pointer to 64-bit key memory handle
 * \input	rc4_type = RC4_128 or RC4_EXPORT_40
 * \input	client_master_secret = master key received in client-master-key handshake message.
 * \input	clear_master_secret_length = length (in bytes) of clear portion of client_master_secret
 * \input	encrypted_master_secret_length = length (in bytes) of encrypted portion of client_master_secret
 * \input	modlength = size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)	
 * \input	challenge = pointer to challenge data.
 * \input	challenge_length = length (in bytes) of challenge data.
 * \input	connection_id = pointer to 16 bytes of connection ID.
 * \input	session_id = pointer to 16 bytes of Session ID.
 *
 *
 * \output	client_finished = pointer to encrypted part of client finished message 
 * \output	server_finished = pointer to encrypted part of server finished message 
 * \output	server_verify =  pointer to encrypted part of server verify message 
 * \output	master_secret = master secret to used in session caching for reuse.
 * \output	master_secret_length = size in bytes of master secret.
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 * \verbatim
 * Context format:
 *	Context is left in a state where it can be used for record processing:
 *	Word	
 *	0-15	reserved for hashing
 *	16-21	master secret
 *	To server
 *	22		Sequence number
 *	23		Unused
 *	24-15	Rc4 Key
 *	26-59	Rc4 State
 *	To client
 *	60		Sequence number
 *	61		Unused
 *	62-63	Rc4 Key
 *	64-97	Rc4 State
 * \endverbatim
 */
 /*-***************************************************************************/
Uint32
Csp1RsaSsl20ServerFullRc4(n1_request_type request_type,
						  Uint64 context_handle,
						  Uint64 *key_handle,
						  Rc4Type rc4_type,
						  Uint8 *client_master_secret,
						  Uint16 clear_master_secret_length,
						  Uint16 encrypted_master_secret_length,
						  Uint16 modlength,
						  Uint8 *challenge,
						  Uint16 challenge_length,	
						  Uint8 *connection_id,
						  Uint8 *session_id,
						  Uint8 *client_finished,	
						  Uint8 *server_finished,	
						  Uint8 *server_verify,		
						  Uint8 *master_secret,
						  Uint16 *master_secret_length,
						  Uint32 *request_id);


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaSsl20ServerClientAuthRc4
 *
 * Generates key material, and certificate req, verify messages.
 *
 *
 * Supported ciphers
 *	SSL_CK_RC4_128_WITH_MD5
 *	SSL_CK_RC4_128_EXPORT40_WITH_MD5
 *
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit byte-pointer to context (context_handle%8=0)
 * \input	key_handle = pointer to 64-bit key memory handle
 * \input	rc4_type = RC4_128 or RC4_EXPORT_40
 * \input	client_master_secret = master key received in client-master-key handshake message.
 * \input	clear_master_secret_length = length (in bytes) of clear portion of client_master_secret
 * \input	encrypted_master_secret_length = length (in bytes) of encrypted portion of client_master_secret
 * \input	modlength = size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)	
 * \input	challenge = pointer to challenge data.
 * \input	challenge_length = length (in bytes) of challenge data.
 * \input	connection_id = pointer to 16 bytes of connection ID.
 * \input	session_id = pointer to 16 bytes of Session ID.
 * \input	auth_type = SSLv2 authentication type
 * \input	cert_challenge = SSLv2 certificate challenge
 *
 * \output	cert_request = SSLv2 certificate request
 * \output	client_finished = pointer to encrypted part of client finished message 
 * \output	server_verify =  pointer to encrypted part of server verify message 
 * \output	master_secret = master secret to used in session caching for reuse.
 * \output	master_secret_length = size in bytes of master secret.
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 * \verbatim
 * Context format:
 *	Context is left in a state where it can be used for record processing:
 *	Word	
 *	0-15	reserved for hashing
 *	16-21	master secret
 *	To server
 *	22		Sequence number
 *	23		Unused
 *	24-15	Rc4 Key
 *	26-59	Rc4 State
 *	To client
 *	60		Sequence number
 *	61		Unused
 *	62-63	Rc4 Key
 *	64-97	Rc4 State
 * \endverbatim
 */
 /*-***************************************************************************/

Uint32
Csp1RsaSsl20ServerClientAuthRc4(
					n1_request_type request_type,
				 	Uint64 context_handle,
				  	Uint64 *key_handle,
				  	Rc4Type rc4_type,
				  	Uint8 *client_master_secret,
				  	Uint16 clear_master_secret_length,
				  	Uint16 encrypted_master_secret_length,
				  	Uint16 modlength,
				  	Uint8 *challenge,
				  	Uint16 challenge_length,	
				  	Uint8 *connection_id,		
				  	Uint8 *session_id,
				  	Uint8 *client_finished,	
					Uint8 auth_type,
					Uint8 *cert_challenge,
					Uint8 *cert_request,
				  	Uint8 *server_verify,		
				  	Uint8 *master_secret,
				  	Uint16 *master_secret_length,
					Uint32 *request_id);


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Ssl20ResumeRc4
 *
 * Resumes a previously negotiated session.
 *
 *
 * Supported ciphers
 *	SSL_CK_RC4_128_WITH_MD5
 *	SSL_CK_RC4_128_EXPORT40_WITH_MD5
 *
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit byte-pointer to context (context_handle%8=0)
 * \input	key_handle = pointer to 64-bit key memory handle
 * \input	rc4_type = RC4_128 or RC4_EXPORT_40
 * \input	master_secret = master secret from previous session.
 * \input	master_secret_length = size in bytes of master secret.
 * \input	challenge = pointer to challenge data.
 * \input	challenge_length = length (in bytes) of challenge data.
 * \input	connection_id = pointer to 16 bytes of connection ID.
 * \input	session_id = pointer to 16 bytes of Session ID.
 *
 *
 * \output	client_finished = pointer to encrypted part of client finished message 
 * \output	server_finished = pointer to encrypted part of server finished message 
 * \output	server_verify =  pointer to encrypted part of server verify message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 * \verbatim
 * Context format:
 *	Context is left in a state where it can be used for record processing:
 *	Word	
 *	0-15	reserved for hashing
 *	16-21	master secret
 *	To server
 *	22		Sequence number
 *	23		Unused
 *	24-15	Rc4 Key
 *	26-59	Rc4 State
 *	To client
 *	60		Sequence number
 *	61		Unused
 *	62-63	Rc4 Key
 *	64-97	Rc4 State
 * \endverbatim
 */
 /*-***************************************************************************/

Uint32
Csp1Ssl20ResumeRc4 (n1_request_type request_type,
			Uint64 context_handle,
			Uint64 *key_handle,
			Rc4Type rc4_type,
			Uint8 *master_secret,
			Uint16 master_secret_length,
			Uint8 *challenge,
			Uint16 challenge_length,	
			Uint8 *connection_id,		
			Uint8 *session_id,
			Uint8 *client_finished,	
			Uint8 *server_finished,	
			Uint8 *server_verify,
			Uint32 *request_id);



/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Ssl20ResumeClientAuthRc4
 *
 * Uses master key from a previous session, generates key material, cert req,
 * and verify messages.
 *
 *
 * Supported ciphers
 *	SSL_CK_RC4_128_WITH_MD5
 *	SSL_CK_RC4_128_EXPORT40_WITH_MD5
 *
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit byte-pointer to context (context_handle%8=0)
 * \input	key_handle = pointer to 64-bit key memory handle
 * \input	rc4_type = RC4_128 or RC4_EXPORT_40
 * \input	master_secret = master secret from previous session.
 * \input	master_secret_length = size in bytes of master secret.
 * \input	challenge = pointer to challenge data.
 * \input	challenge_length = length (in bytes) of challenge data.
 * \input	connection_id = pointer to 16 bytes of connection ID.
 * \input	session_id = pointer to 16 bytes of Session ID.
 * \input	client_finished = pointer to encrypted part of client finished message 
 * \input	auth_type = client auth type
 * \input	cert_challenge = cert challenge
 *
 * \output	cert_request = pointer to encrypted part of cert request message 
 * \output	server_verify =  pointer to encrypted part of server verify message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 * \verbatim
 * Context format:
 *	Context is left in a state where it can be used for record processing:
 *	Word	
 *	0-15	reserved for hashing
 *	16-21	master secret
 *	To server
 *	22		Sequence number
 *	23		Unused
 *	24-15	Rc4 Key
 *	26-59	Rc4 State
 *	To client
 *	60		Sequence number
 *	61		Unused
 *	62-63	Rc4 Key
 *	64-97	Rc4 State
 * \endverbatim
 */
 /*-***************************************************************************/

Uint32
Csp1Ssl20ResumeClientAuthRc4 (n1_request_type request_type,
					Uint64 context_handle,
					Uint64 *key_handle,
					Rc4Type rc4_type,
					Uint8 *master_secret,
					Uint16 master_secret_length,
					Uint8 *challenge,
					Uint16 challenge_length,	
					Uint8 *connection_id,		
					Uint8 *session_id,
					Uint8 *client_finished,	
					Uint8 auth_type,
					Uint8 *cert_challenge,
					Uint8 *cert_request,
					Uint8 *server_verify,
					Uint32 *request_id);



/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaSsl20ServerFull3Des
 *
 * Does a full SSL2.0 handshake on the server with RSA <= 2048 bits. 
 *
 *
 * Supported ciphers
 *	SSL_CK_DES_64_CBC_WITH_MD5
	SSL_CK_DES_192_EDE3_CBC_WITH_MD5
 *
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit byte-pointer to context (context_handle%8=0)
 * \input	key_handle = pointer to 64-bit key memory handle
 * \input	des_type = DES or DES3_192
 * \input	client_master_secret = master key received in client-master-key handshake message.
 * \input	clear_master_secret_length = length (in bytes) of clear portion of client_master_secret
 * \input	encrypted_master_secret_length = length (in bytes) of encrypted portion of client_master_secret
 * \input	modlength = size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)	
 * \input	challenge = pointer to challenge data.
 * \input	challenge_length = length (in bytes) of challenge data.
 * \input	connection_id = pointer to 16 bytes of connection ID.
 * \input	session_id = pointer to 16 bytes of Session ID.
 * \input	iv = initialization vectore sent by client
 *
 *
 * \output	client_finished = pointer to encrypted part of client finished message 
 * \output	server_finished = pointer to encrypted part of server finished message 
 * \output	server_verify =  pointer to encrypted part of server verify message 
 * \output	master_secret = master secret to used in session caching for reuse.
 * \output	master_secret_length = size in bytes of master secret.
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 * \verbatim
 * Context format:
 *	Context is left in a state where it can be used for record processing:
 *	Word	
 *	0-15	reserved for hashing
 *	16-21	master secret
 *	To server
 *	22		Sequence number
 *	23		Unused
 *	24		IV
 *	25-27	3Des Keys
 *	To client
 *	28		Sequence number
 *	29		Unused
 *	30		IV
 *	31-33	3Des Keys
 * \endverbatim
 */
 /*-***************************************************************************/
Uint32
Csp1RsaSsl20ServerFull3Des(n1_request_type request_type,
						   Uint64 context_handle,
						  Uint64 *key_handle,
						  DesType des_type,
						  Uint8 *client_master_secret,
						  Uint16 clear_master_secret_length,
						  Uint16 encrypted_master_secret_length,
						  Uint16 modlength,
						  Uint8 *challenge,
						  Uint16 challenge_length,	
						  Uint8 *connection_id,		
						  Uint8 *session_id,
						  Uint8 *iv,
						  Uint8 *client_finished,	
						  Uint8 *server_finished,	
						  Uint8 *server_verify,		
						  Uint8 *master_secret,
						  Uint16 *master_secret_length,
						  Uint32 *request_id);






/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1RsaSsl20ServerClientAuth3Des
 *
 * Generates key material, verifies cllient finished msg, creates server verify
 * and cert request messages.
 *
 *
 * Supported ciphers
 *	SSL_CK_DES_64_CBC_WITH_MD5
	SSL_CK_DES_192_EDE3_CBC_WITH_MD5
 *
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit byte-pointer to context (context_handle%8=0)
 * \input	key_handle = pointer to 64-bit key memory handle
 * \input	des_type = DES or DES3_192
 * \input	client_master_secret = master key received in client-master-key handshake message.
 * \input	clear_master_secret_length = length (in bytes) of clear portion of client_master_secret
 * \input	encrypted_master_secret_length = length (in bytes) of encrypted portion of client_master_secret
 * \input	modlength = size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)	
 * \input	challenge = pointer to challenge data.
 * \input	challenge_length = length (in bytes) of challenge data.
 * \input	connection_id = pointer to 16 bytes of connection ID.
 * \input	session_id = pointer to 16 bytes of Session ID.
 * \input	iv = initialization vectore sent by client
 * \input	auth_type =  client auth type
 * \input	cert_challenge =  certficate challenge.
 *
 *
 * \output	client_finished = pointer to encrypted part of client finished message 
 * \output	cert_request = pointer to encrypted part of certificate request message 
 * \output	server_verify =  pointer to encrypted part of server verify message 
 * \output	master_secret = master secret to used in session caching for reuse.
 * \output	master_secret_length = size in bytes of master secret.
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 * \verbatim
 * Context format:
 *	Context is left in a state where it can be used for record processing:
 *	Word	
 *	0-15	reserved for hashing
 *	16-21	master secret
 *	To server
 *	22		Sequence number
 *	23		Unused
 *	24		IV
 *	25-27	3Des Keys
 *	To client
 *	28		Sequence number
 *	29		Unused
 *	30		IV
 *	31-33	3Des Keys
 * \endverbatim
 */
 /*-***************************************************************************/

Uint32
Csp1RsaSsl20ServerClientAuth3Des(
						n1_request_type request_type,
						Uint64 context_handle,
						Uint64 *key_handle,
						DesType des_type,
						Uint8 *client_master_secret,
						Uint16 clear_master_secret_length,
						Uint16 encrypted_master_secret_length,
						Uint16 modlength,
						Uint8 *challenge,
						Uint16 challenge_length,	
						Uint8 *connection_id,		
						Uint8 *session_id,
						Uint8 *iv,
						Uint8 *client_finished,	
						Uint8 auth_type,
						Uint8 *cert_challenge,
						Uint8 *cert_request,	
						Uint8 *server_verify,		
						Uint8 *master_secret,
						Uint16 *master_secret_length,
						Uint32 *request_id);



/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Ssl20Resume3Des
 *
 * Resumes a previouly negotiated handshake. 
 *
 *
 * Supported ciphers
 *	SSL_CK_DES_64_CBC_WITH_MD5
 *	SSL_CK_DES_192_EDE3_CBC_WITH_MD5
 *
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit byte-pointer to context (context_handle%8=0)
 * \input	key_handle = pointer to 64-bit key memory handle
 * \input	des_type = DES or DES3_192
 * \input	master_secret = master key generated in previous handshake
 * \input	master_secret_length = size in bytes of master secret.
 * \input	challenge = pointer to challenge data.
 * \input	challenge_length = length (in bytes) of challenge data.
 * \input	connection_id = pointer to 16 bytes of connection ID.
 * \input	session_id = pointer to 16 bytes of Session ID.
 * \input	iv = initialization vectore sent by client
 *
 *
 * \output	client_finished = pointer to encrypted part of client finished message 
 * \output	server_finished = pointer to encrypted part of server finished message 
 * \output	server_verify =  pointer to encrypted part of server verify message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 * \verbatim
 * Context format:
 *	Context is left in a state where it can be used for record processing:
 *	Word	
 *	0-15	reserved for hashing
 *	16-21	master secret
 *	To server
 *	22		Sequence number
 *	23		Unused
 *	24		IV
 *	25-27	3Des Keys
 *	To client
 *	28		Sequence number
 *	29		Unused
 *	30		IV
 *	31-33	3Des Keys
 * \endverbatim
 */
 /*-***************************************************************************/
Uint32
Csp1Ssl20Resume3Des(
			n1_request_type request_type,
			Uint64 context_handle,
		  Uint64 *key_handle,
		  DesType des_type,
		  Uint8 *master_secret,
		  Uint16 master_secret_length,
		  Uint8 *challenge,
		  Uint16 challenge_length,	
		  Uint8 *connection_id,		
		  Uint8 *session_id,
		  Uint8 *iv,
		  Uint8 *client_finished,	
		  Uint8 *server_finished,	
		  Uint8 *server_verify,
		  Uint32 *request_id);



/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Ssl20ResumeClientAuth3Des
 *
 * Uses master key from a previous session, generates key material, cert req,
 * and verify messages.
 *
 *
 * Supported ciphers
 *	SSL_CK_DES_64_CBC_WITH_MD5
	SSL_CK_DES_192_EDE3_CBC_WITH_MD5
 *
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit byte-pointer to context (context_handle%8=0)
 * \input	key_handle = pointer to 64-bit key memory handle
 * \input	des_type = DES or DES3_192
 * \input	master_secret = master key generated in previous handshake
 * \input	master_secret_length = size in bytes of master secret.
 * \input	challenge = pointer to challenge data.
 * \input	challenge_length = length (in bytes) of challenge data.
 * \input	connection_id = pointer to 16 bytes of connection ID.
 * \input	session_id = pointer to 16 bytes of Session ID.
 * \input	iv = initialization vectore sent by client
 * \input	client_finished = pointer to encrypted part of client finished message 
 * \input	auth_type =  client authentication type
 * \input	cert_challenge = cert request challenge
 * \input	cert_request = certificate request
 *
 *
 *
 * \output	cert_request = pointer to encrypted part of cert request message 
 * \output	server_verify =  pointer to encrypted part of server verify message 
 * \output  request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 * \verbatim
 * Context format:
 *	Context is left in a state where it can be used for record processing:
 *	Word	
 *	0-15	reserved for hashing
 *	16-21	master secret
 *	To server
 *	22		Sequence number
 *	23		Unused
 *	24		IV
 *	25-27	3Des Keys
 *	To client
 *	28		Sequence number
 *	29		Unused
 *	30		IV
 *	31-33	3Des Keys
 * \endverbatim
 */
 /*-***************************************************************************/
Uint32
Csp1Ssl20ResumeClientAuth3Des(
			n1_request_type request_type,
			Uint64 context_handle,
		  Uint64 *key_handle,
		  DesType des_type,
		  Uint8 *master_secret,
		  Uint16 master_secret_length,
		  Uint8 *challenge,
		  Uint16 challenge_length,	
		  Uint8 *connection_id,		
		  Uint8 *session_id,
		  Uint8 *iv,
		  Uint8 *client_finished,
		  Uint8 auth_type,
		  Uint8 *cert_challenge,
		  Uint8 *cert_request,	
		  Uint8 *server_verify,
		  Uint32 *request_id);

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Ssl20DecryptRecordRc4
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	record_length = size of record in bytes (0<=length<=2^16-1)
 * \input	record = pointer to length-byte encrypted part of record 
 *
 * \output	message = pointer to decrypted message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32
Csp1Ssl20DecryptRecordRc4(
				n1_request_type request_type,
				Uint64 context_handle,
				Uint16 record_length,
				Uint8 *record,
				Uint8 *message,
				Uint32 *request_id);



/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Ssl20EncryptRecordRc4
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	message_length = size of message in bytes (0<=length<=2^16-1)
 * \input	message = pointer to length-byte message 
 *
 * \output	record = pointer to encrypted record 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32
Csp1Ssl20EncryptRecordRc4(
				n1_request_type request_type,
				Uint64 context_handle,
				Uint16 message_length,
				Uint8 *message,
				Uint8 *record,
				Uint32 *request_id);


/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Ssl20DecryptRecord3Des
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	des_type = DES or DES3_192
 * \input	record_length = size of record in bytes (0<=length<=2^16-1)
 * \input	record = pointer to length-byte encrypted part of record 
 *
 * \output	message = pointer to decrypted message 
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32 
Csp1Ssl20DecryptRecord3Des(
					n1_request_type request_type,
					Uint64 context_handle,
				   DesType des_type,
				   Uint16 record_length,
				   Uint8 *record,
				   Uint8 *message,
				   Uint32 *request_id);

/*+****************************************************************************/
 /*! \ingroup SSL_OPS
 *
 * Csp1Ssl20EncryptRecord3Des
 *
 * \input	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input	context_handle = 64-bit pointer to context (context_handle%8=0)
 * \input	des_type = DES or DES3_192
 * \input	message_length = size of message in bytes (0<=length<=2^16-1)
 * \input	message = pointer to length-byte message 
 *
 * \output	record = pointer to encrypted record 
 * \output	record_length = SSLv2 record size in bytes
 * \output	request_id = Unique ID for this request
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
 /*-***************************************************************************/
Uint32
Csp1Ssl20EncryptRecord3Des(	
				n1_request_type request_type,
				Uint64 context_handle,
				DesType des_type,
				Uint16 message_length,
				Uint8 *message,
				Uint16 *record_length,
				Uint8 *record,
				Uint32 *request_id);





/*
 *	add by lijing
 *	kernel ssl call 
*/
Uint32 Kernel_Csp1SetEncryptedMasterSecretKey (Uint8 * key, Uint32 *request_id, void (*call_back)(int, void *), void *connect);
Uint32 Kernel_Csp1RsaServerFullRc4(n1_request_type request_type,
		      Uint64 context_handle,
		      Uint64 * key_handle,
		      HashType hash_type,
			SslVersion ssl_version,
		      Rc4Type rc4_type,
		      MasterSecretReturn master_secret_ret,
		      Uint16 modlength,
		      Uint8 * encrypt_premaster_secret,
		      Uint8 * client_random,
		      Uint8 * server_random,
		      Uint16 handshake_length,
		      Uint8 * handshake,
		      Uint8 * client_finished_message,
		      Uint8 * server_finished_message,
		      Uint8 * encrypt_master_secret, 
			  Uint32 *request_id,
			  void (*call_back)(int, void*),
			  void *connect);
Uint32 Kernel_Csp1RsaServerVerifyRc4 (n1_request_type request_type,
			Uint64 context_handle,
			Uint64 * key_handle,
			HashType hash_type,
			SslVersion ssl_version,
			Rc4Type rc4_type,
			MasterSecretReturn master_secret_ret,
			Uint16 modlength,
			Uint8 * encrypt_premaster_secret,
			Uint8 * client_random,
			Uint8 * server_random,
			Uint16 handshake_length,
			Uint8 * handshake,
			Uint8 * verify_data,
			Uint8 * encrypt_master_secret, Uint32 * request_id,
			void (*call_back)(int, void*),
			void *connect);			  
Uint32 Kernel_Csp1RsaServerFull3Des (n1_request_type request_type,
		       Uint64 context_handle,
		       Uint64 * key_handle,
		       HashType hash_type,
		       SslVersion ssl_version,
		       DesType des_type,
		       MasterSecretReturn master_secret_ret,
		       ClientFinishMessageOutput clnt_fin_msg_out,
		       ServerFinishMessageOutput srvr_fin_msg_out,
		       Uint16 client_pad_length,
		       Uint16 server_pad_length,
		       Uint16 modlength,
		       Uint8 * encrypt_premaster_secret,
		       Uint8 * client_random,
		       Uint8 * server_random,
		       Uint16 handshake_length,
		       Uint8 * handshake,
		       Uint8 * client_finished_message,
		       Uint8 * server_finished_message,
		       Uint8 * encrypt_master_secret, Uint32 * request_id,
			   void (*call_back)(int, void*),
			   void* connect);
Uint32 Kernel_Csp1RsaServerVerify3Des (n1_request_type request_type,
			 Uint64 context_handle,
			 Uint64 * key_handle,
			 HashType hash_type,
			 SslVersion ssl_version,
			 DesType des_type,
			 MasterSecretReturn master_secret_ret,
			 Uint16 modlength,
			 Uint8 * encrypt_premaster_secret,
			 Uint8 * client_random,
			 Uint8 * server_random,
			 Uint16 handshake_length,
			 Uint8 * handshake,
			 Uint8 * verify_data,
			 Uint8 * encrypt_master_secret, Uint32 * request_id,
			 void (*call_back)(int, void*),
			 void* connect);			   
Uint32 Kernel_Csp1RsaServerFullAes (n1_request_type request_type,
		      Uint64 context_handle,
		      Uint64 * key_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      AesType aes_type,
		      MasterSecretReturn master_secret_ret,
		      ClientFinishMessageOutput clnt_fin_msg_out,
		      ServerFinishMessageOutput srvr_fin_msg_out,
		      Uint16 client_pad_length,
		      Uint16 server_pad_length,
		      Uint16 modlength,
		      Uint8 * encrypt_premaster_secret,
		      Uint8 * client_random,
		      Uint8 * server_random,
		      Uint16 handshake_length,
		      Uint8 * handshake,
		      Uint8 * client_finished_message,
		      Uint8 * server_finished_message,
		      Uint8 * encrypt_master_secret, Uint32 * request_id,
			  void (*call_back)(int, void*),
			  void* connect);	
Uint32 Kernel_Csp1RsaServerVerifyAes (n1_request_type request_type,
			Uint64 context_handle,
			Uint64 * key_handle,
			HashType hash_type,
			SslVersion ssl_version,
			AesType aes_type,
			MasterSecretReturn master_secret_ret,
			Uint16 modlength,
			Uint8 * encrypt_premaster_secret,
			Uint8 * client_random,
			Uint8 * server_random,
			Uint16 handshake_length,
			Uint8 * handshake,
			Uint8 * verify_data,
			Uint8 * encrypt_master_secret, Uint32 * request_id,
			void (*call_back)(int, void*),
			void *connect);
Uint32 Kernel_Csp1OtherFullRc4 (n1_request_type request_type,
		  Uint64 context_handle,
		  HashType hash_type,
		  SslVersion ssl_version,
		  Rc4Type rc4_type,
		  MasterSecretReturn master_secret_ret,
		  Uint16 pre_master_length,
		  Uint8 * client_random,
		  Uint8 * server_random,
		  Uint16 handshake_length,
		  Uint8 * handshake,
		  Uint8 * client_finished_message,
		  Uint8 * server_finished_message,
		  Uint8 * encrypt_master_secret, Uint32 * request_id,
		  void (*call_back)(int, void*),
		  void *connect);
Uint32 Kernel_Csp1OtherFullRc4 (n1_request_type request_type,
		  Uint64 context_handle,
		  HashType hash_type,
		  SslVersion ssl_version,
		  Rc4Type rc4_type,
		  MasterSecretReturn master_secret_ret,
		  Uint16 pre_master_length,
		  Uint8 * client_random,
		  Uint8 * server_random,
		  Uint16 handshake_length,
		  Uint8 * handshake,
		  Uint8 * client_finished_message,
		  Uint8 * server_finished_message,
		  Uint8 * encrypt_master_secret, Uint32 * request_id,
		  void (*call_back)(int, void*),
		  void *connect);		  
Uint32 Kernel_Csp1OtherVerifyRc4 (n1_request_type request_type,
		    Uint64 context_handle,
		    HashType hash_type,
		    SslVersion ssl_version,
		    Rc4Type rc4_type,
		    MasterSecretReturn master_secret_ret,
		    Uint16 pre_master_length,
		    Uint8 * client_random,
		    Uint8 * server_random,
		    Uint16 handshake_length,
		    Uint8 * handshake,
		    Uint8 * verify_data,
		    Uint8 * encrypt_master_secret, Uint32 * request_id,
			void (*call_back)(int, void*),
			void *connect);
Uint32 Kernel_Csp1OtherFull3Des (n1_request_type request_type,
		   Uint64 context_handle,
		   HashType hash_type,
		   SslVersion ssl_version,
		   DesType des_type,
		   MasterSecretReturn master_secret_ret,
		   ClientFinishMessageOutput clnt_fin_msg_out,
		   ServerFinishMessageOutput srvr_fin_msg_out,
		   Uint16 client_pad_length,
		   Uint16 server_pad_length,
		   Uint16 pre_master_length,
		   Uint8 * client_random,
		   Uint8 * server_random,
		   Uint16 handshake_length,
		   Uint8 * handshake,
		   Uint8 * client_finished_message,
		   Uint8 * server_finished_message,
		   Uint8 * encrypt_master_secret, Uint32 * request_id,
		   void (*call_back)(int, void*),
		   void *connect);	
Uint32 Kernel_Csp1OtherVerify3Des (n1_request_type request_type,
		     Uint64 context_handle,
		     HashType hash_type,
		     SslVersion ssl_version,
		     DesType des_type,
		     MasterSecretReturn master_secret_ret,
		     Uint16 pre_master_length,
		     Uint8 * client_random,
		     Uint8 * server_random,
		     Uint16 handshake_length,
		     Uint8 * handshake,
		     Uint8 * verify_data,
		     Uint8 * encrypt_master_secret, Uint32 * request_id,
			 void (*call_back)(int, void*),
			 void* connect);
Uint32 Kernel_Csp1OtherFullAes (n1_request_type request_type,
		  Uint64 context_handle,
		  HashType hash_type,
		  SslVersion ssl_version,
		  AesType aes_type,
		  MasterSecretReturn master_secret_ret,
		  ClientFinishMessageOutput clnt_fin_msg_out,
		  ServerFinishMessageOutput srvr_fin_msg_out,
		  Uint16 client_pad_length,
		  Uint16 server_pad_length,
		  Uint16 pre_master_length,
		  Uint8 * client_random,
		  Uint8 * server_random,
		  Uint16 handshake_length,
		  Uint8 * handshake,
		  Uint8 * client_finished_message,
		  Uint8 * server_finished_message,
		  Uint8 * encrypt_master_secret, Uint32 * request_id,
		  void (*call_back)(int, void*),
		  void *connect);	
Uint32 Kernel_Csp1OtherVerifyAes (n1_request_type request_type,
		    Uint64 context_handle,
		    HashType hash_type,
		    SslVersion ssl_version,
		    AesType aes_type,
		    MasterSecretReturn master_secret_ret,
		    Uint16 pre_master_length,
		    Uint8 * client_random,
		    Uint8 * server_random,
		    Uint16 handshake_length,
		    Uint8 * handshake,
		    Uint8 * verify_data,
		    Uint8 * encrypt_master_secret, Uint32 * request_id,
			void (*call_back)(int, void*),
			void* connect);
Uint32 Kernel_Csp1FinishedRc4Finish (n1_request_type request_type,
		       Uint64 context_handle,
		       HashType hash_type,
		       SslVersion ssl_version,
		       Uint16 handshake_length,
		       Uint8 * handshake,
		       Uint8 * client_finished_message,
		       Uint8 * server_finished_message, Uint32 * request_id,
			   void (*call_back)(int, void*), 
			   void* connect);	
Uint32 Kernel_Csp1Finished3DesFinish (n1_request_type request_type,
			Uint64 context_handle,
			HashType hash_type,
			SslVersion ssl_version,
			ClientFinishMessageOutput clnt_fin_msg_out,
			ServerFinishMessageOutput srvr_fin_msg_out,
			Uint16 client_pad_length,
			Uint16 server_pad_length,
			Uint16 handshake_length,
			Uint8 * handshake,
			Uint8 * client_finished_message,
			Uint8 * server_finished_message, Uint32 * request_id,
			void (*call_back)(int, void *),
			void *connect);
Uint32 Kernel_Csp1FinishedAesFinish (n1_request_type request_type,
		       Uint64 context_handle,
		       HashType hash_type,
		       SslVersion ssl_version,
		       AesType aes_type,
		       ClientFinishMessageOutput clnt_fin_msg_out,
		       ServerFinishMessageOutput srvr_fin_msg_out,
		       Uint16 client_pad_length,
		       Uint16 server_pad_length,
		       Uint16 handshake_length,
		       Uint8 * handshake,
		       Uint8 * client_finished_message,
		       Uint8 * server_finished_message, Uint32 * request_id,
			   void (*call_back)(int, void*),
			   void *connect);	
Uint32 Kernel_Csp1ResumeRc4 (n1_request_type request_type,
	       Uint64 context_handle,
	       HashType hash_type,
	       SslVersion ssl_version,
	       Rc4Type rc4_type,
	       MasterSecretInput master_secret_inp,
	       Uint8 * client_random,
	       Uint8 * server_random,
	       Uint8 * encrypt_master_secret,
	       Uint16 handshake_length,
	       Uint8 * handshake,
	       Uint8 * client_finished_message,
	       Uint8 * server_finished_message, Uint32 * request_id,
		   void (*call_back)(int, void*),
		   void* connect);			   		
Uint32 Kernel_Csp1Resume3Des (n1_request_type request_type,
		Uint64 context_handle,
		HashType hash_type,
		SslVersion ssl_version,
		DesType des_type,
		MasterSecretInput master_secret_inp,
		ClientFinishMessageOutput clnt_fin_msg_out,
		ServerFinishMessageOutput srvr_fin_msg_out,
		Uint16 client_pad_length,
		Uint16 server_pad_length,
		Uint8 * client_random,
		Uint8 * server_random,
		Uint8 * encrypt_master_secret,
		Uint16 handshake_length,
		Uint8 * handshake,
		Uint8 * client_finished_message,
		Uint8 * server_finished_message, Uint32 * request_id,
		void (*call_back)(int, void*),
		void *connect);
Uint32 Kernel_Csp1ResumeAes (n1_request_type request_type,
	       Uint64 context_handle,
	       HashType hash_type,
	       SslVersion ssl_version,
	       AesType aes_type,
	       MasterSecretInput master_secret_inp,
	       ClientFinishMessageOutput clnt_fin_msg_out,
	       ServerFinishMessageOutput srvr_fin_msg_out,
	       Uint16 client_pad_length,
	       Uint16 server_pad_length,
	       Uint8 * client_random,
	       Uint8 * server_random,
	       Uint8 * encrypt_master_secret,
	       Uint16 handshake_length,
	       Uint8 * handshake,
	       Uint8 * client_finished_message,
	       Uint8 * server_finished_message, Uint32 * request_id,
		   void (*call_back)(int, void *),
		   void *connect);	
Uint32 Kernel_Csp1EncryptRecordRc4 (n1_request_type request_type,
		      Uint64 context_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      SslPartyType ssl_party,
		      MessageType message_type,
		      Uint16 message_length,
		      Uint8 * message, Uint8 * record, Uint32 * request_id,
			  void (*call_back)(int, void*),
			  void *connect);
Uint32 Kernel_Csp1DecryptRecordRc4 (n1_request_type request_type,
		      Uint64 context_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      SslPartyType ssl_party,
		      MessageType message_type,
		      Uint16 record_length,
		      Uint8 * record, Uint8 * message, Uint32 * request_id,
			  void (*call_back)(int, void*),
			  void *connect);			  		   	
Uint32 Kernel_Csp1EncryptRecord3Des (n1_request_type request_type,
		       Uint64 context_handle,
		       HashType hash_type,
		       SslVersion ssl_version,
		       SslPartyType ssl_party,
		       MessageType message_type,
		       Uint16 pad_length,
		       Uint16 message_length,
		       Uint8 * message,
		       Uint16 * record_length,
		       Uint8 * record, Uint32 * request_id,
			   void (*call_back)(int, void*),
			   void *connect);
Uint32 Kernel_Csp1DecryptRecord3Des (n1_request_type request_type,
		       Uint64 context_handle,
		       HashType hash_type,
		       SslVersion ssl_version,
		       SslPartyType ssl_party,
		       MessageType message_type,
		       Uint16 record_length,
		       Uint8 * record,
		       Uint16 * message_length,
		       Uint8 * message, Uint32 * request_id,
			   void (*call_back)(int, void *),
			   void *connect);		
Uint32 Kernel_Csp1EncryptRecordAes (n1_request_type request_type,
		      Uint64 context_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      SslPartyType ssl_party,
		      AesType aes_type,
		      MessageType message_type,
		      Uint16 pad_length,
		      Uint16 message_length,
		      Uint8 * message,
		      Uint16 * record_length,
		      Uint8 * record, Uint32 * request_id,
			  void (*call_back)(int, void *),
			  void *connect);		
Uint32
Kernel_Csp1DecryptRecordAes (n1_request_type request_type,
		      Uint64 context_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      SslPartyType ssl_party,
		      AesType aes_type,
		      MessageType message_type,
		      Uint16 record_length,
		      Uint8 * record,
		      Uint16 * message_length,
		      Uint8 * message, Uint32 * request_id,
			  void (*call_back)(int, void *),
			  void *connect);
Uint32
Kernel_Csp1RsaSsl20ServerFullRc4 (n1_request_type request_type,
			   Uint64 context_handle,
			   Uint64 * key_handle,
			   Rc4Type rc4_type,
			   Uint8 * client_master_secret,
			   Uint16 clear_master_secret_length,
			   Uint16 encrypted_master_secret_length,
			   Uint16 modlength,
			   Uint8 * challenge,
			   Uint16 challenge_length,
			   Uint8 * connection_id,
			   Uint8 * session_id,
			   Uint8 * client_finished,
			   Uint8 * server_finished,
			   Uint8 * server_verify,
			   Uint8 * master_secret,
			   Uint16 * master_secret_length, Uint32 * request_id,
			   void (*call_back)(int, void *),
			   void *connect);		
Uint32
Kernel_Csp1RsaSsl20ServerClientAuthRc4 (n1_request_type request_type,
				 Uint64 context_handle,
				 Uint64 * key_handle,
				 Rc4Type rc4_type,
				 Uint8 * client_master_secret,
				 Uint16 clear_master_secret_length,
				 Uint16 encrypted_master_secret_length,
				 Uint16 modlength,
				 Uint8 * challenge,
				 Uint16 challenge_length,
				 Uint8 * connection_id,
				 Uint8 * session_id,
				 Uint8 * client_finished,
				 Uint8 auth_type,
				 Uint8 * cert_challenge,
				 Uint8 * cert_request,
				 Uint8 * server_verify,
				 Uint8 * master_secret,
				 Uint16 * master_secret_length,
				 Uint32 * request_id,
				 void (*call_back)(int, void *),
				 void *connect);
Uint32
Kernel_Csp1Ssl20ResumeRc4 (n1_request_type request_type,
		    Uint64 context_handle,
		    Uint64 * key_handle,
		    Rc4Type rc4_type,
		    Uint8 * master_secret,
		    Uint16 master_secret_length,
		    Uint8 * challenge,
		    Uint16 challenge_length,
		    Uint8 * connection_id,
		    Uint8 * session_id,
		    Uint8 * client_finished,
		    Uint8 * server_finished,
		    Uint8 * server_verify, Uint32 * request_id,
			void (*call_back)(int, void *),
			void *connect);
Uint32
Kernel_Csp1Ssl20ResumeClientAuthRc4 (n1_request_type request_type,
			      Uint64 context_handle,
			      Uint64 * key_handle,
			      Rc4Type rc4_type,
			      Uint8 * master_secret,
			      Uint16 master_secret_length,
			      Uint8 * challenge,
			      Uint16 challenge_length,
			      Uint8 * connection_id,
			      Uint8 * session_id,
			      Uint8 * client_finished,
			      Uint8 auth_type,
			      Uint8 * cert_challenge,
			      Uint8 * cert_request,
			      Uint8 * server_verify, Uint32 * request_id,
				  void (*call_back)(int, void *),
				  void *connect);
Uint32
Kernel_Csp1RsaSsl20ServerFull3Des (n1_request_type request_type,
			    Uint64 context_handle,
			    Uint64 * key_handle,
			    DesType des_type,
			    Uint8 * client_master_secret,
			    Uint16 clear_master_secret_length,
			    Uint16 encrypted_master_secret_length,
			    Uint16 modlength,
			    Uint8 * challenge,
			    Uint16 challenge_length,
			    Uint8 * connection_id,
			    Uint8 * session_id,
			    Uint8 * iv,
			    Uint8 * client_finished,
			    Uint8 * server_finished,
			    Uint8 * server_verify,
			    Uint8 * master_secret,
			    Uint16 * master_secret_length,
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect);
Uint32
Kernel_Csp1RsaSsl20ServerClientAuth3Des (n1_request_type request_type,
				  Uint64 context_handle,
				  Uint64 * key_handle,
				  DesType des_type,
				  Uint8 * client_master_secret,
				  Uint16 clear_master_secret_length,
				  Uint16 encrypted_master_secret_length,
				  Uint16 modlength,
				  Uint8 * challenge,
				  Uint16 challenge_length,
				  Uint8 * connection_id,
				  Uint8 * session_id,
				  Uint8 * iv,
				  Uint8 * client_finished,
				  Uint8 auth_type,
				  Uint8 * cert_challenge,
				  Uint8 * cert_request,
				  Uint8 * server_verify,
				  Uint8 * master_secret,
				  Uint16 * master_secret_length,
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect);
Uint32
Kernel_Csp1Ssl20Resume3Des (n1_request_type request_type,
		     Uint64 context_handle,
		     Uint64 * key_handle,
		     DesType des_type,
		     Uint8 * master_secret,
		     Uint16 master_secret_length,
		     Uint8 * challenge,
		     Uint16 challenge_length,
		     Uint8 * connection_id,
		     Uint8 * session_id,
		     Uint8 * iv,
		     Uint8 * client_finished,
		     Uint8 * server_finished,
		     Uint8 * server_verify, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect);
Uint32
Kernel_Csp1Ssl20ResumeClientAuth3Des (n1_request_type request_type,
			       Uint64 context_handle,
			       Uint64 * key_handle,
			       DesType des_type,
			       Uint8 * master_secret,
			       Uint16 master_secret_length,
			       Uint8 * challenge,
			       Uint16 challenge_length,
			       Uint8 * connection_id,
			       Uint8 * session_id,
			       Uint8 * iv,
			       Uint8 * client_finished,
			       Uint8 auth_type,
			       Uint8 * cert_challenge,
			       Uint8 * cert_request,
			       Uint8 * server_verify, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect);
Uint32
Kernel_Csp1Ssl20DecryptRecordRc4 (n1_request_type request_type,
			   Uint64 context_handle,
			   Uint16 record_length,
			   Uint8 * record,
			   Uint8 * message, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect);
Uint32
Kernel_Csp1Ssl20EncryptRecordRc4 (n1_request_type request_type,
			   Uint64 context_handle,
			   Uint16 message_length,
			   Uint8 * message,
			   Uint8 * record, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect);
Uint32
Kernel_Csp1Ssl20DecryptRecord3Des (n1_request_type request_type,
			    Uint64 context_handle,
			    DesType des_type,
			    Uint16 record_length,
			    Uint8 * record,
			    Uint8 * message, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect);
Uint32
Kernel_Csp1Ssl20EncryptRecord3Des (n1_request_type request_type,
			    Uint64 context_handle,
			    DesType des_type,
			    Uint16 message_length,
			    Uint8 * message,
			    Uint16 * record_length,
			    Uint8 * record, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect);
//#endif /* CSP1_KERNEL */				
/*
 *	end add by lijing
*/																																				  							 			   	  			  	   	   
#endif /* _CAVIUM_SSL_H_ */
