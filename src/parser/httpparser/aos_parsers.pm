use strict;

package aos_parsers;

my @global_states = ("GLOBAL_HTTP_PARSING_DONE", "GLOBAL_HTTP_NEED_MORE", "GLOBAL_HTTP_PARSING_FAIL");

my @aparsers = (

	{
		name => "req_parser",
		initial => "0",
		global_default => "GLOBAL_HTTP_PARSING_FAIL",
		states => [
			{
				id => "0",
				transitions => [
					{ input => "HTTP_TAB_SPACE",	result => "0", },
					{ input => "CRLF",				result => "0", },
					{ input => "HTTP_ALPHA",		result => "1", },
					{ input => "g",					result => "2", },
                	{ input => "p",					result => "8", },
                	{ input => "h",					result => "45", },
                	{ input => "d",					result => "51", },
                	{ input => "t",					result => "57", },
                	{ input => "c",					result => "62", },
                	{ input => "o",					result => "74", },
				],
			},
			{
				id => "1",
				canonical_name => "UNSUPPORT_METHOD",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "HTTP_TAB_SPACE",  	result => "UNSUPPORT_METHOD_END", },
				],
			},
			{ 
				id => "2",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "e",					result => "3", },
				],
			},
			{
				id => "3",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "t",					result => "4", },
				],
			},
			{ 
				id => "8",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "o",					result => "9", },
					{ input => "u",					result => "49", },
				],
			},
			{
				id => "9",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "s",					result => "10", },
				],
			},
			{
				id => "10",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "t",					result => "11", },
				],
			},
			{ 
				id => "45",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "e",					result => "46", },
				],
			},
			{ 
				id => "46",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "a",					result => "47", },
				],
			},
			{ 
				id => "47",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "d",					result => "48", },
				],
			},
			{ 
				id => "48",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "HTTP_TAB_SPACE",	result => "HEAD", },
				],
			},
			{ 
				id => "49",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "t",					result => "50", },
				],
			},
			{ 
				id => "50",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "HTTP_TAB_SPACE",	result => "PUT", },
				],
			},
			{ 
				id => "51",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "e",					result => "52", },
				],
			},
			{ 
				id => "52",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "l",					result => "53", },
				],
			},
			{ 
				id => "53",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "e",					result => "54", },
				],
			},
			{ 
				id => "54",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "t",					result => "55", },
				],
			},
			{ 
				id => "55",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "e",					result => "56", },
				],
			},
			{
				id => "56",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "HTTP_TAB_SPACE",	result => "DELETE", },
				],
			},
			{ 
				id => "57",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "r",					result => "58", },
				],
			},
			{ 
				id => "58",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "a",					result => "59", },
				],
			},
			{ 
				id => "59",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "c",					result => "60", },
				],
			},
			{ 
				id => "60",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "e",					result => "61", },
				],
			},
			{
				id => "61",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "HTTP_TAB_SPACE",	result => "TRACE", },
				],
			},
			{ 
				id => "62",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "o",					result => "63", },
				],
			},
			{ 
				id => "63",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "n",					result => "64", },
				],
			},
			{ 
				id => "64",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "n",					result => "65", },
				],
			},
			{ 
				id => "65",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "e",					result => "66", },
				],
			},
			{ 
				id => "66",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "c",					result => "67", },
				],
			},
			{ 
				id => "67",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "t",					result => "68", },
				],
			},
			{ 
				id => "68",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "HTTP_TAB_SPACE",	result => "CONNECT", },
				],
			},
			{
				id => "74",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "p",					result => "75", },
				],
			},
			{
				id => "75",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "t",					result => "76", },
				],
			},
			{
				id => "76",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "i",					result => "77", },
				],
			},
			{
				id => "77",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "o",					result => "78", },
				],
			},
			{
				id => "78",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "n",					result => "79", },
				],
			},
			{
				id => "79",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "s",					result => "80", },
				],
			},
			{
				id => "80",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "HTTP_TAB_SPACE",	result => "OPTIONS", },
				],
			},
			{
				id => "11",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "HTTP_TAB_SPACE",	result => "POST", },
				],
			},
			{
				id => "4",
				transitions => [
					{ input => "HTTP_ALPHA",      	result => "1", },
					{ input => "HTTP_TAB_SPACE",	result => "GET", },
				],
			},
			{
				id => "5",
				canonical_name => "URI_START",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "6", },
					{ input => "HTTP_TAB_SPACE",	result => "URI_END", },
				],
			},
			{
				id => "6",
				canonical_name => "URI",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "6", },
					{ input => "HTTP_TAB_SPACE",	result => "URI_END", },
				],
			},
			{
				id => "7",
				canonical_name => "VER_START",
				transitions => [
					{ input => "t",					result => "42", },
				],
			},
			{
				id => "42",
				transitions => [
					{ input => "t",					result => "43", },
				],
			},
			{
				id => "43",
				transitions => [
					{ input => "p",					result => "44", },
				],
			},
			{
				id => "44",
				transitions => [
					{ input => "/",					result => "VER_DIGITAL_START", },
				],
			},
			{							
				id => "12",						
				canonical_name => "VER_DIGITAL_START",
				transitions => [
				{ input => "HTTP_DIGIT",			result => "13", },
				{ input => "HTTP_TAB_SPACE",		result => "12", },
				],
			},
			{							
				id => "13",						
				canonical_name => "VER_DIGITAL",
				transitions => [
				{ input => "HTTP_DIGIT",			result => "13", },
				{ input => ".",						result => "13", },
				{ input => "HTTP_CR",				result => "VER_END", },
				{ input => "HTTP_TAB_SPACE",		result => "40", },
				],
			},
    		{							
				id => "40",						
				transitions => [
				{ input => "HTTP_TAB_SPACE",		result => "40", },
				{ input => "HTTP_CR",				result => "VER_END", },
				],
			},

			{
				id => "39",
				canonical_name => "VER_END",
				transitions => [
					{ input => "HTTP_LF",			result => "14", },
					{ input => "HTTP_TAB_SPACE",	result => "39", },
				],
			},
			{
				id => "14",
				canonical_name => "SECOND_CRLF",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CR",			result => "15", },
					{ input => "HTTP_TAB_SPACE",	result => "14", },
					{ input => "HTTP_ALPHA",		result => "UNKNOWN_HEADER", },
					{ input => "c",					result => "16", },
				],
			},
			{
				id => "15",
				transitions => [
					{ input => "HTTP_LF",			result => "GLOBAL_HTTP_PARSING_DONE", },
					{ input => "HTTP_TAB_SPACE",	result => "15", },
				],
			},
			{ #header start state
				id => "16",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "o",					result => "17", },
				],
			},
			{
				id => "17",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "n",					result => "18", },
				],
			},
			{
				id => "18",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "t",					result => "19", },
				],
			},
			{
				id => "19",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "e",					result => "20", },
				],
			},
			{
				id => "20",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "n",					result => "21", },
				],
			},
			{
				id => "21",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "t",					result => "22", },
				],
			},
			{
				id => "22",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "-",					result => "23", },
				],
			},
			{
				id => "23",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "l",					result => "24", },
				],
			},
			{
				id => "24",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "e",					result => "25", },
				],
			},
			{
				id => "25",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "n",					result => "26", },
				],
			},
			{
				id => "26",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "g",					result => "27", },
				],
			},
			{
				id => "27",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "t",					result => "28", },
				],
			},
			{
				id => "28",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "h",					result => "29", },
				],
			},
			{
				id => "29",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_TAB_SPACE",	result => "29", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => ":",					result => "CONTENT_LENGTH_START", },
				],
			},
			{
				id => "30",
				canonical_name => "CONTENT_LENGTH_START",
				transitions => [
					{ input => "HTTP_DIGIT",		result => "CONTENT_LENGTH", },
					{ input => "HTTP_TAB_SPACE",	result => "30", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
				],
			},
			{
				id => "31",
				canonical_name => "CONTENT_LENGTH",
				transitions => [
					{ input => "HTTP_DIGIT",		result => "CONTENT_LENGTH", },
					{ input => "HTTP_CR",			result => "CONTENT_LENGTH_END", },
					{ input => "HTTP_TAB_SPACE",	result => "41", },
				],
			},
			{
				id => "41",
				transitions => [
					{ input => "HTTP_CR",			result => "CONTENT_LENGTH_END", },
					{ input => "HTTP_TAB_SPACE",	result => "41", },
				],
			},
			{
				id => "32",
				canonical_name => "CONTENT_LENGTH_END",
				transitions => [
					{ input => "HTTP_LF",			result => "SECOND_CRLF", },
					{ input => "HTTP_TAB_SPACE",	result => "32", },
				],
			},
#unkown state, will skip this header
			{
				id => "33",
	   #     	default => "UNKNOWN_HEADER",
				canonical_name => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_TAB_SPACE",	result => "UNKNOWN_HEADER", },
					{ input => "HTTP_PRNCHAR",		result => "UNKNOWN_HEADER", },
					{ input => "HTTP_CR",			result => "34", },
					{ input => "HTTP_LF",			result => "SECOND_CRLF", },
				],
			},
			{
				id => "34",
				transitions => [
					{ input => "HTTP_LF",			result => "SECOND_CRLF", },
					{ input => "HTTP_TAB_SPACE",	result => "34", },
				],
			},
			{
				id => "35",
				canonical_name => "GET",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "URI_START", },
   					{ input => "HTTP_TAB_SPACE",	result => "35", },
				],
			},
			{
				id => "38",
				canonical_name => "POST",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "URI_START", },
					{ input => "HTTP_TAB_SPACE",	result => "38", },
				],
			},
			{
				id => "69",
				canonical_name => "HEAD",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "URI_START", },
					{ input => "HTTP_TAB_SPACE",	result => "69", },
				],
			},
			{
				id => "70",
				canonical_name => "PUT",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "URI_START", },
					{ input => "HTTP_TAB_SPACE",	result => "70", },
				],
			},
			{
				id => "71",
				canonical_name => "DELETE",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "URI_START", },
					{ input => "HTTP_TAB_SPACE",	result => "71", },
				],
			},
			{
				id => "72",
				canonical_name => "TRACE",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "URI_START", },
					{ input => "HTTP_TAB_SPACE",	result => "72", },
				],
			},
			{
				id => "73",
				canonical_name => "CONNECT",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "URI_START", },
					{ input => "HTTP_TAB_SPACE",	result => "73", },
				],
			},
			{
				id => "81",
				canonical_name => "OPTIONS",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "URI_START", },
					{ input => "HTTP_TAB_SPACE",	result => "81", },
				],
			},
			{
				id => "36",
				canonical_name => "UNSUPPORT_METHOD_END",
				transitions => [
					{ input => "HTTP_PRNCHAR",		result => "URI_START", },
					{ input => "HTTP_TAB_SPACE",	result => "36", },
				],
			},
			{
				id => "37",
				canonical_name => "URI_END",
				transitions => [
					{ input => "HTTP_TAB_SPACE",	result => "URI_END", },
					{ input => "h",					result => "VER_START", },
				],
			},
		],
	},
	
	
	{
		name => "res_parser",
		initial => "0",
		global_default => "GLOBAL_HTTP_PARSING_FAIL",
		states => [
			{
				id => "0",
				transitions => [
					{ input => "HTTP_TAB_SPACE",	result => "0", },
					{ input => "CRLF",				result => "0", },
					{ input => "HTTP_ALPHA",		result => "0", },
					{ input => "h",					result => "2", },
				],
			},
#			{
#				id => "1",
#				canonical_name => "BEFORE_HEADER",
#				transitions => [
#					{ input => "HTTP_ALPHA",      	result => "1", },
#					{ input => "HTTP_CR",       	result => "", },
#					{ input => "h",					result => "2", },
#				],
#			},
			{
				id => "2",
				transitions => [
					{ input => "t",					result => "3", },
				],
			},
			{
				id => "3",
				transitions => [
					{ input => "t",					result => "4", },
				],
			},
			{
				id => "4",
				transitions => [
					{ input => "p",					result => "5", },
				],
			},
			{
				id => "5",
				transitions => [
					{ input => "/",					result => "6", },
				],
			},
			{							
				id => "6",						
				canonical_name => "VER_DIGITAL_START",
				transitions => [
					{ input => "HTTP_DIGIT",		result => "7", },
					{ input => "HTTP_TAB_SPACE",	result => "6", },
				],
			},
			{							
				id => "7",						
				canonical_name => "VER_DIGITAL",
				transitions => [
					{ input => "HTTP_DIGIT",		result => "7", },
					{ input => ".",					result => "7", },
					{ input => "HTTP_TAB_SPACE",	result => "VER_END", },
				],
			},
			{
				id => "12",
				canonical_name => "VER_END",
				transitions => [
					{ input => "HTTP_TAB_SPACE",	result => "12", },
					{ input => "HTTP_DIGIT",		result => "13", },
#					{ input => "HTTP_ALPHA",		result => "12", },
				],
			},
			{
				id => "13",
				canonical_name => "STATUS_CODE",
				transitions => [
					{ input => "HTTP_DIGIT",		result => "13", },
					{ input => "HTTP_TAB_SPACE",	result => "UNKNOWN_HEADER", },
				],
			},
			{
				id => "16",
				canonical_name => "SECOND_CRLF",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CR",			result => "17", },
					{ input => "HTTP_TAB_SPACE",	result => "16", },
					{ input => "HTTP_ALPHA",		result => "UNKNOWN_HEADER", },
					{ input => "c",					result => "18", },
				],
			},
			{
				id => "17",
				transitions => [
					{ input => "HTTP_LF",			result => "GLOBAL_HTTP_PARSING_DONE", },
					{ input => "HTTP_TAB_SPACE",	result => "17", },
				],
			},
			{ #header start state
				id => "18",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "o",					result => "19", },
				],
			},
			{
				id => "19",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "n",					result => "20", },
				],
			},
			{
				id => "20",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "t",					result => "21", },
				],
			},
			{
				id => "21",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "e",					result => "22", },
				],
			},
			{
				id => "22",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "n",					result => "23", },
				],
			},
			{
				id => "23",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "t",					result => "24", },
				],
			},
			{
				id => "24",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "-",					result => "25", },
				],
			},
			{
				id => "25",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "l",					result => "26", },
				],
			},
			{
				id => "26",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "e",					result => "27", },
				],
			},
			{
				id => "27",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "n",					result => "28", },
				],
			},
			{
				id => "28",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "g",					result => "29", },
				],
			},
			{
				id => "29",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "t",					result => "30", },
				],
			},
			{
				id => "30",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => "h",					result => "31", },
				],
			},
			{
				id => "31",
				default => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_TAB_SPACE",	result => "31", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
					{ input => ":",					result => "CONTENT_LENGTH_START", },
				],
			},
			{
				id => "32",
				canonical_name => "CONTENT_LENGTH_START",
				transitions => [
					{ input => "HTTP_DIGIT",		result => "CONTENT_LENGTH", },
					{ input => "HTTP_TAB_SPACE",	result => "32", },
					{ input => "HTTP_CR",			result => "UNKNOWN_HEADER", },
				],
			},
			{
				id => "33",
				canonical_name => "CONTENT_LENGTH",
				transitions => [
					{ input => "HTTP_DIGIT",		result => "CONTENT_LENGTH", },
					{ input => "HTTP_CR",			result => "CONTENT_LENGTH_END", },
					{ input => "HTTP_TAB_SPACE",	result => "37", },
				],
			},
			{
				id => "34",
				canonical_name => "CONTENT_LENGTH_END",
				transitions => [
					{ input => "HTTP_LF",			result => "SECOND_CRLF", },
					{ input => "HTTP_TAB_SPACE",	result => "34", },
				],
			},
#unkown state, will skip this header
			{
				id => "35",
				default => "UNKNOWN_HEADER",
				canonical_name => "UNKNOWN_HEADER",
				transitions => [
					{ input => "HTTP_CTLCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_EXTCHAR",		result => "GLOBAL_HTTP_PARSING_FAIL", },
					{ input => "HTTP_TAB_SPACE",	result => "UNKNOWN_HEADER", },
					{ input => "HTTP_PRNCHAR",		result => "UNKNOWN_HEADER", },
					{ input => "HTTP_LF",			result => "SECOND_CRLF", },
					{ input => "HTTP_CR",			result => "36", },
				],
			},
			{
				id => "36",
				transitions => [
					{ input => "HTTP_LF",			result => "SECOND_CRLF", },
					{ input => "HTTP_TAB_SPACE",	result => "36", },
				],
			},
			{
				id => "37",
				transitions => [
						{ input => "HTTP_CR",			result => "CONTENT_LENGTH_END", },
						{ input => "HTTP_TAB_SPACE",	result => "37", },
				],
			},
		],
	},


);

sub get_parsers()
{
	return \@aparsers;
}

sub get_global_states()
{
	return \@global_states;
}
