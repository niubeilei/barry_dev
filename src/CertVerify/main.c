////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "common.h"
#include "asn1.h"

#include "aosUtil/Memory.h"
#include "x509.h"
#include "x509_ocsp.h"
#include "aosBase64.h"
#include "jns_ocsp.h"


static char *ocsp_req_file = "ocsp_req.der";
extern int base64_decode(const unsigned char *data, int len, char *dest);

#define COMMAND_NUM	8
static char *comm_name[COMMAND_NUM] = {
	"base64_decode",
	"cert_decode",
	"ocsp_request",
	"ocsp_response_decode",
	"ocsp_check",
	"crl_decode",
	"prvkey_decode",
	"jns_ocsp",
};

typedef enum {
	COMM_ERROR = -1,
	COMM_BASE64_DECODE = 0,
	COMM_CERT_DECODE,
	COMM_OCSP_REQUEST,
	COMM_OCSP_RESOPNSE_DECODE,
	COMM_OCSP_CHECK,
	COMM_CRL_DECODE,
	COMM_PRVKEY_DECODE,
	COMM_JNS_OCSP,
}	BASE_COMMAND;

static BASE_COMMAND get_command(char *comm) {
	int i;

	for (i = 0; i < COMMAND_NUM; i++)
		if (strcmp(comm_name[i], comm) == 0)
			return (BASE_COMMAND)i;

	return COMM_ERROR;
}

static int Usage(char *comm) {
	int i;

	printf("%s sub_command other_parameter\n", comm);
	printf("\tsub_command:\n");
	for (i = 0; i < COMMAND_NUM; i++)
		printf("\t\t%s\n", comm_name[i]);

	printf("For Example:\n");
	printf("\t%s base64_decode bas64_file\n", comm);
	printf("\t%s cert_decode cert_file\n", comm);
	printf("\t%s ocsp_request <out file> <ca file> <cert file>\n", comm);
	printf("\t%s ocsp_response_decode <ocsp response file>\n", comm);
	printf("\t%s ocsp_check ip port <ca file> <cert file> ...\n", comm);
	printf("\t%s crl_decode <crl file>\n", comm);
	printf("\t%s prvkey_decode <prveky file>\n", comm);
	printf("\t%s jns_ocsp <serial number> <out file>\n", comm);
	printf("\n\n");
}

X509_CERT_INFO *
parse_cert_file(char *filename)  {
	DATA_OBJECT *object = NULL;
	X509_CERT_INFO * cert = NULL;

	if (!filename)
		return NULL;

	object = file_to_object(filename);
	if (!object)
		goto error;

	cert = x509_parse_cert_object(object);
	if (!cert)
		goto error;

	return cert;

error:
	if (object)	asn_free_object(object);
	return NULL;
}

void show_DN(DN_attributes_t *dn) 
{
/*
	if (dn->E_len) {
		printf("\t\tE = ");
		x509_printc(dn->E, dn->E_len);
	}
	if (dn->CN_len) {
		printf("\t\tCN= ");
		x509_printc(dn->CN, dn->CN_len);
	}
	if (dn->OU_len) {
		printf("\t\tOU= ");
		x509_printc(dn->OU, dn->OU_len);
	}
	if (dn->O_len) {
		printf("\t\tO = ");
		x509_printc(dn->O, dn->O_len);
	}
	if (dn->L_len) {
		printf("\t\tL = ");
		x509_printc(dn->L, dn->L_len);
	}
	if (dn->S_len) {
		printf("\t\tS = ");
		x509_printc(dn->S, dn->S_len);
	}
	if (dn->C_len) {
		printf("\t\tC = ");
		x509_printc(dn->C, dn->C_len);
	}
*/
}

void
x509_show_certinfo(X509_CERT_INFO *certinfo) {

	if (!certinfo)
		return;

	printf("X509 Certificate information:\n");
	printf("\tBytes Length: %d\n", (int)certinfo->object->length);

	printf("\tVersion:V%d\n\n", (int)certinfo->version);

	printf("\tSerial Number: ");
	x509_printx(certinfo->serial_number.data, certinfo->serial_number.len);

	printf("\tSignature Algorithm: %d:%d\n",
			(int)certinfo->sig_alg.algorithm,
			(int)certinfo->sig_alg.sub_algorithm);


	printf("\tIssuer:\n");
	show_DN(&certinfo->issuer);

	printf("\tIssuer HASH: ");
	x509_printx(certinfo->issuer.hash, 20);

	printf("\tValidity:\n");
	printf("\t\tnotBefore(local):%s", ctime(&certinfo->validity.notbefore));
	printf("\t\tnotAfter(local) :%s", ctime(&certinfo->validity.notafter));

	printf("\tSubject:\n");
	show_DN(&certinfo->subject);

	printf("\tSubject HASH: ");
	x509_printx(certinfo->subject.hash, 20);

	printf("\tSubject public key information:\n");
	printf("\t\tPublic Key Algorithm: %d:%d\n",
			certinfo->pubkey.alg.algorithm,
			certinfo->pubkey.alg.sub_algorithm);
	printf("\t\tRSA Public Key:\n");
	printf("\t\t\tModulus:\n");
	x509_printx_format(certinfo->pubkey.modulus.data,
			certinfo->pubkey.modulus.len, "\t\t\t\t");
	printf("\t\t\tExponent: \n");
	x509_printx_format(certinfo->pubkey.public_exponent.data,
			certinfo->pubkey.public_exponent.len, "\t\t\t\t");

	printf("\tPublic key HASH:\t");
	x509_printx(certinfo->pubkey.hash, 20);

	printf("\tSignature Algorithm: %d:%d\n", certinfo->signature_algorithm.algorithm,
			certinfo->signature_algorithm.sub_algorithm);

	x509_printx_format(certinfo->signature.data, certinfo->signature.len, "\t\t");

	return;
}

static DATA_OBJECT * ocsp_request(char *ca, char **user_certs, int num) {
	X509_CERT_INFO  *cainfo;
	X509_CERT_INFO  *userinfo = NULL;
	DATA_OBJECT	*ocsp_object = NULL;
	int i;

	if (!ca || !user_certs || num <= 0)
		return NULL;

	cainfo = parse_cert_file(ca);
	if (cainfo < 0) {
		printf("Decode CA certificate failed\n");
		return NULL;
	}

	for (i = 0; i < num; i++) {
		X509_CERT_INFO * cert;

		cert= parse_cert_file(user_certs[i]);
		if (cert == NULL) {
			printf("Decode cert %s Failed\n", user_certs[i]);
			continue;
		}
		cert->next = userinfo;
		userinfo = cert;
	}

	APP_OCSP_REQUEST ocsp_req = {
		userinfo,
		cainfo,
		"\x01\x02\x03\x04\x01\x02\x03\x04\x01\x02\x03\x04\x01\x02\x03\x04",
		16,
		0,
		NULL
	};

	ocsp_object = x509_encode_ocsp_request(&ocsp_req);
	if (ocsp_object == NULL) {
		printf(" x509_ocsp_request_http failed\n");
		x509_free_certinfo(cainfo);
		x509_free_certinfo(userinfo);
		return NULL;
	}

	x509_free_certinfo(cainfo);
	x509_free_certinfo(userinfo);
	return ocsp_object;
}

int decode_base64(char *filename) {
	DATA_OBJECT *object;
	BYTE	buffer[2048];
	int len;

	object=file_to_object(filename);
	if (!object) {
		printf("Failed open file: %s\n", filename);
		return -1;
	}

	printf("Base64 file length:%d\n", object->length);

	len = base64_decode((BYTE *)object->data, object->length, (char *)buffer);
	if (len < 0) {
		printf("base64_decode failed\n");
		return -1;
	}  else
		x509_printx(buffer, len);

	asn_free_object(object);

	return 0;
}

void show_ocsp_certid(OCSP_CERTID *certid) {
	printf("\t\tHASH algorithm: %d %d\n", certid->hash_alg.algorithm, certid->hash_alg.sub_algorithm);
	printf("\t\tIssuer DN hash:     ");
	x509_printx(certid->issuer_dn_hash, certid->issuer_dn_hash_len);
	printf("\t\tIssuer PubKey hash: ");
	x509_printx(certid->issuer_pubkey_hash, certid->issuer_pubkey_hash_len);
	printf("\t\tCert serial number:");
	x509_printx(certid->serial_number.data, certid->serial_number.len);
}


void x509_show_ocsp_response(OCSP_RESPONSE *respinfo) {
	OCSP_SINGLE_RESPONSE   *osr;

	if (!respinfo)
		return;

	printf("OCSP response information:\n");

	if (respinfo->status == OCSP_RESP_SUCCESSFUL)
		printf("\tstatus:	Success\n");
	else
		printf("\tstatus:	error\n");

	if (respinfo->resp_type == 1)
		printf("\tresponse type: id-pkix-ocsp-basic\n");
	else
		printf("\nresponse type: not known type, maybe id-pkix-ocsp\n");

	printf("\tversion: %d\n", respinfo->version);

	printf("\tResponderID:\n");
	show_DN(&respinfo->resp_certid);

	printf("\tproducedAt: %s", ctime(&respinfo->producted_time));

	printf("\tResponse list:\n");
	osr = respinfo->single_response_list;
	while (osr) {
		show_ocsp_certid(&osr->certid);

		printf("\t\tstatus: ");
		switch(osr->status) {
		case OCSP_CERTSTATUS_GOOD:
			printf("Good\n");
			break;
		case OCSP_CERTSTATUS_REVOKED:
			printf("Revoked\n");
			break;
		case OCSP_CERTSTATUS_UNKNOWN:
			printf("Unknown\n");
			break;
		default:
			printf("Error\n");
			break;
		}

		if (osr->status == OCSP_CERTSTATUS_REVOKED) {
			printf("\t\t\tRevoked time :%s", ctime(&osr->time));
			printf("Reason: %d\n", osr->reason);
		}

		printf("\t\tthisUpdate: %s", ctime(&osr->this_update));
		if (osr->next_update != 0)
			printf("\t\tnextUpdate: %s", ctime(&osr->next_update));
		printf("\t\t-------------------------------------------------------------------------------\n");
		osr = osr->next;
	}

	printf("\tsignatureAlgorithm: %d %d\n", respinfo->sig_alg.algorithm, respinfo->sig_alg.sub_algorithm);
	printf("\tsign:\n");
	x509_printx_format(respinfo->signature.data, respinfo->signature.len, "\t\t");

	printf("\n");
	x509_show_certinfo(&respinfo->resp_cert);

}

void show_prvkey_info(rsa_privatekey_t *prvkey) {
	printf("private key information:\n");

	printf("\tversion:%ld\n", prvkey->version);

	printf("\tmodulus: %d\n", prvkey->modulus.len);
	x509_printx_format(prvkey->modulus.data, prvkey->modulus.len, "\t\t");

	printf("\tpublic_exponent: %d\n", prvkey->public_exponent.len);
	x509_printx_format(prvkey->public_exponent.data, prvkey->public_exponent.len, "\t\t");

	printf("\tprivate_exponent: %d\n", prvkey->private_exponent.len);
	x509_printx_format(prvkey->private_exponent.data, prvkey->private_exponent.len, "\t\t");

	printf("\tprime1: %d\n", prvkey->prime1.len);
	x509_printx_format(prvkey->prime1.data, prvkey->prime1.len, "\t\t");

	printf("\tprime2: %d\n", prvkey->prime2.len);
	x509_printx_format(prvkey->prime2.data, prvkey->prime2.len, "\t\t");

	printf("\texponent1: %d\n", prvkey->exponent1.len);
	x509_printx_format(prvkey->exponent1.data, prvkey->exponent1.len, "\t\t");

	printf("\texponent2: %d\n", prvkey->exponent2.len);
	x509_printx_format(prvkey->exponent2.data, prvkey->exponent2.len, "\t\t");

	printf("\tcoefficient: %d\n", prvkey->coefficient.len);
	x509_printx_format(prvkey->coefficient.data, prvkey->coefficient.len, "\t\t");
}

int main(int argc, char **argv) {
	int  ret, cert_len, i;


	DATA_OBJECT	*crl_object = NULL;
	DATA_OBJECT 	*resp_object;


	int length, len;

	BASE_COMMAND comm;

	printf("argc:%d\n", argc);
	if (argc < 3) {
		Usage(argv[0]);
		exit(-1);
	}

	comm = get_command(argv[1]);
	printf("command:%d\n", comm);
	switch (comm) {
		case COMM_BASE64_DECODE: {
			decode_base64(argv[2]);
			break;
		}
		case COMM_CERT_DECODE:	{
			X509_CERT_INFO *pcert;

			pcert = parse_cert_file(argv[2]);
			if (!pcert) {
				printf("Decode Certificate file %s failed\n", argv[2]);
				exit(-1);
			}
			x509_show_certinfo(pcert);
			x509_free_certinfo(pcert);
			break;
		}
		case COMM_OCSP_REQUEST: {
			DATA_OBJECT	*ocsp_object;

			if (argc < 5) {
				Usage(argv[0]);
				exit(-1);
			}

			ocsp_object = ocsp_request(argv[3], &argv[4], argc - 4);
			if (!ocsp_object) {
				printf("Encode OCSP request Failed\n");
				exit(-1);
			}
			printf("OCSP_request objectlength:%d\n", asn_object_length(ocsp_object));
			printf("Saved to %s\n", argv[2]);

			object_to_file(ocsp_object, ocsp_req_file);
			break;
		}
		case COMM_OCSP_RESOPNSE_DECODE: {
			DATA_OBJECT *object;
			OCSP_RESPONSE *respinfo;

			object = file_to_object(argv[2]);
			if (!object) {
				printf("Open OCSP response file %s failed\n",argv[2]);
				exit(-1);
			}

			respinfo = (OCSP_RESPONSE *)aos_zmalloc(sizeof(OCSP_RESPONSE));
			if (!respinfo) {
				asn_free_object(object);
				printf("Malloc OCSP_RESPONSE failed\n");
				exit(-1);
			}
			ret = x509_decode_ocsp_response(object, respinfo);
			if (ret < 0) {
				asn_free_object(object);
				ocsp_free_response(respinfo);
				printf("[x509_decode_ocsp_response] failed\n");
				exit(-1);
			} else
				printf("[x509_decode_ocsp_response] success\n");


			x509_show_ocsp_response(respinfo);

			ocsp_free_response(respinfo);
			break;
		}
		case COMM_OCSP_CHECK: {
			DATA_OBJECT	*ocsp_object;
			DATA_OBJECT	*resp_object = NULL;
			OCSP_RESPONSE	response_info, *respinfo = &response_info;
			struct sockaddr_in dest;
			int sock;;
			int port;
			char * http_post_of_ocsp = "POST / HTTP/1.1\r\nContent-Type: application/ocsp-request\r\n"
				"Content-Length: %d\r\n\r\n";
			char buf[2048];

			if (argc < 6) {
				Usage(argv[0]);
				exit(-1);
			}

			inet_aton(argv[2], &dest.sin_addr.s_addr);
			port = strtoul(argv[3], NULL, 0);
			printf("Begin connect to ocsp server....\n");
			sock = socket(AF_INET, SOCK_STREAM, 0);
			if (sock < 0) {
				printf("socket failed\n");
				exit(-1);
			}

			dest.sin_family = AF_INET;
			dest.sin_port = htons(port);

			if (connect(sock, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
				printf("connect failed\n");
				close(sock);
				exit(-1);
			} else
				printf("Connect server ok\n");

			ocsp_object = ocsp_request(argv[4], &argv[5], argc - 5);

			if (ocsp_object == NULL) {
				close(sock);
				printf(" x509_ocsp_request_http failed\n");
				exit(-1);
			}

			//sprintf(buf ,http_post_of_ocsp, asn_object_datalen(ocsp_object));
			//len = send(sock, buf, strlen(buf), 0);
			//printf("http: write sock of %d:%d\n", len, strlen(buf));
			x509_printx(ocsp_object->data, asn_object_datalen(ocsp_object));
			len = send(sock, ocsp_object->data, asn_object_datalen(ocsp_object), 0);
			printf("ocsp: write sock of %d:%d \n", len, asn_object_datalen(ocsp_object));

			len = recv(sock, buf, 2048, 0); //MSG_WAITALL);
			printf("recv: data length:%d\n", len);

			close(sock);
			asn_free_object(ocsp_object);

			ret = x509_ocsp_parse_http((BYTE *)buf, 2048, &resp_object);
			if (ret != 0) {
				printf("x509_ocsp_parse_http failed or not enough data\n");
				exit(-1);
			} else
				printf("ocsp response data length:%d\n", resp_object->length);

			object_to_file(resp_object, "t_resp.der");
			printf("object_to_file finished\n");
			ret = x509_decode_ocsp_response(resp_object,respinfo);
			if (ret < 0)
				printf("decode ocsp response failed\n");
			else	{
				printf("decode ocsp response success\n");
				x509_show_ocsp_response(respinfo);
			}

			asn_free_object(resp_object);

			break;
		}
		case COMM_CRL_DECODE: {
			X509_CRL_INFO	*crlinfo;
			X509_CRL_ENTRY	*entry;
			DATA_OBJECT 	*crl_object;
			int 		ret;

			crlinfo = (X509_CRL_INFO *)aos_zmalloc(sizeof(X509_CRL_INFO));
			if (!crlinfo) {
				printf("Malloc X509_CRL_INFO failed\n");
				exit(-1);
			}

			crl_object = file_to_object(argv[2]);
			if (!crl_object) {
				printf("Open crl file %s failed\n",argv[2]);
				exit(-1);
			}
			printf("CRL file length:%d\n", crl_object->length);
			ret = x509_decode_crlinfo(crl_object, crlinfo);
			if (ret < 0) {
				asn_free_object(crl_object);
				printf("x509_decode_crlinfo failed\n");
				exit(-1);
			} else
				printf("x509_decode_crlinfo OK\n");

			entry = crlinfo->list;
			while(entry) {
				printf("sn:");
				x509_printx(entry->serial_number.data, entry->serial_number.len);
				printf("RevocationDate:%s", ctime(&entry->revocation_date));
				entry = entry->next;
			}
			printf("Total number :%d\n", crlinfo->entry_num);

			x509_free_crlinfo(crlinfo);
			break;
		}
		case COMM_PRVKEY_DECODE: {
			DATA_OBJECT *object;
			rsa_privatekey_t prvkey;

			memset(&prvkey, 0, sizeof(rsa_privatekey_t));
			object = file_to_object(argv[2]);
			if (!object) {
				printf("Open prvkey file %s failed\n",argv[2]);
				exit(-1);
			}

			ret = x509_decode_private_key_info(object, &prvkey);
			if (ret < 0) {
				printf("[x509_decode_private_key_info] failed\n");
				asn_free_object(object);
				exit(-1);
			}

			show_prvkey_info(&prvkey);
			break;
		}
		case COMM_JNS_OCSP: {
			asn_data_t sn;
			JNS_OCSP_REQUEST request;
			DATA_OBJECT *object;
			int len;
			FILE *fp;

			if (argc < 4) {
				Usage(argv[0]);
				exit(-1);
			}

			len = strlen(argv[2]);
			if (len > ASN_SMALL_DATA_LEN) {
				printf("sn is too len\n");
				exit(-1);
			}
			memcpy(sn.data, argv[2], len);
			sn.len = len;
			request.sn = &sn;
			request.time = (UINT32)time(NULL);
			request.nonce_len = 64;
			memset(request.nonce, 0xff, 64);
			object = jns_encode_ocsp_request(&request);
			if (!object) {
				printf("jns_encode_ocsp_request failed\n");
				exit(-1);
			}
			object_to_file(object, argv[3]);
		}

			break;
		default:
			Usage(argv[0]);
			exit(-1);
	}
	return 0;
}

