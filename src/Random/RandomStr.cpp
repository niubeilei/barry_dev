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
// This class generates random strings. A string is generally defined
// as three parts:
//    <Lead> <Body> <Tail>
//
// Each part has its minimum, maximum, and its own character set. 
// When generating a random string, it will construct the string
// based on the definition of the three parts. 
//
//     <Random>
//         <Type>RandomString</Type>
//         <MinLen>
//         <MaxLen>
//         <StringType>
//     </Random>
//
// Modification History:
// 03/09/2007 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Random/RandomStr.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Random/RandomUtil.h"
#include "Random/random_util.h"

//Decimal    Octal    Hex     Binary    Value    Description
//----------------------------------------------------------------------------
//  000      000    000   00000000      NUL    (Null char.)
//	001      001    001   00000001      SOH    (Start of Header)
//	002      002    002   00000010      STX    (Start of Text)
//	003      003    003   00000011      ETX    (End of Text)
//	004      004    004   00000100      EOT    (End of Transmission)
//	005      005    005   00000101      ENQ    (Enquiry)
//	006      006    006   00000110      ACK    (Acknowledgment)
//	007      007    007   00000111      BEL    (Bell)
//	008      010    008   00001000       BS    (Backspace)
//	009      011    009   00001001       HT    (Horizontal Tab)
//	010      012    00A   00001010       LF    (Line Feed)
//	011      013    00B   00001011       VT    (Vertical Tab)
//	012      014    00C   00001100       FF    (Form Feed)
//	013      015    00D   00001101       CR    (Carriage Return)
//	014      016    00E   00001110       SO    (Shift Out)
//	015      017    00F   00001111       SI    (Shift In)
//	016      020    010   00010000      DLE    (Data Link Escape)
//	017      021    011   00010001      DC1    (XON) (Device Control 1)
//	018      022    012   00010010      DC2          (Device Control 2)
//	019      023    013   00010011      DC3    (XOFF)(Device Control 3)
//	020      024    014   00010100      DC4          (Device Control 4)
//	021      025    015   00010101      NAK    (Negative Acknowledgement)
//	022      026    016   00010110      SYN    (Synchronous Idle)
//	023      027    017   00010111      ETB    (End of Trans. Block)
//	024      030    018   00011000      CAN    (Cancel)
//	025      031    019   00011001       EM    (End of Medium)
//	026      032    01A   00011010      SUB    (Substitute)
//	027      033    01B   00011011      ESC    (Escape)
//	028      034    01C   00011100       FS    (File Separator)
//	029      035    01D   00011101       GS    (Group Separator)
//	030      036    01E   00011110       RS    (Request to Send)(Record Separator)
//	031      037    01F   00011111       US    (Unit Separator)
//	032      040    020   00100000       SP    (Space)
//	033      041    021   00100001        !    (exclamation mark)
//	034      042    022   00100010        "    (double quote)
//	035      043    023   00100011        #    (number sign)
//	036      044    024   00100100        $    (dollar sign)
//	037      045    025   00100101        %    (percent)
//	038      046    026   00100110        &    (ampersand)
//	039      047    027   00100111        '    (single quote)
//	040      050    028   00101000        (    (left/opening parenthesis)
//	041      051    029   00101001        )    (right/closing parenthesis)
//	042      052    02A   00101010        *    (asterisk)
//	043      053    02B   00101011        +    (plus)
//	044      054    02C   00101100        ,    (single quote)
//	045      055    02D   00101101        -    (minus or dash)
//	046      056    02E   00101110        .    (dot)
//	047      057    02F   00101111        /    (forward slash)
//	048      060    030   00110000        0
//	049      061    031   00110001        1
//	050      062    032   00110010        2
//	051      063    033   00110011        3
//	052      064    034   00110100        4
//	053      065    035   00110101        5
//	054      066    036   00110110        6
//	055      067    037   00110111        7
//	056      070    038   00111000        8
//	057      071    039   00111001        9
//	058      072    03A   00111010        :    (colon)
//	059      073    03B   00111011        ;    (semi-colon)
//	060      074    03C   00111100        <    (less than)
//	061      075    03D   00111101        =    (equal sign)
//	062      076    03E   00111110        >    (greater than)
//	063      077    03F   00111111        ?    (question mark)
//	064      100    040   01000000        @    (AT symbol)
//	065      101    041   01000001        A
//	066      102    042   01000010        B
//	067      103    043   01000011        C
//	068      104    044   01000100        D
//	069      105    045   01000101        E
//	070      106    046   01000110        F
//	071      107    047   01000111        G
//	072      110    048   01001000        H
//	073      111    049   01001001        I
//	074      112    04A   01001010        J
//	075      113    04B   01001011        K
//	076      114    04C   01001100        L
//	077      115    04D   01001101        M
//	078      116    04E   01001110        N
//	079      117    04F   01001111        O
//	080      120    050   01010000        P
//	081      121    051   01010001        Q
//	082      122    052   01010010        R
//	083      123    053   01010011        S
//	084      124    054   01010100        T
//	085      125    055   01010101        U
//	086      126    056   01010110        V
//	087      127    057   01010111        W
//	088      130    058   01011000        X
//	089      131    059   01011001        Y
//	090      132    05A   01011010        Z
//	091      133    05B   01011011        [    (left/opening bracket)
//	092      134    05C   01011100        \    (back slash)
//	093      135    05D   01011101        ]    (right/closing bracket)
//	094      136    05E   01011110        ^    (caret/cirumflex)
//	095      137    05F   01011111        _    (underscore)
//	096      140    060   01100000        `
//	097      141    061   01100001        a
//	098      142    062   01100010        b
//	099      143    063   01100011        c
//	100      144    064   01100100        d
//	101      145    065   01100101        e
//	102      146    066   01100110        f
//	103      147    067   01100111        g
//	104      150    068   01101000        h
//	105      151    069   01101001        i
//	106      152    06A   01101010        j
//	107      153    06B   01101011        k
//	108      154    06C   01101100        l
//	109      155    06D   01101101        m
//	110      156    06E   01101110        n
//	111      157    06F   01101111        o
//	112      160    070   01110000        p
//	113      161    071   01110001        q
//	114      162    072   01110010        r
//	115      163    073   01110011        s
//	116      164    074   01110100        t
//	117      165    075   01110101        u
//	118      166    076   01110110        v
//	119      167    077   01110111        w
//	120      170    078   01111000        x
//	121      171    079   01111001        y
//	122      172    07A   01111010        z
//	123      173    07B   01111011        {    (left/opening brace)
//	124      174    07C   01111100        |    (vertical bar)
//	125      175    07D   01111101        }    (right/closing brace)
//	126      176    07E   01111110        ~    (tilde)
//	127      177    07F   01111111      DEL    (delete)

AosRandomStr::AosRandomStr()
{
}


AosRandomStr::AosRandomStr(const OmnString &name, 
						   const u32 min, 
						   const u32 max, 
						   const AosStrType strtype)
:
mName(name),
mMin(min),
mMax(max),
mStrType(strtype)
{
	mIsGood = initCharset();
}


AosRandomStr::AosRandomStr(const OmnXmlItemPtr &conf)
{
	//
	// This constructor creates the instance based on an XML configuration:
	// <Random>
	//     <Name>
	//     <Type>RandomString</Type>
	//     <MinLen>
	//     <MaxLen>
	//     <StrType>
	// </Random>
	// 
	
	mIsGood = false;
	mName = conf->getStr("Name", "NoName");
	mMin = (u32)conf->getInt("MinLen", 1);
	mMax = (u32)conf->getInt("MaxLen", eDefaultMaxLen);
	
	OmnString type = conf->getStr("StrType", "");
	if (type == "")
	{
		OmnAlarm << "Missing string type: " << conf->toString() << enderr;
		return;
	}

	mStrType = AosStrType_strToEnum(type);
	if (mStrType == eAosStrType_Invalid)
	{
		OmnAlarm << "Invalid string type. Allowed types are: " 
			<< AosStrType_getStrTypeNames() << enderr;
	}

	mIsGood = initCharset();
}


AosRandomStr::~AosRandomStr()
{
}


// 
// This function selects an index based on the weights
//
int 		
AosRandomStr::nextInt(AosGenTable &data)
{
	OmnAlarm << "Cannot call AosRandomStr::nextInt()" << enderr;
	return -1;
}

int 		
AosRandomStr::nextInt()
{
	OmnAlarm << "Cannot call AosRandomStr::nextInt()" << enderr;
	return -1;
}


int 		
AosRandomStr::nextInt(const OmnString &name)
{
	OmnAlarm << "Cannot call AosRandomStr::nextInt()" << enderr;
	return -1;
}


int 		
AosRandomStr::nextInt(const OmnString &name, AosGenTable &data)
{
	OmnAlarm << "Cannot call AosRandomStr::nextInt()" << enderr;
	return -1;
}


OmnString
AosRandomStr::nextStr(AosGenTable &data)
{
	// 
	// TBD
	//
	return "";
}


bool
AosRandomStr::generateStr(OmnString &str, 
						  const u32 min, 
						  const u32 max, 
						  const char *cs, 
						  const u32 charsetSize)
{
	aos_assert_r(min <= max, false);

	int len = OmnRandom::nextU32(min, max);
	for (int i=0; i<len; i++)
	{
		str << cs[aos_next_int(0, charsetSize-1)];
	}
	return true;
}

		
OmnString
AosRandomStr::nextStr()
{
	return nextStr(mMin, mMax);
}


// 
// This function generates a random string whose length is randomly
// determined between [min, max].
//
OmnString
AosRandomStr::nextStr(const u32 min, const u32 max)
{
	switch (mStrType)
	{
	case eAosStrType_HHMMSS:
		 return aos_next_time_str(eAosTimeStrFormat_HHMMSS);

	case eAosStrType_InvalidHHMMSS:
		 return aos_next_invalid_time_str(mMin, mMax);

	default:
		 break;
	}

	aos_assert_r(mBodyCharset, "***Error***");
	aos_assert_r(mBodyCharsetSize > 0, "***Error***");

	OmnString str;
	if (mLeadCharsetSize == 0 && mTailCharsetSize == 0)
	{
		aos_assert_r(generateStr(str, min, max, mBodyCharset, mBodyCharsetSize), 
						str);
		checkStr(str);
		return str;
	}

	// 
	// First, generate the lead part, if needed.
	//
	if (mLeadCharsetSize > 0)
	{
		// 
		// Generate the lead part first. The lead part min must be 'mLeadMin', 
		// but the its max must not be bigger than 
		// (Min - (mLeadMin + mTailMin)) + mLeadMin
		//
		u32 delta = min - (mLeadMin + mTailMin) + mLeadMin;
		u32 mmax = (mLeadMax < delta)?mLeadMax:delta;
		aos_assert_r(generateStr(str, mLeadMin, mmax, 
				mLeadCharset, mLeadCharsetSize), str);
	}

	// 
	// Second, generate the tail part, if needed.
	//
	OmnString tail;
	if (mTailCharsetSize > 0)
	{
		u32 delta = min - (str.length() + mTailMin) + mTailMin;
		u32 mmax = (mTailMax < delta)?mTailMax:delta;
		aos_assert_r(generateStr(str, mTailMin, mmax, mTailCharset, 
				mTailCharsetSize), tail);
	}

	int mmin = min - str.length() - tail.length();
	if (mmin < 0) mmin = 0;
	int mmax = max - str.length() - tail.length();
	aos_assert_r(generateStr(str, mmin, mmax, mBodyCharset, 
			mBodyCharsetSize), str);
	if (tail.length() > 0)
	{
		str << tail;
	}
	
	checkStr(str);
	return str; 
}


OmnString
AosRandomStr::nextStr(const OmnString &name, AosGenTable &data)
{
	// 
	// TBD
	//
	return "";
}


OmnString
AosRandomStr::nextStr(const OmnString &name)
{
	// 
	// TBD
	//
	return "";
}


u32
AosRandomStr::setCharset(char *charset, 
						 const u32 startIndex, 
						 const char startChar, 
						 const char endChar)
{
	aos_assert_r(endChar >= startChar, 0);

	u32 index = startIndex;
	for (char i=startChar; i<=endChar; i++)
	{
		if (index >= eCharsetMaxSize)
		{
			OmnAlarm << "Charset too big: " << startIndex
				<< ". Size: " << (int)endChar - (int)startChar + 1
				<< enderr;
			return 0;
		}

		charset[index++] = i;
	}

	return endChar - startChar + 1;
}


bool
AosRandomStr::initCharset()
{
	mLeadCharsetSize = 0;
	mTailCharsetSize = 0;
	mBodyCharsetSize = 0;
	switch (mStrType)
	{
	case eAosStrType_Invalid:
		 OmnAlarm << "Invalid string type" << enderr;
		 return false;

    case eAosStrType_AsciiStr:
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 1, 126);
		 return true;

	case eAosStrType_Printable:
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 32, 126);
		 return true;

	case eAosStrType_NoSpaceStr:
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 33, 33);
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 35, 91);
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 93, 126);
		 return true;

	case eAosStrType_BinStr:
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 0, 255);
		 return true;

	case eAosStrType_LowcaseStr:
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 'a', 'z');
		 return true;

	case eAosStrType_CapitalStr:
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 'A', 'Z');
		 return true;

	case eAosStrType_DigitStr:
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, '0', '9');
		 return true;

	case eAosStrType_LetterStr:
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 'a', 'z');
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 'A', 'Z');
		 return true;

	case eAosStrType_LetterDigitStr:
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 'a', 'z');
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 'A', 'Z');
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, '0', '9');
		 return true;

	case eAosStrType_Symbol:
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 33, 47);
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 58, 64);
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 91, 96);
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 123, 126);
		 return true;

	case eAosStrType_VarName:
		 // 
		 // VarName is a letter-digit-undersore string that can only start 
		 // with letters and underscores. 
		 //
		 mLeadMin = 1;
		 mLeadMax = 1;
		 mLeadCharsetSize = setCharset(mLeadCharset, 0, 'a', 'z');
		 mLeadCharsetSize += setCharset(mLeadCharset, mLeadCharsetSize, 'A', 'Z');
		 mLeadCharsetSize += setCharset(mLeadCharset, mLeadCharsetSize, '_', '_');

		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 'a', 'z');
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 'A', 'Z');
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, '0', '9');
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, '_', '_');
		 return true;

	case eAosStrType_WhiteSpace:
		 // 
		 // It includes space and tab
		 //
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, ' ', ' ');
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, '\t', '\t');
		 return true;
	     
	case eAosStrType_WhiteSpaceWithNewLines:
		 // 
		 // It includes spaces, tabs, '\n', and '\t'. 
		 //
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, ' ', ' ');
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, '\t', '\t');
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, '\n', '\n');
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, '\r', '\r');
		 return true;

	case eAosStrType_NonPrintable:
		 // 
		 // It includes [1, 31], 127
		 //
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 1, 31);
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 127, 127);
		 return true;

	case eAosStrType_ControlChar:
		 // 
		 // It includes: 8 (backspace), 9('\t'), 10('\n'), 
		 // 11 (vertical tab), 12 (form feed)
		 // 13 ('\r'), 27 (ESC), 127 (DEL)
		 //
		 mBodyCharsetSize = setCharset(mBodyCharset, 0, 8, 13);
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 27, 27);
		 mBodyCharsetSize += setCharset(mBodyCharset, mBodyCharsetSize, 127, 127);
		 return true;

	case eAosStrType_HHMMSS:
	case eAosStrType_InvalidHHMMSS:
	 	 // 
		 // No need to create charset for these two types.
		 //
		 return true;

	default:
		 OmnAlarm << "Unrecognized string type: " << mStrType << enderr;
		 return false;
	}

	aos_should_never_come_here;
	return false;
}


bool		
AosRandomStr::setCharset(const u32 leadmin, const u32 leadmax, 
						 const char *leadcs, const u32 leadCharsetSize, 
						 const char *bodycs, const u32 bodyCharsetSize, 
						 const u32 tailmin, const u32 tailmax, 
						 const char *tailcs, const u32 tailCharsetSize)
{
	//
	// A string normally consists of three parts: 
	// The leading part
	// The body part
	// The tail part
	//
	// This function serves to set the user defined character set. 
	//
	mLeadCharsetSize = 0;
	mBodyCharsetSize = 0;
	mTailCharsetSize = 0;

	aos_assert_r(leadCharsetSize <= eCharsetMaxSize, false)
	aos_assert_r(bodyCharsetSize <= eCharsetMaxSize, false)
	aos_assert_r(tailCharsetSize <= eCharsetMaxSize, false)

	mLeadMin = leadmin; 
	mLeadMax = leadmax;
	mLeadCharsetSize = leadCharsetSize;
	if (leadCharsetSize) memcpy(mLeadCharset, leadcs, leadCharsetSize);

	mBodyCharsetSize = bodyCharsetSize;
	memcpy(mBodyCharset, bodycs, bodyCharsetSize);

	mTailMin = tailmin; 
	mTailMax = tailmax;
	mTailCharsetSize = tailCharsetSize;
	if (tailCharsetSize) memcpy(mTailCharset, tailcs, tailCharsetSize);
	return true;
}


bool
AosRandomStr::checkStr(OmnString &str)
{
	// 
	// Need to remove any '@'. 
	//
	return true;
}
