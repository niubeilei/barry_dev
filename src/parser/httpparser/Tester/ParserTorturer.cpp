////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: ApplicationProxyTester.cpp
//// Description:
////   
////
//// Modification History:
//// 11/16/2006      Created by Chen Ding
////
//////////////////////////////////////////////////////////////////////////////
#include "parser/httpparser/Tester/ParserTorturer.h"

#include "parser/httpparser/aos_httpparser_util.h"
#include "parser/httpparser/aos_parser.h"
//#include "parser/httpparser/aos_httpparser_util.c"
//#include "parser/httpparser/aos_parser.c"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Random.h"
#include "Util/RandomSelector.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


AosParserTorturer::AosParserTorturer()
{
}

bool AosParserTorturer::start()
{
    OmnBeginTest << "Begin the Parser Torturer Testing";
    mTcNameRoot = "Parser_Torturer";
//    specialTest1();	

    test_req();
	test_res();
	return true;
}

// extern void parser_init();
//extern void parser_init();
//extern void http_res_init(http_res_t* res, char* in, uint32_t in_len);
//extern parser_result_t http_res_parse(http_res_t* res);

bool 
AosParserTorturer::test_req()
{
	int tries = 2000000;

//	bool validResponse;

	while (tries >0)
	{
		// 
		// Create an HTTP request 
		//
		OmnString head = "";

		//Create http_method
		
		char methods[9][8] = {"GET","POST","PUT","HEAD","TRACE","CONNECT","OPTIONS","DELETE","ERROR"};
		int method = OmnRandom::nextInt(0,8);

//		validResponse = (method == 2)?false:true;

		head += methods[method];
		OmnString sp1(OmnRandom::nextInt(1, 5), ' ', true);
		head << sp1;
		
		//Add URL
		char URI[30];
		OmnRandom::nextNoSpaceStr(URI,1,29);//should contain no blank
		head += URI;
		OmnString sp2(OmnRandom::nextInt(1, 5), ' ', true);
		head << sp2;

		char http[16] = "http/";
		head += http;
		// Add the version
		OmnString sp3(OmnRandom::nextInt(0, 3), ' ', true);
		head << sp3;
		
		char versions[4][4] = {"1.1","0.9","1.0","99"};
		int version = OmnRandom::nextInt(0,3);

//		validResponse = (version == 3)?false:true;

		head += versions[version];
		char line_end[3] ="\r\n";
		head += line_end;
		
		// Add headers
		int line_num = OmnRandom::nextInt(0,6);
		char unkown_header[6][30];
		for (int i = 0;i<line_num;i++)
		{
			OmnRandom::nextNoSpaceStr(unkown_header[i],1,29);
			head += unkown_header[i];
			head += line_end;
		}

		//Add content-length
		char content_length[17] = "content-length:";
		head += content_length;
		OmnString sp4(OmnRandom::nextInt(0, 5), ' ', true);
		head << sp4;
		
		int length = OmnRandom::nextInt(0,65535);
		char num2[10];
		sprintf(num2,"%d",length);
		head += num2;

		OmnString sp5(OmnRandom::nextInt(0, 5), ' ', true);
		head << sp5;

		head += line_end;

		char unkown_header2[3][30];
		int line_num2 = OmnRandom::nextInt(0,3);
		for (int i = 0;i<line_num2;i++)
		{
			OmnRandom::nextNoSpaceStr(unkown_header2[i],1,29);
			head += unkown_header2[i];
			head += line_end;
		}
		head += line_end;

		http_req_t req;
		parser_init();
		http_req_init(&req,(char*)head.data(),head.length());

		// Parse the http response
		int ret = http_req_parse(&req);

/*
	printf("%s\n",head.data());
	// print result 
	printf("Parser result:\n");
	printf("	Total statistics:\n");
	printf("		total len: %d, parsed len: %d\n\n", res.total_len, res.parsed_len);
	
	printf("	Parsed Result:\n");
	if (res.connect_state != 0) {
		printf("		Connect_State_Code = %d\n", res.connect_state);
	}
	
	if (res.content_len != 0) {
		printf("		Content_Len = %d\n", res.content_len);
	}
	
	if (res.http_ver != 0) {
		printf("		version = %d\n", res.http_ver);
	}
*/	
	//	if (validResponse)
	//	{
		OmnTC(OmnExpected<int>(0),OmnActual<int>(ret)) << "Message:\n" 
			<< head.data() << tries <<" " << head.length() <<endtc;
///		if (ret)
//		{
//			printf("	total len: %d, parsed len: %d\n\n", req.total_len, req.parsed_len);
//		}
	//	}
	//	else
	//	{
	//		OmnTC(OmnExpected<bool>(true),OmnActual<bool>(ret != 0)) << endtc;
	//	}
//		if (tries == 1823 )
//		{
//		printf ("error exist!**********\n");
//		}
		tries--;
		if (tries == 0)
		{
		//	printf ("where is the segmentation fault?!! \n");
		}
	}

	return true;
}


bool 
AosParserTorturer::test_res()
{
	int tries = 2000000;

//	bool validResponse;

	while (tries >0)
	{
		// 
		// Create an HTTP response 
		//
		OmnString head = "http/";
		OmnString sp1(OmnRandom::nextInt(0, 5), ' ', true);
		head << sp1;
				
		// Add the version
		char versions[4][4] = {"1.1","0.9","1.0","99"};
		int version = OmnRandom::nextInt(0,3);

//		validResponse = (version == 3)?false:true;

		head += versions[version];
		OmnString sp2(OmnRandom::nextInt(1, 5), ' ', true);
		head << sp2;

		// Create response code
		int connect_state = OmnRandom::nextInt(100,500);
		char num[10];
		sprintf(num,"%d",connect_state);
		head += num;
		OmnString sp3(OmnRandom::nextInt(1, 5), ' ', true);
		head << sp3;
		
		// Response code explanations
		char connect_explain[6];
		OmnRandom::nextStr(connect_explain,0,5);
		head += connect_explain;
		OmnString sp4(OmnRandom::nextInt(0, 3), ' ', true);
		head << sp4;
		
		char line_end[3] ="\r\n";
		head += line_end;

		// Add headers
		int line_num = OmnRandom::nextInt(0,6);
		char unkown_header[6][30];
		for (int i = 0;i<line_num;i++)
		{
			OmnRandom::nextNoSpaceStr(unkown_header[i],1,29);
			head += unkown_header[i];
			head += line_end;
		}

		char content_length[17] = "content-length:";
		head += content_length;
		OmnString sp5(OmnRandom::nextInt(0, 5), ' ', true);
		head << sp5;
		
		int length = OmnRandom::nextInt(0,65535);
		char num2[10];
		sprintf(num2,"%d",length);
		head += num2;

		OmnString sp6(OmnRandom::nextInt(0, 5), ' ', true);
		head << sp6;

		head += line_end;

		char unkown_header2[3][30];
		int line_num2 = OmnRandom::nextInt(0,3);
		for (int i = 0;i<line_num2;i++)
		{
			OmnRandom::nextNoSpaceStr(unkown_header2[i],1,29);
			head += unkown_header2[i];
			head += line_end;
		}
		head += line_end;

		http_res_t res;
		parser_init();
		http_res_init(&res,(char*)head.data(),head.length());

		// Parse the http response
		int ret = http_res_parse(&res);
	
/*
	printf("%s\n",head.data());
	// print result 
	printf("Parser result:\n");
	printf("	Total statistics:\n");
	printf("		total len: %d, parsed len: %d\n\n", res.total_len, res.parsed_len);
	
	printf("	Parsed Result:\n");
	if (res.connect_state != 0) {
		printf("		Connect_State_Code = %d\n", res.connect_state);
	}
	
	if (res.content_len != 0) {
		printf("		Content_Len = %d\n", res.content_len);
	}
	
	if (res.http_ver != 0) {
		printf("		version = %d\n", res.http_ver);
	}
*/	
	//	if (validResponse)
	//	{
		OmnTC(OmnExpected<int>(0),OmnActual<int>(ret)) << "Message:" 
			<< head.data() << tries << head.length() <<endtc;
	//	}
	//	else
	//	{
	//		OmnTC(OmnExpected<bool>(true),OmnActual<bool>(ret != 0)) << endtc;
	//	}
//		if (tries == 1823 )
//		{
//		printf ("error exist!**********\n");
//		}


		tries--;
//		if (tries == 0)
//		{
		//	printf ("where is the segmentation fault?!! \n");
//		}
	}

	return true;
}


bool
AosParserTorturer::specialTest1()
{
	OmnString response = 
		"http/     1.5 158   .*T  "
		"9:s0ej7y*_[3KT.Jm@88it}"
 		"(MRVzVl2J}w52o<oKP; ^"
 		"j{x bj}Sg&!:YxmG"
 		"9B$"
 		"TshA<"
 		"]9t=!m6Q! i$#!Z{o'&A(Qsx:52M"
 		"content-length:     38442    " 
 		"J.DLCL^*M^+)Zz(^}JdP"
 		"ZgSj6D"
 		"UO]cs*@4T{>Z"
 		"+2B1qA[Sny+VJul"
 		"*@;"
 		"!-rBAD>`Fv(O)G`weG`weG`we\r\n\r\n";

	http_res_t res;
	parser_init();
	printf ("res_length = %d",response.length());
	http_res_init(&res,(char*)response.data(), response.length());
	int ret = http_res_parse(&res);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	return true;
}


