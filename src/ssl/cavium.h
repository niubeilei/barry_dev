////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_ssl_Cavium_h
#define Aos_ssl_Cavium_h

typedef enum {UNSUPPORTED_RC4 = -1, RC4_128 = 8, RC4_EXPORT_40 = 9, RC4_EXPORT_56 = 11} Rc4Type;
typedef enum {UNSUPPORTED_DES = -1, DES = 12, DES_EXPORT_40 = 13, DES3_192 = 14} DesType;

/*! \enum RsaBlockType Public BT1 Private BT2*/
typedef enum
{ BT1 = 0, BT2 = 1 }
RsaBlockType;

/*! \enum Csp1RequestType Blocking or NonBlocking */
typedef enum
{ CAVIUM_BLOCKING = 0, CAVIUM_NON_BLOCKING = 1, CAVIUM_SIGNAL = 2 }
Csp1RequestType, n1_request_type;

/*! \enum SslVersion SSL/TLS protocol version */
typedef enum {VER_TLS = 0, VER3_0 = 1, VER2_0 = 2} SslVersion;

/*! \enum master_secretReturn  SSL/TLS Record Type */
typedef enum {NOT_RETURNED = 0, RETURN_ENCRYPTED = 1} master_secretReturn;

/*! \enum HashType Sha-1 or MD-5 */
typedef enum
{ SHA1_TYPE = 2, MD5_TYPE = 1 }
HashType;

/*! \enum ContextType Context Type to allocate or deallocate */
typedef enum
{ CONTEXT_SSL = 0, CONTEXT_IPSEC = 2 }
ContextType;

/*! \enum RsaModExType Normal or Chinese Remainder Theorem */
typedef enum
{ NORMAL_MOD_EX = 0, CRT_MOD_EX = 1 }
RsaModExType;

/*! \enum KeyMaterialLocation Where Key Material is stored */
typedef enum {INTERNAL_SRAM = 0, HOST_MEM = 1, LOCAL_DDR = 2} KeyMaterialLocation;

/*! \enum Csp1DmaMode Direct or Scatter Gather*/
typedef enum
{ CAVIUM_DIRECT = 0, CAVIUM_SCATTER_GATHER = 1 }
Csp1DmaMode, n1_dma_mode, DmaMode;

/*! \enum master_secretInput  Master secret location */
typedef enum {READ_FROM_CONTEXT = 0, INPUT_ENCRYPTED = 1} master_secretInput;

/*! \enum SslPartyType server or client */
typedef enum {SSL_SERVER = 0, SSL_CLIENT = 1} SslPartyType;

/*! \enum MessageType  SSL/TLS Record Type */
typedef enum {CHANGE_CIPHER_SPEC = 0, ALERT = 1, HANDSHAKE = 2, APP_DATA = 3} MessageType;

/*+****************************************************************************/
/*!\ingroup MISC
 * Csp1Initialize
 *
 * Prepares the aplication.
 *
 * \input dma_mode  CAVIUM_DIRECT or CAVIUM_SCATTER_GATHER
 * \if NPLUS
 * \input microcode_type \arg ADMIN_IDX      NPLUS microcode and core administration.
 *                                     nplus_init uses this microcode type.
 *                                     
 *          \arg SSL_MLM_IDX    SSL 2.x microcode in fast path mode.
 *
 *          \arg SSL_SPM_IDX    SSL 2.x microcode in slow path mode.
 * 
 *          \arg IPSEC_MLM_IDX  IPsec2.x microcode in fast path mode.
 *
 *          \arg IPSEC_SPM_IDX  IPsec2.x microcode on slow path mode. 
 * \endif
 * 
 * \return completion code
 * \retval SUCCESS 0 
 * \retval FAILURE #Csp1ErrorCodes
 */
/*-***************************************************************************/
uint32 Csp1Initialize(Csp1DmaMode dma_mode);

/*+****************************************************************************/
/*!\ingroup MISC
 * Csp1Shutdown
 *
 * Cleanup the driver.
 *
 * \return Status
 * \retval SUCESS 0           
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
uint32 Csp1Shutdown(void);

/*+****************************************************************************/
/*! \ingroup MISC
 * Csp1AllocContext
 *
 * Allocates a context segment (in the local DDR DRAM or the host memory 
 * depending on the system) and returns its handle that will be passed to the 
 * processor in the final 8 bytes of the request as Cptr.
 *
 * \input  cntx_type     CONTEXT_SSL or CONTEXT_IPSEC
 *
 * \output context_handle pointer to 8-byte address of the context for use by 
 *      the Cavium processor
 * \output cntx_type       type of context SSL IPSEC allocated??
 *
 * \return Status 
 * \retval SUCCESS 0 
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
uint32 Csp1AllocContext(ContextType cntx_type, uint64 *context_handle);

/*+****************************************************************************/
/*! \ingroup MISC
 * Csp1FreeContext
 *
 * Free a context segment for use by another SSL connection.
 *
 * \input cntx_type       CONTEXT_SSL or CONTEXT_IPSEC
 * \input context_handle    8-byte address of the context for use by 
 *             the Cavium processor
 *
 * \return Status
 * \retval SUCCESS 0 
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
uint32 Csp1FreeContext(ContextType cntx_type, uint64 context_handle);

/*+****************************************************************************
 *
 * Csp1WriteContext
 *
 * Write data to context memory.
 *
 * Input
 *    request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *      context_handle = 64-bit context handle pointer (context_handle%8=0)
 *      length = size of the data in bytes (8<=length<=1024,length%8=0)
 *      data = pointer to length bytes of data to be stored
 *
 * output
 *    request_id = Unique ID for this request.
 *
 * Return Value
 *    0  = success 
 *    >0 = failure or pending
 *    see error_codes.txt
 *
 *-***************************************************************************/
uint32 Csp1WriteContext(
	n1_request_type request_type,
       uint64 context_handle, 
       uint16 length,
       uint8 *data,
       uint32 *request_id);

/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1AllocKeyMem
 *
 * Acquires the handle to a key memory segment and returns a handle.
 *
 * \input key_material_loc   INTERNAL_SRAM, HOST_MEM, or LOCAL_DDR
 *
 * \output key_handle      pointer to 8-byte handle to key memory segment
 *
 * \return Completion Code
 * \retval SUCCESS 0 
 * \retval COMPLETION_CODE #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
uint32 Csp1AllocKeyMem(KeyMaterialLocation key_material_loc, uint64 *key_handle);

/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1FreeKeyMem
 *
 * Free a key memory segment.
 *
 * \input key_handle   8-byte handle to key memory segment
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
uint32 Csp1FreeKeyMem(uint64 key_handle);

/*+****************************************************************************/
/*! \ingroup MISC
 * Csp1StoreKey
 *
 * Store a key to memory segment indicated by key handle.
 *
 * \input key_handle     8-byte handle to key memory segment
 * \input length    size of key in bytes
 * \input key       pointer to key
 * \input mod_ex_type    NORMAL_MOD_EX or CRT_MOD_EX
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
uint32 Csp1StoreKey (uint64 * key_handle, uint16 length, uint8 * key, RsaModExType mod_ex_type);

/*+****************************************************************************
 *
 * Csp1RsaServerFullRc4
 *
 * Does a full handshake on the server with RSA <= 1024. This entry point 
 * handles all the RC4 cases. The handshake message data for this request 
 * should include all handshake message data after (and including) the client 
 * hello message up until (but not including) the first finished message. 
 *
 * Input
 *		request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *		context_handle = 64-bit byte-pointer to context (context_handle%8=0)
 *		key_handle = pointer to 64-bit key memory handle
 *		hash_type = MD5_TYPE or SHA1_TYPE
 *		ssl_version = VER3_0 or VER_TLS
 *		rc4_type = RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 *		master_secret_ret = NOT_RETURNED or RETURN_ENCRYPTED
 *		modlength = size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 *	#ifdef MC2
 *		encrypt_premaster_secret = pointer to modlength-byte value.
 *	#else
 *		encrypt_premaster_secret = pointer to modlength-byte value in integer format
 *	#endif
 *		client_random = pointer to 32 bytes of random data
 *		server_random = pointer to 32 bytes of random data
 *		handshake_length = size in bytes of the handshake message data
 *		handshake = pointer to the handshake message data
 *
 * Output
 *		client_finished_message = pointer to encrypted part of client finished message 
 *		server_finished_message = pointer to encrypted part of server finished message 
 *		encrypt_master_secret = (master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *									returned encrypted master secret : don't care
 *		request_id = Unique ID for this request
 *
 * Return Value
 *		0  = success 
 *		>0 = failure or pending
 *		see error_codes.txt
 *-***************************************************************************/
uint32 Csp1RsaServerFullRc4 (
	n1_request_type request_type,
	uint64 context_handle,
	uint64 * key_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretReturn master_secret_ret,
	uint16 modlength,
	uint8 * encrypt_premaster_secret,
	uint8 * client_random,
	uint8 * server_random,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * client_finished_message,
	uint8 * server_finished_message,
	uint8 * encrypt_master_secret, 
	uint32 *request_id);

/*+****************************************************************************
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
 * Input
 *	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *	context_handle = 64-bit pointer to context (context_handle%8=0)
 *	hash_type = MD5_TYPE or SHA1_TYPE
 *	ssl_version = VER3_0 or VER_TLS
 *	rc4_type = RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 *	master_secret_inp = READ_FROM_CONTEXT or INPUT_ENCRYPTED
 *	client_random = pointer to 32 bytes of random data
 *	server_random = pointer to 32 bytes of random data
 *	encrypt_master_secret = pointer to 48-byte secret
 *	handshake_length = size in bytes of the handshake message data
 *	handshake = pointer to the handshake message data
 *
 * Output
 *	client_finished_message = pointer to encrypted part of client finished message 
 *	server_finished_message = pointer to encrypted part of server finished message 
 *	request_id = Unique ID for this request
 *
 * Return Value
 *	0  = success 
 * >0 = failure or pending
 * see error_codes.txt
 *
 *-***************************************************************************/
uint32 Csp1ResumeRc4 (
	n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretInput master_secret_inp,
	uint8 * client_random,
	uint8 * server_random,
	uint8 * encrypt_master_secret,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * client_finished_message,
	uint8 * server_finished_message, 
	uint32 * request_id);

/*+****************************************************************************
 *
 * Csp1DecryptRecordRc4
 *
 * Input
 *	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *	context_handle = 64-bit pointer to context (context_handle%8=0)
 *	hash_type = MD5_TYPE or SHA1_TYPE 
 *	ssl_version = VER3_0 or VER_TLS
 *	message_type = CHANGE_CIPHER_SPEC, ALERT, HANDSHAKE, or APP_DATA
 *	record_length = size of record in bytes (0<=length<=2^14+1024)
 *	record = pointer to length-byte encrypted part of record 
 *
 * Output
 *	message = pointer to (record length - hash size) bytes 
 *	request_id = Unique ID for this request
 *
 * Return Value
 *	0  = success 
 * >0 = failure or pending
 * see error_codes.txt
 *
 *-***************************************************************************/
uint32 Csp1DecryptRecordRc4 (
	n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	SslPartyType ssl_party,
	MessageType message_type,
	uint16 record_length,
	uint8 * record, 
	uint8 * message, 
	uint32 * request_id);

/*+****************************************************************************
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
 * Input
 *	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *	context_handle = 64-bit pointer to context (context_handle%8=0)
 *	hash_type = MD5_TYPE or SHA1_TYPE
 *	ssl_version = VER3_0 or VER_TLS
 *	rc4_type = RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 *	master_secret_ret = NOT_RETURNED or RETURN_ENCRYPTED
 *	pre_master_length = size of premaster secret in bytes 
 *		(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 TLS: 16<=modlength<=128, modlength%16=0)
 *	client_random = pointer to 32 bytes of random data
 *	server_random = pointer to 32 bytes of random data
 *	handshake_length = size in bytes of the handshake message data
 *	handshake = pointer to the handshake message data
 *
 * Output
 *	client_finished_message = pointer to encrypted part of client finished message 
 *	server_finished_message = pointer to encrypted part of server finished message 
 *	encrypt_master_secret = (master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *		returned encrypted master secret : don't care
 *	request_id = Unique ID for this request
 *
 * Return Value
 *	0  = success 
 * >0 = failure or pending
 * see error_codes.txt
 *
 *-***************************************************************************/
uint32 Csp1OtherFullRc4 (
	n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretReturn master_secret_ret,
	uint16 pre_master_length,
	uint8 * client_random,
	uint8 * server_random,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * client_finished_message,
	uint8 * server_finished_message,
	uint8 * encrypt_master_secret, 
	uint32 * request_id);

/*+****************************************************************************
 *
 * Csp1EncryptRecordRc4
 *
 * Input
 *	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *	context_handle = 64-bit pointer to context (context_handle%8=0)
 *	hash_type = MD5_TYPE or SHA1_TYPE 
 *	ssl_version = VER3_0 or VER_TLS
 *	message_type = CHANGE_CIPHER_SPEC, ALERT, HANDSHAKE, or APP_DATA
 *	message_length = size of message in bytes (0<=length<=2^14+1024)
 *	message = pointer to length-byte message 
 *
 * Output
 *	record = pointer to (length + hash_size) bytes of encrypted record 
 *	request_id = Unique ID for this request
 *
 * Return Value
 *	0  = success 
 * >0 = failure or pending
 * see error_codes.txt
 *
 *-***************************************************************************/
uint32 Csp1EncryptRecordRc4 (
	n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	SslPartyType ssl_party,
	MessageType message_type,
	uint16 message_length,
	uint8 * message, 
	uint8 * record, 
	uint32 * request_id);

/*+****************************************************************************
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
 * Input
 *		request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *		context_handle = 64-bit byte-pointer to context (context_handle%8=0)
 *		key_handle = pointer to 64-bit key memory handle
 *		hash_type = MD5_TYPE or SHA1_TYPE
 *		ssl_version = VER3_0 or VER_TLS
 *		rc4_type = RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 *		master_secret_ret = NOT_RETURNED or RETURN_ENCRYPTED
 *		modlength = size of RSA operation in bytes (64<=modlength<=256, modlength%8=0)
 *		encrypt_premaster_secret = pointer to modlength-byte value in integer format
 *		client_random = pointer to 32 bytes of random data
 *		server_random = pointer to 32 bytes of random data
 *		handshake_length = size in bytes of the handshake message data
 *		handshake = pointer to the handshake message data
 *
 * Output
 *		verify_data = pointer to 36 bytes of verify data 
 *		encrypt_master_secret = (master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *						returned encrypted master secret : don't care
 *		request_id = Unique ID for this request
 *
 * Return Value
 *		0  = success 
 *		>0 = failure or pending
 *		see error_codes.txt
 *
 *-***************************************************************************/
uint32 Csp1RsaServerVerifyRc4 (
	n1_request_type request_type,
	uint64 context_handle,
	uint64 * key_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretReturn master_secret_ret,
	uint16 modlength,
	uint8 * encrypt_premaster_secret,
	uint8 * client_random,
	uint8 * server_random,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * verify_data,
	uint8 * encrypt_master_secret, 
	uint32 * request_id);

/*+****************************************************************************
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
 * Input
 *	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *	context_handle = 64-bit pointer to context (context_handle%8=0)
 *	hash_type = MD5_TYPE or SHA1_TYPE
 *	ssl_version = VER3_0 or VER_TLS
 *	handshake_length = size in bytes of the handshake message data
 *	handshake = pointer to the handshake message data
 *
 * Output
 *	client_finished_message = pointer to encrypted part of client finished message 
 *	server_finished_message = pointer to encrypted part of server finished message 
 *	request_id = Unique ID for this request
 *
 * Return Value
 *	0  = success 
 * >0 = failure or pending
 * see error_codes.txt
 *
 *-***************************************************************************/
uint32 Csp1FinishedRc4Finish (
	n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * client_finished_message,
	uint8 * server_finished_message, 
	uint32 * request_id);

/*+****************************************************************************
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
 * Input
 *	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *	context_handle = 64-bit pointer to context (context_handle%8=0)
 *	hash_type = MD5_TYPE or SHA1_TYPE
 *	ssl_version = VER3_0 or VER_TLS
 *	rc4_type = RC4_128, RC4_EXPORT_40, or RC4_EXPORT_56
 *	master_secret_ret = NOT_RETURNED or RETURN_ENCRYPTED
 *	pre_master_length = size of premaster secret in bytes 
 *		(SSLv3: 4<=modlength<=256, modlength%4=0; 
 *		 TLS: 16<=modlength<=128, modlength%16=0)
 *	client_random = pointer to 32 bytes of random data
 *	server_random = pointer to 32 bytes of random data
 *	handshake_length = size in bytes of the handshake message data
 *	handshake = pointer to the handshake message data
 *
 * Output
 *	verify_data = pointer to 36 bytes of verify data 
 *	encrypt_master_secret = (master_secret_ret == RETURN_ENCRYPTED) ? pointer to
 *		returned encrypted master secret : don't care
 *	request_id = Unique ID for this request
 *
 * Return Value
 *	0  = success 
 * >0 = failure or pending
 * see error_codes.txt
 *
 *-***************************************************************************/
uint32 Csp1OtherVerifyRc4 (
	n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretReturn master_secret_ret,
	uint16 pre_master_length,
	uint8 * client_random,
	uint8 * server_random,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * verify_data,
	uint8 * encrypt_master_secret, 
	uint32 * request_id);

/*+****************************************************************************
 *
 * Csp1Pkcs1v15CrtEnc
 *
 * Creates PKCS#1v1.5 container using the Chinese Remainder Theorem.
 * The combination of block type BT2 and CRT may produce unpredictable results.
 *
 * Input
 *      request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *   #ifdef MC2
 *      block_type = type of PKCS#1v1.5 padding (BT1 only)
 *      modlength = size of modulus in bytes
 *      datalength = size of input data in bytes.
 *      Q = prime factor of RSA modulus
 *      Eq = exponent mod(Q-1)
 *      P = prime factor of RSA modulus
 *      Ep = exponent mod(P-1)
 *      iqmp = (Q^-1) mod P
 *   #else
 *      result_location = CONTEXT_PTR or RESULT_PTR 
 *      context_handle = 64-bit pointer to context (context_handle%8=0)
 *      key_material_input = KEY_HANDLE or INPUT_DATA
 *      key_handle = 64-bit handle for key memory 
 *      block_type = type of PKCS#1v1.5 padding (BT1 only)
 *      modlength = size of modulus in bytes (modlength%8=0, 24<modlength<=256)
 *      Q = (key_material_input == INPUT_DATA) ? prime factor of RSA modulus : don't care
 *      Eq = (key_material_input == INPUT_DATA) ? exponent mod(Q-1) : don't care
 *      P = (key_material_input == INPUT_DATA) ? prime factor of RSA modulus : don't care
 *      Ep = (key_material_input == INPUT_DATA) ? exponent mod(P-1) : don't care
 *      iqmp = (key_material_input == INPUT_DATA) ? (Q^-1) mod P : don't care
 *      length = size of the input value 
 *   #endif
 *      data = pointer to length-byte value to be exponentiated
 *
 * Output
 *   #ifdef MC2
 *      result = pointer to modlength bytes of output
 *   #else
 *      result = (result_location == RESULT_PTR) ? (pointer to modlength bytes of output : don't care
 *   #endif
 *      request_id = Unique ID for this request.
 * 
 *
 * Return Value
 *      0  = success 
 *      >0 = failure or pending
 *      see error_codes.txt
 *
 *-***************************************************************************/
uint32 Csp1Pkcs1v15CrtEnc(
	n1_request_type request_type,
	RsaBlockType block_type,
	uint16 modlength, 
	uint16 datalength, 
	uint8 *Q, 
	uint8 *Eq, 
	uint8 *P, 
	uint8 *Ep, 
	uint8 *iqmp, 
	uint8 *data,
	uint8 *result,
	uint32 *request_id);


/******************************************************************************
  * 
  * Kernel API
  *
  *****************************************************************************/
  uint32 Kernel_Csp1WriteContext(n1_request_type request_type,
       uint64 context_handle, 
       uint16 length,
       uint8 *data,
       uint32 *request_id,
       void (*call_back)(int, void*),
       void *connect);
  uint32 Kernel_Csp1Pkcs1v15CrtEnc(n1_request_type request_type,
	RsaBlockType block_type,
	uint16 modlength, 
	uint16 datalength, 
	uint8 *Q, 
	uint8 *Eq, 
	uint8 *P, 
	uint8 *Ep, 
	uint8 *iqmp, 
	uint8 *data,
	uint8* result,
	uint32 *request_id,
	void (*call_back)(int, void*),
	void *connect);
uint32 Kernel_Csp1RsaServerFullRc4(n1_request_type request_type,
	uint64 context_handle,
	uint64 * key_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretReturn master_secret_ret,
	uint16 modlength,
	uint8 * encrypt_premaster_secret,
	uint8 * client_random,
	uint8 * server_random,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * client_finished_message,
	uint8 * server_finished_message,
	uint8 * encrypt_master_secret, 
	uint32 *request_id,
	void (*call_back)(int, void*),
	void *connect);
uint32 Kernel_Csp1RsaServerVerifyRc4 (n1_request_type request_type,
	uint64 context_handle,
	uint64 * key_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretReturn master_secret_ret,
	uint16 modlength,
	uint8 * encrypt_premaster_secret,
	uint8 * client_random,
	uint8 * server_random,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * verify_data,
	uint8 * encrypt_master_secret, uint32 * request_id,
	void (*call_back)(int, void*),
	void *connect);			  

uint32 Kernel_Csp1OtherFullRc4 (n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretReturn master_secret_ret,
	uint16 pre_master_length,
	uint8 * client_random,
	uint8 * server_random,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * client_finished_message,
	uint8 * server_finished_message,
	uint8 * encrypt_master_secret, uint32 * request_id,
	void (*call_back)(int, void*),
	void *connect);
uint32 Kernel_Csp1OtherFullRc4 (n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretReturn master_secret_ret,
	uint16 pre_master_length,
	uint8 * client_random,
	uint8 * server_random,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * client_finished_message,
	uint8 * server_finished_message,
	uint8 * encrypt_master_secret, uint32 * request_id,
	void (*call_back)(int, void*),
	void *connect);		  
uint32 Kernel_Csp1OtherVerifyRc4 (n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretReturn master_secret_ret,
	uint16 pre_master_length,
	uint8 * client_random,
	uint8 * server_random,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * verify_data,
	uint8 * encrypt_master_secret, uint32 * request_id,
	void (*call_back)(int, void*),
	void *connect);

uint32 Kernel_Csp1FinishedRc4Finish (n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * client_finished_message,
	uint8 * server_finished_message, uint32 * request_id,
	void (*call_back)(int, void*), 
	void* connect);	

uint32 Kernel_Csp1ResumeRc4 (n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	Rc4Type rc4_type,
	master_secretInput master_secret_inp,
	uint8 * client_random,
	uint8 * server_random,
	uint8 * encrypt_master_secret,
	uint16 handshake_length,
	uint8 * handshake,
	uint8 * client_finished_message,
	uint8 * server_finished_message, uint32 * request_id,
	void (*call_back)(int, void*),
	void* connect);			   		

uint32 Kernel_Csp1EncryptRecordRc4 (n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	SslPartyType ssl_party,
	MessageType message_type,
	uint16 message_length,
	uint8 * message, uint8 * record, uint32 * request_id,
	void (*call_back)(int, void*),
	void *connect);

uint32 Kernel_Csp1DecryptRecordRc4 (n1_request_type request_type,
	uint64 context_handle,
	HashType hash_type,
	SslVersion ssl_version,
	SslPartyType ssl_party,
	MessageType message_type,
	uint16 record_length,
	uint8 * record, uint8 * message, uint32 * request_id,
	void (*call_back)(int, void*),
	void *connect);			  		   	

uint32 Kernel_Csp1Pkcs1v15Enc(
	n1_request_type request_type,
    RsaBlockType block_type,
    uint16 modlength, 
	uint16 explength,
	uint16 datalength, 
	uint8 *modulus, 
	uint8 *exponent, 
	uint8 *data,
	uint8 *result,
	uint32 *request_id,
	void (*call_back)(int, void *),
	void* connect);

uint32 Kernel_Csp1Pkcs1v15Dec(
	n1_request_type request_type,
	RsaBlockType block_type,
	uint16 modlength, 
	uint16 explength,
	uint8 *modulus, 
	uint8 *exponent, 
	uint8 *data,
	uint16 *out_length,
	uint8 *result,
	uint32 *request_id,
	void (*call_back)(int, void*),
	void *connect);

#endif

