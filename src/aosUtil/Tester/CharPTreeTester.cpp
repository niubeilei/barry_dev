////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CharPTreeTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/CharPTreeTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/String.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"

const int sgDefaultValue = 10;
static OmnRandom sgRandom;

bool OmnCharPTreeTester::start()
{
	// 
	// Test default constructor
	//
	specialCases();
	randomTester();
	return true;
}


bool OmnCharPTreeTester::specialCases()
{
	specialTest1();
	specialTest2(); 
	specialTest3(); 
	specialTest4(); 
	specialTest5(); 
	specialTest6(); 
	specialTest7(); 
	specialTest8(); 
	specialTest9(); 
	specialTest10(); 
	specialTest11(); 
	specialTest12(); 
	specialTest13(); 
	specialTest14(); 
	specialTest15();
	specialTest16();
	return true;
}

 
bool OmnCharPTreeTester::randomTester()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-RandomTesting";
	struct aosCharPtree * tree = aosCharPtree_create();
	int deleted;
	int rslt;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;

	OmnCharPtreeTestData data[eTries];
	int i;

	for (i=0; i<eTries; i++)
	{
		if ((i % 50) == 0)
		{
			cout << "// Entries: " << i << endl;
		}

		OmnRandom::nextStr(data[i].mKey, eAosMaxChar - eAosMinChar);
		data[i].mValue = OmnRandom::nextInt();
		data[i].mInsertFlag = (OmnRandom::nextInt() & 0x03) != 0;
		data[i].mIsDeleted = false;

		if (data[i].mInsertFlag)
		{
			addEntry(tree, data[i].mKey, (void *)data[i].mValue, 1);
		}
		else
		{
			// 
			// Check whether the key is in the tree.
			//
			int index;
			bool isInList = isKeyInList(tree, data, i, data[i].mKey, index);
			removeEntry(tree, data[i].mKey, isInList);

			// 
			// Mark all the insert as being deleted
			//
			for (int p=0; p<index; p++)
			{
				if (strcmp(data[p].mKey, data[index].mKey) == 0)
				{
					data[p].mIsDeleted = true;
				}
			}
		}

		verify(tree, data, i);
	}

	// 
	// After finishing all, remove all
	//
	for (i=eTries-1; i>=0; i--)
	{
		if (data[i].mInsertFlag)
		{
			// 
			// Make sure the key is not deleted
			//
			int k;
			for (k=i+1; k<eTries; k++)
			{
				if (strcmp(data[i].mKey, data[k].mKey) == 0)
				{
					break;
				}
			}

			if (k >= eTries)
			{
				// 
				// Not deleted yet
				//
				rslt = aosCharPtree_remove(tree, data[i].mKey, &deleted);

				if (strlen(data[i].mKey) == 0)
				{
					OmnTC(OmnExpected<int>(eAosAlarmEmptyKey), OmnActual<int>(rslt)) << endtc;
				}
				else
				{
					OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
				}
				OmnTC(OmnExpected<int>(1), OmnActual<int>(deleted)) << endtc;
			}
		}
	}

	// 
	// The tree should be empty now.
	//
	OmnTC(OmnExpected<int>(0), OmnActual<int>(tree->mRoot->mNumValidBranches)) << endtc;

	for (i=0; i<eAosCharTreeBranchSize; i++)
	{
		OmnTC(OmnExpected<int>(0), OmnActual<int>((int)tree->mRoot->mBranches[i])) << endtc;
	}

#endif
	return true;
}


bool
OmnCharPTreeTester::isKeyInList(aosCharPtree *tree, 
								OmnCharPtreeTestData *data, 
								const int numEntries, 
								const char *key, 
								int &index)
{
	for (int m=numEntries-1; m>=0; m--)
	{
		if (strcmp(data[m].mKey, key) == 0)
		{
			if (data[m].mInsertFlag && !data[m].mIsDeleted)
			{
				index = m;
				return true;
			}
		}
	}

	return false;
}


bool
OmnCharPTreeTester::addEntry(aosCharPtree * tree, 
							 const char *key, 
							 void *value, 
							 bool override)
{
#if 0
	cout << "    addEntry(tree, \""
		<< key << "\", (void *)"
		<< value << ", "
		<< override << "); " << endl;
	int rslt = aosCharPtree_insert(tree, key, strlen(key), value, override);

	if (strlen(key) == 0)
	{
		OmnTC(OmnExpected<int>(eAosAlarmEmptyKey), OmnActual<int>(rslt)) << endtc;
	}
	else
	{
		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	}

	return (rslt == 0);
#endif
	return false;
}


bool
OmnCharPTreeTester::removeEntry(aosCharPtree * tree, 
							 const char *key, 
							 bool expected)
{
#if 0
	cout << "    removeEntry(tree, \""
		<< key << "\", "
		<< expected << ");" << endl;

	int deleted;
	int rslt = aosCharPtree_remove(tree, key, &deleted);
	bool flag = (deleted != 0);

	if (strlen(key) == 0)
	{
		OmnTC(OmnExpected<int>(eAosAlarmEmptyKey), OmnActual<int>(rslt)) << endtc;
	}
	else
	{
		OmnTC(OmnExpected<bool>(expected), OmnActual<bool>(flag)) << endtc;
		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	}

	return rslt == 0;
#endif

	return false;
}


bool
OmnCharPTreeTester::verify(struct aosCharPtree * tree, 
						   OmnCharPtreeTestData *data, 
						   int numEntries)
{
	char theKey[eAosMaxChar - eAosMinChar];

	int len = sgRandom.nextInt(1, eAosMaxChar - eAosMinChar);
	for (int k=0; k<len; k++)
	{
		char c = (char)sgRandom.nextInt(eAosMinChar, eAosMaxChar);
		if (c == '"' || c == '\\')
		{
			k--;
			continue;
		}
		theKey[k] = c;
	}

	theKey[len] = 0;
	theKey[0] = 't';
	verify(tree, theKey, data, numEntries);

	for (int i=0; i<=numEntries; i++)
	{
		verify(tree, data[i].mKey, data, numEntries);
	}

	return true;
}


bool
OmnCharPTreeTester::verify(struct aosCharPtree * tree, 
						   char *theKey,
						   OmnCharPtreeTestData *data,
						   int numEntries)
{
#if 0
	bool found = false;
	int value = -1;
	for (int i=numEntries; i>=0; i--)
	{
		if (strcmp(data[i].mKey, theKey) == 0)
		{
			found = data[i].mInsertFlag && !data[i].mIsDeleted;
			if (found)
			{
				value = data[i].mValue;
				break;
			}
		}
	}
 
	void *node; 
	int index;
	int rslt = aosCharPtree_get(tree, theKey, &index, &node);
	if (strlen(theKey) == 0)
	{
		OmnTC(OmnExpected<int>(eAosAlarmEmptyKey), OmnActual<int>(rslt)) << endtc;
		return true;
	}

	if (found)
	{
		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
		OmnTC(OmnExpected<int>(value), OmnActual<int>((int)node)) 
			<< theKey << endtc;

		if (rslt != 0 || value != (int)node)
		{
			exit(0);
			while (1)
			{
				rslt = aosCharPtree_get(tree, theKey, &index, &node);
			}
		}
	}
	else
	{
		// 
		// The key is not in the list. Check the longest prefix. 
		// 
		int len = -1;
		for (int i=numEntries; i>=0; i--)
		{
			if (data[i].mInsertFlag && !data[i].mIsDeleted)
			{
				int p = OmnString::findPrefix(data[i].mKey, theKey);
				if (p > len) len = p;
			}
		}
 		 
		if (rslt != eAosRc_NotFound)
		{
			while (1)
			{
				rslt = aosCharPtree_get(tree, theKey, &index, &node);
			}
		}

		OmnTC(OmnExpected<int>(eAosRc_NotFound), OmnActual<int>(rslt)) << theKey << endtc;
		OmnTC(OmnExpected<int>(len), OmnActual<int>(index)) << theKey << endtc;

		if (len != index)
		{
			cout << "incorrect" << endl;
		}
	}
#endif
 
	return true;
}


#define AosHttpFwdtbl_MaxIdLen  300
bool OmnCharPTreeTester::specialTest1()
{
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest1";
	struct aosCharPtree * tree = aosCharPtree_create();
	int deleted;
	char app[256] ="app3";
	char url[256] ="www.ipacketengine201.com";
	char *id;
	int vslen;
	void *ptr;
	int foundIndex;

	id = (char *)aos_malloc(AosHttpFwdtbl_MaxIdLen);
	aos_assert1(id);

	vslen = strlen(app);
	strcpy(id, app);
	id[vslen] = 'z';

aos_trace("url %s", url);
    strncpy(&id[vslen+1], url, AosHttpFwdtbl_MaxIdLen-vslen-1);
aos_trace("id\n%s",id);
aos_trace_hex("id",id, vslen+1+strlen(url));
aos_trace_hex("id",id, AosHttpFwdtbl_MaxIdLen);


	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;

	if ( aosCharPtree_insert(tree, id, strlen(id), (void*)1234, 0))
	{
		cout << "Failed to add" << endl;
		aos_free(id);
		return false;
	}

	aosCharPtree_get(tree, id, &foundIndex, &ptr);
	if (!ptr)
	{
		cout << "Failed to get" << endl;
		aos_free(id);
		return false;
	}
	aos_free(id);
/*
	aosCharPtree_insert(tree, "B:='4:ODf}DF6 3@bs`QK5D", 23, (void*)26177, 0);
	aosCharPtree_remove(tree, "&+h[W*B_6AQ.EBv06g06#pvmt6==78g2@=RDD*f]j|&M^GFqY~-,'0JTjh7YCrz@`f0(&/MV", &deleted);
	aosCharPtree_insert(tree, "@]l HivF3LgrM?mtz>+v-nz6,T?xWaV:Mv'JM>,w!5N#7oVU{{N^$G};nt9o2t}efXI@7}5CzUyD64tYp0-}%(m", 87, (void*)9378, 0);
	aosCharPtree_insert(tree, "3JCJ$fEeHXK!V", 13, (void*)21062, 0);
	aosCharPtree_remove(tree, "4~D-b?F5n;Ht]?I=WC9$&0MC2(Q#XLNyCS;':4&FK SxX|}EQM6sy", &deleted);
	aosCharPtree_insert(tree, "zkn,VowCt;L!jDG_06>!.I%uRC=]/&l:tAL?3(ZBSBLE]uMO.7&~~q2fOE`6)%", 61, (void*)30130, 0);
	aosCharPtree_insert(tree, "p 3C$zSbutX;FU$z)l/' KL#NWZO`/k>kHG,!t\'l6~dwPP-WIncCC).?lqf*N7P~0A51Vm@w", 71, (void*)22019, 0);
	aosCharPtree_insert(tree, "HS0I[94D[<5;SnxYCM!er!CQy'<;@Hdo2}`*0}@;$!{KsHv;}INM4qgR*s-TXJ>GRRuXGBK|$;r*", 75, (void*)22953, 0);
	aosCharPtree_insert(tree, "GB&X'h{QnsuhvXvVzM3OPv[^&&iR4^:c", 32, (void*)22663, 0);
	aosCharPtree_remove(tree, "rV?(}(.K@r>mKnY3-`@Uddh>o-!XAb9dtSA", &deleted);
*/
	return true;
}


bool OmnCharPTreeTester::specialTest2()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest1";
	struct aosCharPtree * tree = aosCharPtree_create();
	int deleted;
	int rslt;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
	rslt = aosCharPtree_insert(tree, "=6JGP{<5|7TFfib'tGX wFO-4#qrA!_1iP", 34, (void *)13427, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "i+-:T*w~|h)dGb-Xu[f)O)`&n^bej|Twg@lm>6L9GTXzm^la7R^aBrvD4>u3EEym:", 65, (void *)28303, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "#'u<Qf//^E&s*Tky", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "+|*<{Lnc'TU4aL8", 15, (void *)31839, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "15(O9e(RJa-|%oQZpl5ds(<84!:ndK", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "WRe7Cm3~?kmjmei+IPEWtB40t{J|u.A`WFsc3:~t<i7JuxtEM0NtzhrgeF`C%i*;d)W", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "D/rB&X>dcO83$YKPIc+YpGv", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "WW78XGr~8kwgQ-<*f}:2|mO-YFbmlI~G,'&EaK0c/w)G_;@[7->6+RhZfsSr48~L&GZ@! `WqS=VO^apmPY(7fp-R<jF", 92, (void *)1553, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "Lkj8DO-~W_,i<O_F^@$s;j(-8Ee|^NW;8So#2'jx>q+Lx+VR5kIp#L]bTwxf<vD>Lj!?~PD}V+e%OI|{yA", 82, (void *)6509, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "WL;`g}ccC`i", 11, (void *)29621, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "3$nC*)dM_'fJpUvO/(NO/^iNyU8H-KZ9U`#jTHIiEsC'7c8XkmESsm@@V}U,DJ", 62, (void *)30970, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "36(BlH2i)0U4J* K4z<_u:H1kI^jDT}BM$RJ/#q21|;y-a+!vkW`i'", 54, (void *)2032, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "C6>>r;'2JR)ojQ3!i)piL6;MfPeA)Jg-+EghIZp%Dz,O2eUmPUblAvpH1mlfKIG o7XP^/cLO|cQph-%", 80, (void *)25812, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "S8L`]354wEspdiaBf#2Uc(dSaRa2k7n>[z^_Q]T3.yK3", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "3ff#=:ki [q:*qzIsN?Up7", 22, (void *)7286, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "w~MJ.b+?:G#f5J:u?yx;:Z^[ta3k5<KQ[{(", 35, (void *)3964, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "aEB[0AI gddP3T", 14, (void *)5278, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "Mvc>a%Q`-v0iaRSP4}>oUI^#rar{q'p1I[q>B@)]UM;4{R$P&M5&KECAJp,M%*C.U|@LYRGpfErW6e*|", 80, (void *)9704, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, ",}l:)", 5, (void *)2785, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "hpVxG1Bh)&k2Ngt$v@|0U5w~y]Zh4CfF]JJvH", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "R7wAGLMCM^0~/xHf%:>9[Q b!?!#!$]U/iVZjO{#Nvj=L24ha)", 50, (void *)10726, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "j/QQC`ra:W9I7uK4j#,}L(o#GF/ysT/PnuH9R)!7=jYi d&zfz5Vw53du'5NFF+*LE9ZT7vV*_}_)NB", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "XyL.~6A[+##O>hA {^97*yG_bK<h^ Za5W1XdRMAp]b{o<L4MISN|IU)t >Q)v", 62, (void *)2188, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "gk},%S%_D'{DpXg*5jPI?Avs;<U&r^O88BvO0HF|+8c=/WZk!TTIl#0#4&zgzo,!Pm;tv_5OYo>&{rn@Ts%]tE4[", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "k(E&L8Yf cJ,z-]S_=", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "R^!xem5IR~OD7mggL@~~`YK3;wEzf+U)xg|<nw<G78O|`slr", 48, (void *)14210, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "`E", 2, (void *)32328, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "=$Iq9%?DCIzqKOr<&7yc/XqB&[*OYpI=~,>kZ@kYzOcA7m/2MrY0=5{rW# TvD)+{LR", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "eC2Edj/YACtBo!@QjRe_akP:5]-Yo$*W6jMVlvM*q`brKzaKhG/!.-ycf><Pw", 61, (void *)24328, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, " fR.}?Dz:MRt7<z", 15, (void *)23204, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, ",1}157.ouo%qS54v$VNYMP^?Ec3c04APa)A6N}<Z5kmu*O3gRp``z'&GS<&P1]Jn)f~YSUcPev0W2!<J##<.)k_u#%", 90, (void *)10882, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "T!RpX u4k#s.nk=/L{fLyY5;&7v#ZA~9_1x`Y[j%XO2GL3p5oA?z", 52, (void *)1999, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "W2Am,w$)TlZ.%@b(mK M6Oxs^U#=.3wBbZvO~AEo(A-.EOu$Y_|T@", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "lJ6Ic'~N-W4_S3m$Xokw6V77<Z{YiAQdC[3m5 +QMIEmrJb_@Z4h.]_yA+?", 59, (void *)2156, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, " UWS", 4, (void *)14499, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "h`,}e^5OoBEj$$jNe.(T~)M'gJLDL.Zxd#FzA*CuV~gS*ncEpTo24G2<ZE-}''rGcj@hB'DXF_:", 75, (void *)28678, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "`[fKomldu:cBL[&T1_}q~Ux0O6^'I}RZ8?} MJCW!+-0VDN&}5HWmpy>J.5<|s'.Cu5~?lQ6ER3JiY0G8%YU", 84, (void *)18087, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "L0m%`^@T^CUdG[_!)HcFY-zin^BYm?2[I25_5y+0Up#qtYYUi7pf)v@X*N]s+N'VRnfYSJP8T.m:iO[.u?ke", 84, (void *)16771, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "iHE9^Isoryq*xaNM@X/ajjH4[1tRweXRFu9hj{02RQ+@GF>X1]T[HO}f", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "u2k+<xAxbTIoF0>m2TP['Y!B^", 25, (void *)3501, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "i+`Y-Yxsa7zQ", 12, (void *)16011, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, ",0LAICEMa6dKV|8?3n<kp;0oZoxk>+`B'&", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "QgoVNy?oTr]qYcwms>3xQbm_tUF<R9%$TKG,5~R/t5:S-34n?c)K;FXoQ!^,6", 61, (void *)12620, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, "7a)'IZXL^H2Q;7", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_insert(tree, "qf4b!_<;Y:5jRhK{P} =>)'`1J$%]uDSyyo+%TJHCio", 43, (void *)12773, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	rslt = aosCharPtree_remove(tree, ",", &deleted);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
#endif

	return true;
}


bool OmnCharPTreeTester::specialTest3()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest1";
	struct aosCharPtree * tree = aosCharPtree_create();
	int deleted;
	int rslt;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
    rslt = aosCharPtree_remove(tree, "Ft|@?[}JZ^XFS;HN[l{~gYc)(N^5<-RYUu2_#r1hy2-N n.WR55q.ps)8i1Y}fVC3^?$/ci]&11|h{L;+ $-l0{~Zol", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "F_6co=/hjkHpnhTcBek9hX[1_ JS{'NI-xH;h+>T]|e.o~S", 47, (void *)15191, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "iv?~g<E&{u0BQ0M$)Z,Ox$9efsjE1?M/*]O%{n)-e{Z7%M.7EO>+M?gS8#/OVha8/a_9(#CBl5{0[T$Uz3rSL)c", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void *node; 
	int index;
	rslt = aosCharPtree_get(tree, "Ft|@?[}JZ^XFS;HN[l{~gYc)(N^5<-RYUu2_#r1hy2-N n.WR55q.ps)8i1Y}fVC3^?$/ci]&11|h{L;+ $-l0{~Zol", &index, &node);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(index)) << endtc;
#endif 
	return true;
}


bool OmnCharPTreeTester::specialTest4()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest1";
	struct aosCharPtree * tree = aosCharPtree_create();
	int rslt;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
    rslt = aosCharPtree_insert(tree, "cB/e", 4, (void *)14635, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
 
	void *node; 
	int index;
	rslt = aosCharPtree_get(tree, "tz/QDTJrSHZ#jWNnqbeI~.=Ki<", &index, &node);
    OmnTC(OmnExpected<int>(-1), OmnActual<int>(index)) << endtc;
#endif 
	return true;
}


bool OmnCharPTreeTester::specialTest5()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest5";
	struct aosCharPtree * tree = aosCharPtree_create();
	int rslt;
	int deleted;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
    rslt = aosCharPtree_remove(tree, "@1z{tQ4.G]uF}H= U[K.u2^(W*mddG0x)K]2 65P]r", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "2Dq)+b<!yO5}Ns}iE]J1b", 21, (void *)3303, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void *node; 
	int index;
	rslt = aosCharPtree_get(tree, "2Dq)+b<!yO5}Ns}iE]J1b", &index, &node);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	OmnTC(OmnExpected<int>(3303), OmnActual<int>((int)node)) << endtc;
	OmnTC(OmnExpected<int>(strlen("2Dq)+b<!yO5}Ns}iE]J1b")-1), OmnActual<int>(index)) 
	 	<< endtc;
#endif 
	return true;
}


bool OmnCharPTreeTester::specialTest6()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest5";
	struct aosCharPtree * tree = aosCharPtree_create();
	int rslt;
	int deleted;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
    rslt = aosCharPtree_insert(tree, "}HU<F^u)tCYxxq-oxj)k(T@=J!=AtZ!^6Gw)=NIDLydh=NbE>3icocDf~Rk2F-+Ys", 65, (void *)17719, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "p`Q,4Cd#>`sJ+7]W<).^(4~3[}h5dfIIc>/C3hE#,.z|j~u[z>SGR*3C%", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void *node; 
	int index;
	rslt = aosCharPtree_get(tree, "8Z>w*T*YQ6xlA$@% l:(gi85J9WNmqeG5@^-%>$^.'Be5*BwvErw#_KfyA%c?GrBNMl6hqgLc9R.QVsv", &index, &node);
    OmnTC(OmnExpected<int>(eAosRc_NotFound), OmnActual<int>(rslt)) << endtc;
	OmnTC(OmnExpected<int>(-1), OmnActual<int>(index)) << endtc;
#endif 
	return true;
}


bool OmnCharPTreeTester::specialTest7()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest5";
	struct aosCharPtree * tree = aosCharPtree_create();
	int rslt;
	int deleted;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
    rslt = aosCharPtree_insert(tree, "Q$t2KOle ;m$ZjaR_ ,e/'E`rb-m7|!E|O+m@^~AEM8xL>qh/mT!40J T3HMJd4532K", 67, (void *)22760, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "{{u|h<J.o*a$>;kF p9i~'&|Ma^=w9Z:n`9<rj/*p&hF71:(pyaiFJeRo'?VjyowvfeOlhy*l%n`xX4DpN`N", 84, (void *)15345, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "0<9a-8PmW!I;MU{oBP}Ge/FdTd3!w6.lRGA", 35, (void *)7305, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "K", 1, (void *)19313, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "rnFn82F'=g1C,VN%**v,I$9", 23, (void *)9010, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "E`5AdDm)M0iN[-X7.jJ=:p}L7OC]]ysn!{(?IM&a',6>ni|V$Ww$=dIIi8-h+]=(KM3$I_A?w)fB()SFFS$61xTZK", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "D#]6Su[b1q0I}NL<arrzy)!r?1k<V$B$@8vkI4.o8v$0XxEjackd K7RpO35=66;j5L}K$eCw.e{@(#~=X`JMgE", 87, (void *)20181, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "vcJ[", 4, (void *)12535, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "J3yY{|WdaLKS>;}s:s7,=|h+3MMsg,IY.L=e_vf$;r>8m=<uNh;yI48O4TaN[[", 62, (void *)24238, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "gSmolw'3PL.6<%}m[q0^lTPby?C<,R;!oT&`KmXnR:56Mr?hQY>rg}sb3(pQ}&j'$X", 66, (void *)18692, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "/F|)T j?m&H", 11, (void *)23953, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "`cv9Yc!sH9eoR~biZ~=inh3+zHXo+", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "x56TO8+Pr?D9a$D 5o<9k0x=y}-^vF>e,<3)aieygxYX:)JN)dG<3u ", 55, (void *)20241, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "FWu63;)|4<-RSKESiE6HL.tZe9*m", 28, (void *)24183, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "F.?<B&x&F^Z^9>6D~7DE( 96Ky] Dvgdv", 33, (void *)10703, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "]fXS%*vKL]@r/wSD:;va0C5[1dPku_L9*%E5=7", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "!:+", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "jbpF{o[61,Ky+$L,D;Ks'4D4E=:r*xTTw-kwhoG?g&'p_@$f[>&vS7OxH wj1ed'Oat_Hw:1yYb(kJ:c7%6yd", 85, (void *)31474, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "K,RV^5=*K?KWtaew{sy]~Wo-_w#N2 c1Zy(,'xj=-U2EXES(Hx~??YzaE>vPcQrKHZDNT{ ul|4J(+UK;jP", 83, (void *)8161, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
  
	void *node; 
	int index;
	rslt = aosCharPtree_get(tree, "K,RV^5=*K?KWtaew{sy]~Wo-_w#N2 c1Zy(,'xj=-U2EXES(Hx~??YzaE>vPcQrKHZDNT{ ul|4J(+UK;jP", &index, &node);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    OmnTC(OmnExpected<int>(8161), OmnActual<int>((int)node)) << endtc;
	OmnTC(OmnExpected<int>(strlen("K,RV^5=*K?KWtaew{sy]~Wo-_w#N2 c1Zy(,'xj=-U2EXES(Hx~??YzaE>vPcQrKHZDNT{ ul|4J(+UK;jP")-1), OmnActual<int>(index)) << endtc;
#endif 
	return true;
}


bool OmnCharPTreeTester::specialTest8()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest5";
	struct aosCharPtree * tree = aosCharPtree_create();
	int rslt;
	int deleted;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
    rslt = aosCharPtree_insert(tree, "cw'?`RWD2*0`'2wRq7wea+i u,L*dL9C)8]P@^4*<;@chBwvT$Q+M", 53, (void *)19646, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Q%@-}DV&^9ecq/G,u^{Vu4'ZBCC@V#-Rx9qXq5#7Dk'D@O+VVD/!CGs:zbC&=$VDJ1sBh8Rl:G~<_v=0,3", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "D['eT/s1A)?|+Ki6aG!r1s(o&>_?3,~( AaC84rc9(PEZ^lm~y' z[ihTc[", 59, (void *)32339, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "xtXEL*)5HR3Ig}[Q,Qp~p]]%oKpKLzA<lrl Ir5Fx2*i4x&JYfs@1kf:vXTy$5O", 63, (void *)1181, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "j&aH.y*9+0R+t3HRf+q{:>7oP.W<)9c98i3]<1H.d?YIaN8r??}k@eo7t%z-*c7W^'V3Bpznm-lTM!$R8-|-JisiNY#", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "+Ba'g(y$|Q]a:xWRKPSp*+sf]!bONBU!Yx]EG:", 38, (void *)4159, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "7Ftd)x%7CJW>Lx('Z&}5`/_&,a2uNM0>BT`2HP#u849)-* C&<WdcNlbH~pWS6v_lTPd", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "6", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "j^'#ldo7E)8evkC4ES[w.=.khzl#xj", 30, (void *)19686, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void *node; 
	int index;
	rslt = aosCharPtree_get(tree, "j&aH.y*9+0R+t3HRf+q{:>7oP.W<)9c98i3]<1H.d?YIaN8r??}k@eo7t%z-*c7W^'V3Bpznm-lTM!$R8-|-JisiNY#", &index, &node);
    OmnTC(OmnExpected<int>(eAosRc_NotFound), OmnActual<int>(rslt)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(index)) << endtc;
#endif  
	return true;
}


bool OmnCharPTreeTester::specialTest9()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest5";
	struct aosCharPtree * tree = aosCharPtree_create();
	int rslt;
	int deleted;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
    rslt = aosCharPtree_insert(tree, "p;P[zh(&|0ciuWu)jl4q|H2rp%S(||*x&B|(8=yob=h*UO!T-", 49, (void *)14815, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "'lG`=1=etw)|4zPvmt{U5=$t)/&N{B-9C1?B>c,.]S12H->jPp25w@*!J#W;7s!>0y'P[`L=&nWAIh:qH_||", 84, (void *)22753, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "x:jOz8LQG&Z56u;%k;", 18, (void *)1235, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Tmm:5yW6>smBu_)pny8Hps@UUWps ]d/W>*Y|edx95O/!fHn4[`w[50#Q", 57, (void *)22680, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "*&L]{IXe89Hu&hXq;(tS5P?x-2P`x_WKZOa1CZpp5O!.~Y", 46, (void *)5602, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "+QkL*X.!_;&O$/", 14, (void *)9489, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "2wb>mN#ae5>Qv.V^U0^Y#n^l]0VgsS|aD~8fo[22)#A5c`", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ".''tsy$GM1YNC7D(d[*KUh,sAlee'Lw2AYi.7uQkkG>wT2PGOAyy~'Ur@#;.+j20duIJJBVtycPDd`;Pb$/GS", 85, (void *)25756, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "=|`LN/o}Y]eVBQOmn&kb]iD{eGwBKY ?1}59po<Gw9oOo+t+;t!0[KY~dw;<mc( [&F+f>A':>5xi3^4VJ", 82, (void *)12171, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "a+qx[S?TY0NWvR`}SPlU,7D67=VQ9j`eA.Gly'=Jo-l,nM[{(GbY+p'3At&7~oy@|A0L3h2 GK:kz,,?8Z", 82, (void *)30442, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "WUYFTtp_(;(pv&y+1O{^s_|+c4K{ecaSd", 33, (void *)24412, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "[V?gH_Y`A6H}fD$Fr*xB >5|]s6j8?d#pk/Z/:KmOw1", 43, (void *)5568, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "o]4u?&V", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "D)>`}vm,^1)92aR2zuEowOc=8^4vt}NpOY:X=XDI3/@/T3M?'86hzdi2UW#=jOTJZa]P}_Z$pQ5", 75, (void *)1119, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ".$>Lfcx1[buebk[f;rvrZ~^!gpd_(>E[?%zKs|=o+K8Njm{5A:HC{,Zg", 56, (void *)23625, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "?1x^A^r4'}w;7{~H^E0}DG,wnyb;Q`RXAb&6NWo6T$1$;FIL1GODDVV;_a1PnFci-auB;gSs'PwenKDoQ]AZfb4zO", 89, (void *)833, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "&>7ptn>+&68__}z-N5.I20nHLeCk?$t[H12el1.'&=Ep2i'8h}Y5u.qXS", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "W'=c2z@Qlg7oZm5tpEM8n=1!f0=dSA=c!@CL58_Ef{Y?aVO>q/u,{pX!#88Szkx:kZb2d ", 70, (void *)10616, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "5~zx[(gIFbI^)./--{DzZEY]KccVjhJc'6*??H#q0u: r6Bfo&jK$qSSVj`9+c,q |IXRQhxTr}|]CZ", 79, (void *)28215, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ")", 1, (void *)14806, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "^D<x-vpw$e96(vjEZlx#`3IozrYO", 28, (void *)5172, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Rsjv*B`!", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "lF!IoY_;N_mFNxbqn?fwKa'}o<v5Bd?q|$kap@hc$[}n= n;", 48, (void *)8072, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "YH;'Eztw#y!2<>dC22:*)+akuH[nCyf4lYz600nVkjP-,,${<hTuxD[AEdx~n5OV{=Wykh:}4:tqtc0>v(eDSDmkC3", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "*Tz-F/!X1L87G0ACwmIJ-w%nZmB/", 28, (void *)7122, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Y>q4!{ISeM~zhZmztPAnf!+,`xe?5QPGmv$r(qzW_<&<,={DN48rcforYK*X$)>$0?6(&O=I=Sv[RJW}Lj+|BvR(4", 89, (void *)22850, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "b-P[l=EM_1U5j&'@9jBVO+@UJ?6xIAyCGqf:wy-l0_q_QeX>3J}>0}Q(7L'", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "JVokTuQZ,,@[;<)#c3l~JgZ{VdlimY2[?enL5xIu.p/=}csR", 48, (void *)3969, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "!X/Mi;L_}!Tr5,w}Q", 17, (void *)5863, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "-/+eihfMuRW`L}%Hr9@G4WW*v%% Wo*J>PO5.-,Mz_Lsb*,av]VQG$Nv&D9zr=qM;u_Sv't<cq}mIbl+&}2(^Iew", 88, (void *)4273, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Zi1Ke94hO/x?E/{lJ-mzsjcyceS)", 28, (void *)25763, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "1B>n)*D &]<]fm1lm2v.sO,')J2v.jRy`h8GTnH{)ZpFo4>>EmMbC94=S6POka wbd*)>1XI5|", 74, (void *)22993, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "#rE82GCR)mRUg3}o%,6IB07c;.cwwhwf%uds%.- OMW}?}xrI&,GemAgkD$V``-Buu&q6CRxJ-!B'QGM8g9ei", 85, (void *)28164, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "P@'#Zue9EOT6LraBG{4An/jI nfQ4p;.v*vy", 36, (void *)7538, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "WBKW2_U[~jaBC=e0dvoDVw2cTW?@}^k:>il.>6", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "P?w2gA]b<9aJ{80sm&61~!3'NM9JU0)kGJB=|UbD'+N[IS#sr1'w)&o*{l9b.n4HFADL", 68, (void *)22374, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ".|)Z}XDW{5%nBZ J6l", 18, (void *)30536, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "dq@q;v|#XrgK>|H@y-WDY(I0eywmWyXzJLbWj0[", 39, (void *)6799, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "EvHs/C%@/fZ", 11, (void *)20701, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "?Jp6d?dST:?/H2x@4nTQ:)dc+eF<vc fPM..).W @&NS{}8+*lE", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "H{xmi/ZT", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "O:CAJSgpvRJ1`@VF0OD9W4", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "lMl}2G4 N6Joa~L0A*ul1`^C,7GiQ@'bnjg5;o@71yfE9F&x^X8kRc8S3iS`", 60, (void *)3799, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Sq?-NsUEq[RT5^sxdkV4OV!VrwDxP_K<3t|8+)?{uS==]WYdv_Hx,5,LSk^4Sp=#Fwo&O_FEXkN(", 76, (void *)25071, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "OP0.WG nPa<vvr`6p-R>&0xwRkKW]:Dc", 32, (void *)9076, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "DL^6@5/JQ02U!-{NF%:-weE[&N`:Ov?vovT=/zRT]/W@:?", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "bhE>&|k4RCKmc", 13, (void *)3126, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Gp~;fmgg6`bfDUiaY,3|u_<qV5d&i*J6$Ee/E=.Zt~x<)yv,L>i2nH/'0]y1+9RNzyVRPeVvG NkG$Q#n1SA|n1+?[O+e&", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "6M7$[*rS4PM;vlcP3uBnwEf_KND: zyCF9fvd+wxs[ocRtX?#4hWG4+SMP@gvg>R$W)IA>=vB-k{2^*3qmf<a:", 86, (void *)15486, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "!(q+_-.3_b<HtbE@]]NYo!t|G8yW`;v87<WwBiQJ(A'+P~#?g;oN", 52, (void *)23818, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "DsoFT4F~", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
 
	void *node; 
	int index;
	rslt = aosCharPtree_get(tree, "t>lGy)N]Bdx%Ya.9!&FD*{cjA*]h9i<>S) r+>n`", &index, &node);
    OmnTC(OmnExpected<int>(eAosRc_NotFound), OmnActual<int>(rslt)) << endtc;
	OmnTC(OmnExpected<int>(-1), OmnActual<int>(index)) << endtc;
#endif 
	return true;
}


bool OmnCharPTreeTester::specialTest10()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest5";
	struct aosCharPtree * tree = aosCharPtree_create();
	int rslt;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
    rslt = aosCharPtree_insert(tree, "9}o8O|=E6m+dSRRoW5@&V*&Pl8aM?I31Q#YOGiQfE6EXHKH", 47, (void *)2023, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "9", 1, (void *)7999, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
 
	void *node; 
	int index;
	rslt = aosCharPtree_get(tree, "9}o8O|=E6m+dSRRoW5@&V*&Pl8aM?I31Q#YOGiQfE6EXHKH", &index, &node);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    OmnTC(OmnExpected<int>(2023), OmnActual<int>((int)node)) << endtc;
#endif 
	return true;
}


bool OmnCharPTreeTester::specialTest11()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest5";
	struct aosCharPtree * tree = aosCharPtree_create();
	int rslt;
	int deleted;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
    rslt = aosCharPtree_insert(tree, "b+)g@o]![QrGw[D?lG|zw`42bplKAt!DXYbhn+:7*|}6]'8FH*1W#[v&:1gbbnR:bfKm s.n=Y`K", 76, (void *)28867, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "E)gyLg[WV(}r[ofkOz.q,?,sd}'Kn/^P?`U?C<:i~{e}mCf2", 48, (void *)27357, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Y]b98cEg+-CXB!I %C][K5]5 }:}Zi!V|f4|oRa>bOAOkX7dvF.8_p!aCV$Z9+F?hgn`)D?", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "E)HHM|P`goM@6Jh6bL7j(wb@4c'o5g-4]ql-n0]vt~m)G'{8*t=`Q$?lor", 58, (void *)32356, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ">IuTz:G=/!0-vCbUi1;KK-sS>3WM[i#b*idv$_->PS6#~[8ea?(MgqrNB<'~HM4EbgJ>:2O", 71, (void *)24433, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "!V-dE", 5, (void *)15358, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "i<Y*,h:7ecK9Y>dxdZujS[TvM4 }J|6]s,6N 8E&f;D>c@AUiF{Fe:`p(p;lmoRPQ94bu-9qsdgsV*:zBI7", 83, (void *)5606, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "BK%Ew`vT8Z1]$xYJFaP@`?X9N'+t`4[-_628=%@NB1Z)beV$.6F##uhN=[oWQ,7(;YG,M|oO=-oZRgWqi?", 82, (void *)20364, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "]Jde#O@aNI`UX&z20>-=wQ^", 23, (void *)30344, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "y.T@-I6[AV#}nR$L:bZEx!FG]a~!IQ", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "uwna2F,<{X*|+gnK{ x]._,0k8e^{R-0`vc2/t{PNaD55EfeV6x(", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "f01OCR9uru|.DbzR:t4|7).g{z=@0$M:VBwW/vk^&3Yn;P0}BJSJIC[A8>Xx11LBU^R@nJ.v4*h:y~C'xn06l)qR>", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "'uo;MrN:F3b2skSGJv&(&n,KjG5_a|}RGs8gv;QO<lYDJiP6l~GI, 7~v", 57, (void *)27074, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "02Wr!jyS5N", 10, (void *)19429, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "^SH?h(X&U%69X5Jo[$B+zw6/<FK'xx$T%=;a1OmcP8_v<7IfD@/4I*;X)'I<T:iSs4]WALRy:+4pFQ}CL1xVBHFkRO!", 91, (void *)6906, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "fX2.$8}8*[#[Yx:L&pm8B|SY#@#UuyL7~'Y>ll0(nh)=3gb]!0pGipCrq09/3a+Z_)y;}eAs?~{HA", 77, (void *)17968, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "hIGM-YBrYG ,@{QF!|y]`6al pFjnS_w", 32, (void *)27050, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "!oCX0%P*fQ'c?8jn_TxdEya6#g;Nk(v];", 33, (void *)8143, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ")W0lwC23Zes-iBqp5i5&,<GTXZP4X<@:7iTpSt1.%2gWn;j@Rop*", 52, (void *)20023, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "]CR@M{eO>}98&4K~r}>>IW)#>)6oFdwl*pQ<q|&nJ$N)}-|Pdm'z({Jp0wMFT", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "x8V:*P^&`M", 10, (void *)20822, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "sB2XNV(l,(T?mz>'uZTwG.4W|2:WnOn3V1_uvD$|O=g/bAh4<t[Sc)]1Q'Zm+rFnFpyp (e:yKSJjn8o", 80, (void *)1190, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "1W:z:zSN#qG)mVJj5WI8'3(oB4%^o_@]_PnBDH%3[@wtUW866UJ0eCq-K5b?@m", 62, (void *)27241, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "8!cjC`!-?l_bL7>>$X`i0=kim^[<2w%''", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "^,:JYptVF$#W8ojB[qIJYBIdbD#.#*;,b>}eS[4;6+:&jx", 46, (void *)15770, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "s6~1t5,2YDs#XDv2W1~a", 20, (void *)27001, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "#Y9x=7R,H0S|a:F`[7|XYd9vJ5|IdcqswA", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "R3x5leE=>vHZ`}}@*N7/xMC,d9HMCW_52/-^?-tMLwa&Ia(ZlZ?Td1T9I;+e&", 61, (void *)20695, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "vJS-yx*I;6B>xDME(TTUE0", 22, (void *)18444, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "FSbUnPNV``TSq=Yf?|7vl5J.Utx.YF57]V5x+n?Ti3MiM)# )%eeXeKz%z)FdIy{-!@~eV1]5WWO/g7QX_UD*9b+#p}", 91, (void *)21777, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "h{{AF~LcvEh", 11, (void *)14477, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "LgjAfe?I.uzA-`JA4~?)K4n`|]+at4bs#~BLit/!z=>,Ew<SCgI7A}^<SQ={`5;Wt/o|J{5", 71, (void *)21531, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "j=sCM_Axizu4j5/0r2`0}Mjr5d*HFW*+S;,[W~JHi|.V47-z_FyW.I'DU(fD'~sCY$}oCK>9I3;WK2CE", 80, (void *)14207, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "u}l`3n03??t2u0^=-_wjD;GmFTS`k%$:qGolo(T!KYDwh7]QMv3c", 52, (void *)16960, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "qTaUt@:be[=sgE+{Eh1!_YN1TsCuNkUL)9f6pOrVQyj3D~ccIK", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "}V^*{*t^Io4L C{],9),<THuMGI[JF|GpQV)pU^E9x-+CCX2-O&xU8Q9<'9Qo", 61, (void *)15000, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "fT:kdK^O91zYQpP~7]]&;rg=g%f*?&-dSdw})=g{paO3PsQcxq#S;F>ts=hVie05.4CU8wM", 71, (void *)3266, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, " Xr9E//>TRZ#0d[E'5#nY.@&`++Z4p;]8]ZF./+Z^Xc-ff,9|unJzkg", 55, (void *)8753, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "{RYz7HB9f[{hP*g'Aa,u]M9/1;gEI&]u+/&8dLDLMuu", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "&wT+fu.? sTg8~x tT.GJt>b@Mq!e}=FK7{( 2aI8`k!KYbGJrrb I=kpcLrI?$", 63, (void *)27778, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "$kR`F?}Y9PCN1a#Yws7z.R{u>qP/UG6{}D3#%u?o uPG[Kdqo,;/+=7W:mn6#LP(P6G%X%[&S80`", 76, (void *)8701, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "#MpZS.(8~%`", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "* NFJW", 6, (void *)2649, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "kq@qM#fH#YwL|Y*/`hDoE]td5R,tt_O^oMpmY5SG-x,{9eXj_EAq&*xRXC-+0.7Vqw_#e=Z3!", 73, (void *)30903, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "d0qdH4-{d8 #rHs%@3i~+)-DFG+xJ){c", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "n?5>Em@4=739!eSn=<JZ 0", 22, (void *)12747, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "E@+UPW'`$6_ctS8Q?N`CgsuQ", 24, (void *)9674, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "hR:j", 4, (void *)26065, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "knS~3v?!v1-]52T/3,F@ia", 22, (void *)24481, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "3z3hl7fKF`", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ".KTUPqd%n^VoXSucxS,TCm}kPVGcIj/?_XZ.e[;?@o3T2sppS-J}OyH>qjdsy!,ecwB@MYRRu4k2y~Ja2H|*XVF<", 88, (void *)15977, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "{#%7mv(sAPm;ihV.!Y&~xrJC*dA]9F2U5%a2]brK1YjNLd$0rT0LTn33spKZ1|cGn", 65, (void *)31730, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "|M8yTv `~,h^fQm[J&Dxxh G9c%rg(jqZ*`936vvmJ/AOJAVa[6V<pgVH$", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "rVZa'+lXM6;nPDsRZqG%E A=BzY^w$V}B<7oG X0E+DX|$#oB{Ep4o5akpEt&g 8ZC&9mzI<IM", 74, (void *)12781, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "7c1G$|_", 7, (void *)15027, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Oo0:YCzTJ7B?CKt4IaF:jaJ@WgZqn|%+jkW$M<0", 39, (void *)20556, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "3wxPm8wesz*FwQf_dllbf$N", 23, (void *)10515, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "g7I[*KASsAt.bb7-gsA>Xq5@!FC6DLh}?$0Rb:s6hA-e(J.|l <-U", 53, (void *)23054, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "O}{bZ7F0(s@v`p+VjLc>Gm4<zp?{pC)j", 32, (void *)31298, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "D}el!iy4hq@o7vh(T>(a2oA;}.F<S=!A[**CO+?M<gIgS<ahm%18", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "9G4+/wDgUt2^?Sd~cD_YP~TPr<>za%Z(r| q1Is%cayFdY)D'q-%Mp$u.G,`|o<M", 64, (void *)12324, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "s?1L I`7uUg$vp6y?4bC&W) ~g[;t!^N>nL`<_.MtNdUFfw*o$lv'y4)J)8mhrJ-^pV^_uX$RTjehF?x8OY bqOr`u.", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "]5OOi|6sz)If> bm2y'K5k1@s{lQ}/3+z+%", 35, (void *)13924, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, ", lxOFn MM<5T8:5oI", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "M^dt#aMdE[M#Jq}o!1^#mLK", 23, (void *)13497, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "~MGI#lS", 7, (void *)10281, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "5B-C3a6b]Wl|1q(0pf5-wFEKJ8m/TlF^?g<jYE3Zaus8i(", 46, (void *)20947, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "K}Dnp#94(?b{RsB@spzx%c2tpDOF[Ml%G<GHzzfxa!@.dBr ajG%R[AN4Fmbp$}^_vb%Y'Q/", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "_ozhpTN}v' yAKv[DBE&L=J(!'8dx*r?MfXD#~+yK'y%`;NX!s)3", 52, (void *)16509, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "@&4QPI3(>'lQ7amd$FVt2T,-7-{lg[y,]L~LVZ#r&PYhG/jNX6LwCGmwMY]`u[<@pZ^C1B,Ul~", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "y#M(DA`IkaC7c&9)8TbdDvy{b+.a8?PjX1UKsLL;N-~-;h RC'yFOt", 54, (void *)28474, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "O,4pmQgmJ6Qu`!FEGkwiP#o_U.-L?G,f}O'+CkX'8S5~p,$L1nuRRg>xEoZPH/?wxh%/}_", 70, (void *)1175, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "l~cGTe0dE9*8!*n", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "DF4c ~!4s/XR.kx1[$@_vLkU2Q.dj9D{~!8G-9m7<=)h?g2U{c(-'VtRAc,yd/|woZX:Uc^~{mq50", 77, (void *)31826, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "X<0C l2", 7, (void *)19616, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "0WW|8D,LiiJ.9ZG2LCQj#.w }E~vvLz,KD=ICTYe#Q8{%NenY?Yw%iYeZAzO>T|udZ", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "ei/ZSp;rZnw4|DzLKDgp!s'CU9*q?o{+VK|ksR,!^pU/2eD=km^A.cxt;tTY&71o>6<>QdK}=eNr$vBzS^w", 83, (void *)20344, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "P)", 2, (void *)30662, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "|Gzv9?G|]xf$oJD[w8W^3DIA'YXX=jK@bO;W,MN)U&%*w @d|PhKVby%Nf,q`'(&^7~ypbSTK%s$v=*H':G;}|zjLfaZ'", 93, (void *)18292, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "W>RT^5zQ}k9&1X<Q }w_?<|yn[Hb", 28, (void *)11835, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "k.Rt*%W~'/1jA=U8#hZpD>90[Kb0K}", 30, (void *)25358, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "IG^Yy])*q_3x2.", 14, (void *)24828, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "[5|h-|B#aUa~-v[IjaqlT#0^|3`C7sjcqmBiZ0,=r~;Qi.+)xGn0 :F8j-XKGM.Sn^Zy<h>S<+<YeNsosW!l#:hdqc~b1", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "{DG|nY#e/!", 10, (void *)32015, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "W^ {Hg7?%lQ:DRRfs~Se!L=+C}XkS;ADi~J4K'Lt{qXe)>E!5cN:Os8E^6Y7Rw|`gx6Z:h>k[R],}%.ATovK0?ga$gl", 91, (void *)4004, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "A9!.L#i21e#mm[[?wz8S f|;#j)TjT+YU9dm13_}iZiRK6G", 47, (void *)30936, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "!", 1, (void *)27070, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
 
	void *node; 
	int index;
	rslt = aosCharPtree_get(tree, "!", &index, &node);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    OmnTC(OmnExpected<int>(27070), OmnActual<int>((int)node)) << endtc;
#endif 
	return true;
}


bool OmnCharPTreeTester::specialTest12()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest5";
	struct aosCharPtree * tree = aosCharPtree_create();
	int rslt;
 
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
	void *node; 
	int index;
	rslt = aosCharPtree_get(tree, "t", &index, &node);
    OmnTC(OmnExpected<int>(eAosRc_NotFound), OmnActual<int>(rslt)) << endtc;
#endif 
	return true;
}


bool OmnCharPTreeTester::specialTest13()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest5";
	struct aosCharPtree * tree = aosCharPtree_create();
	int rslt;
	int deleted;
 
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
    rslt = aosCharPtree_insert(tree, "tt", 2, (void *)27070, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
        
	rslt = aosCharPtree_remove(tree, "tt", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    OmnTC(OmnExpected<int>(1), OmnActual<int>(deleted)) << endtc;
#endif  
	return true;
}


bool OmnCharPTreeTester::specialTest14()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest5";
	struct aosCharPtree * tree = aosCharPtree_create();
 
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;

	specialTest14_1(tree);
	specialTest14_2(tree);
	specialTest14_3(tree);
#endif
	return true;
}


bool OmnCharPTreeTester::specialTest14_1(aosCharPtree *tree)
{
#if 0
	int rslt;
	int deleted;

    rslt = aosCharPtree_insert(tree, "]3u0*[RHkZBhfBx}K,+2ZbG+R2FFRm1-2;/b:,SDU|D{dc'", 47, (void *)31441, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "<j8[YrYzv78:h&O~rDrMN&Nri6o/Lh%~V=<[&mK''0!<8tv!SQ-usNc0Ht ,tVVmVbB$c]09dn(l5e~Buy.}?", 85, (void *)19605, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "YgW`y!&8#wKdw)dq$B{Bfdu;@+>uNQy='#d$tT1lfk3M%b+TnG't@0eAu%''u1s9|6mKYV3}#BF6!Qjto", 81, (void *)27506, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "$J34>v/BQmX*y#nUFh]XBr8r.io<^Pu|Z|NumJjp~O,}AtZ=KBx}+X;e%MB aU2", 63, (void *)3447, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "qKCptqXcB3<>oe&gEB+bd!'(~qYj:k^GGC)/mII3yCbEW3fVLq]}*Yg=K2", 58, (void *)2585, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "j2;EcE(<u(nn-`u:NswOtlEf4BBV<W", 30, (void *)22718, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "(sfW{yY=9V~i;edGIe2sH!o%*W!HrfrbL$P($1+>Sf_4I1<@:a)x5NKfy T", 59, (void *)29257, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "_9`Ra9n+v>O5-uIqv$-]' I2-NK", 27, (void *)21998, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "AdXS", 4, (void *)15379, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "hG(^#YzY?}hJOX#9!V(3Hn')VCoNS`Ii$LMAIe4Vu3VN&@%zUf|nj24}CF|;", 60, (void *)11149, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "B:0A8<><pU`0rFZpHUL,O[nPP4S#[e'y};d{4OR^IF.,d51;U$03JhOLq-xM+qs!Ay@#1NE`J0q`zWD0G)56s:tt`Jnf3z", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "+<}!`>8V.hV;-?6}-aJ*-j:xBui&-'t>&bt{leEHs", 41, (void *)19798, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, ":$}>T'", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "lyFq5}'@r.UY?67otkTswr)4k'X0sA3wDYNo}M16'ioeU0@Y;dcQG)3<RA}F3FD?X%d31M", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "}};:|u`wYW6v1t E5", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "J`JYzE8Jpa=7S:xErD]XvEr4uiphfU0}!tIF^+~6m1K.!hK`zY<yD!Q(vUXC>3iS4(<|:C}:NZtt~r+Kh<W ", 84, (void *)15691, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "B3rb>j[>iuNQbx)E/ONbivDQzg5I0+)W:'|j0'g7L>H.iOU<&gpFf-%", 55, (void *)17190, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "PhmhQ?aisIQEBTD>;Cg", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "xqEL<lZGZzj,>XU}(tlF<!|[@Jlc@9tpd}s{Z)X8%sx9L>T|<B(Vf4Gr34Hdn%v;:Vf*<(y<X", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "|P7+eXM|sM)Lr w?3oA#dSKjTyos9oWk}pPIkV-@R_2:M9qN&Q;NEH+{(C,xJ)Q<C6qUG;o+m0|NBJKDPm3", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "@%>_0mA{v,E66Ep7!Wlf9X!{s%uwX<Ph}d,)L#5%6yCVA;R", 47, (void *)21727, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "STSV{StA~S&:'@5guc<D7o)Q{H1OYqUb^ik", 35, (void *)20758, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "yKL^6H[#E~`EQ~udaC", 18, (void *)27384, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Jg)7%S0c1 BkvN4DE!'G9EyK:sA'Z%[%)jn4TR]BBF'IUPGo+Op;X3.", 55, (void *)19976, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ":-P'Yk4_Tv}|`q>Y0L._<r8JiG$p}?B#w3xh9eG}2bh4(.X]Mq1N2Y&| }Cq/5y<FtQ", 67, (void *)28966, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "`5|wX[M)?VV}N8{E", 16, (void *)26362, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ":@Zt/2aRN6$J2#d/{#{VF1'n97BNYlag*EAv:;HpsAzi7%wqil-dOdSYYTiOX", 61, (void *)13348, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "zve(&:t&0n3KoW8_?%i@B.(@_H;d|M=35%K3K'1Q$gU ~S|X[WKPx:Hm8M%s{)g{8i.bat-QMTDzUi?c{", 81, (void *)24828, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Y(~~Yz$#`y@)oIjEs5;%7-9fRxjau)4UZ~X$Kdpow%DyanSQ", 48, (void *)25309, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "=UMDoHFAktcG_[6NiC/NTHx`w1qI_GO=w", 33, (void *)8545, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "j>%IBy7M5!F_e_:N", 16, (void *)18276, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "riW=4):~tj+]cW_'zsjRx[o v4%5=_/l5[<~hidb[G@hl;Gx{>,Ej36]]lV^:)@z@X+Uz", 69, (void *)25741, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "[_yQ49ZjT5ODDu2fdxQ..U,-Tj`?FZu^/O> d", 37, (void *)19352, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "*5APy'5r.,h$R$l6tt*(/>;5Q`$'nvKy0h)ye^9'M)7Zt$}pFlwX3nnUo/Zx_Lt<", 64, (void *)31808, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "TpT&^sGs~|;=WmPX{nob{7", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "fGNG>XpT~%T%}$,T+iM?_,`d=vmry{B+[+p", 35, (void *)26067, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "1'WWWDj+(eHq],_:.YStE*D.u0!c}ddqoIIwdi^/z!pG&kvsO!8'LTMl=/j9u++if&rHymA9QeFWTy`,+Wl", 83, (void *)16186, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "E>G%L62", 7, (void *)16415, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "AGN,CYvs9V!{YL/mMRe+-[/P2VK$}U{D-YV]X(0xA6@j'&{s}G&hb%f.[eY9Ds_Az7?>JPa@sSVmij]8YJm", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "3t&", 3, (void *)23241, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "]a9v~lB0vKa<3?MMReX|HE4a;o}H]q|aw? J*]=8}d5+%x-b`ZpLUkUNu7j!Mxq~U}h[Z/)g;dcd]Sj((a/h9:w,W5EB", 92, (void *)15553, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "+>:GN(nl;h1u9/sJ&}K1)A0sp7yzfyL]TmwJ$F>1B]6>TYL,K/YTT7c^yltcUN,tzZ!7T9 DpZG??u^%,0+j", 84, (void *)17661, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "8|gUd+bm+Bdc-Ln%)z3Uqxp4BDW=TjuJ]O6V!S5B2-'(*J@~", 48, (void *)4622, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "E_v#)tQ:Nk8_3F9yQCh>eh-;'I/<0_@o4'BhM0kSi{dg", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, ";AX=xx*=igV6U@EqiSmo+R", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "R$/,w$2X{[6/)VXUs+]tRr`=GWdzM&X7Kcn6KE$5yC4NJ$KH/A)Pqsy}=b}H.d>ON?RzV-F-+^fCL=Zl", 80, (void *)6527, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "xDQ@0WRhyUzV<.2Oh~GQxWX{`$,#", 28, (void *)2104, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "%sg,o~6lCcd18^<`wI>@;#-|<$`E0m/d]|1k{h/zNFu8x*ywegvja=q|!~eG 4=5yG&,;Ip6N` 1V#1X[|&/@C@*L.", 90, (void *)9996, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "eJoRh7]?:Qs*Ar:6Ls:Ve5.ErD", 26, (void *)1444, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "csy??3B>", 8, (void *)25138, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
// Entries: 50
    rslt = aosCharPtree_insert(tree, "5QJN#8r7klP'gpzsIuO'WXT#'W<sL=.:dxL@rxa0:bY@7B_;n(TY52", 54, (void *)2346, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Q6KTC}RF[r!TzxcP}sUTcRfq)j@@A`^h~ ^??__8m<SB.W=zSz>,irEW|zI!H]e~ M#l", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ">a_W`k0d6E,$WIE<LqxA'GR`", 24, (void *)5777, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc; 
    rslt = aosCharPtree_insert(tree, "B%_(`Jc`@{}:J5%&&XU*C9~Pl$',rs!o^%(wm&I-#pkHgG#t*/fus@Ae5z5>{Ya7Kn5@n?=dF]p", 75, (void *)4711, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "GhpGJ", 5, (void *)9133, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "B<l2jGiZWXMX`:z]}<47!NX6", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "W/tWh/;VInEp", 12, (void *)15481, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "X7'-ZJ6z2zcqqRojNax/YEj^22<zdsknvLRQ^Ynh^IG/iE, `CTpl+'ssv@V0@O$jy@eVP)i c(<}FWs`40:T%CPY", 89, (void *)22980, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "6H AREop,kOB/BTT}fHR$xVq%0?~C: 3*IK_f", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "K^um $WB2d0Ug}`;$s2*!p)yT)eEUJ9!5%t+cK3jWZQgI=6Y6b`b]wG(", 56, (void *)798, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "SX!d<*YA{e9+uX|RUsu5grf%Zk4#3($1K}=#H,l?|,5)  ]xV/T5L5C3f", 57, (void *)15763, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "?L5`#Hs!r1g$$}Ja)9N3%t<+iT&S'*'", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "L$x7M2y;@X6n:)MF'+wr<o8qEso4IG{Q=_w/hdJOt~QY>9!1($-%|wJ;iM<e,c.@D$e+?PVOwL|A0dxhkyS9-", 85, (void *)11274, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "c.[^a_C=5xWu/g}~jnGvpD/T lx?{G!X3d+6F%Bq QH-.>JJiv=U`;J(yiae~g' tAAJ`>TC<5", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "E/Q@PCq$+LRBkLr_&70h8FYl[ So52h[% 1DNaP#'UH0eNey+JZni!H2kD_j#-:g4xTG(OaLC;S", 75, (void *)18603, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "a4#(gt@P [4O.*J?jwegCDQmm(d1YQ[G[*qunGQ}ksUVh[=T+ZZnB&i>Ri[n]dG", 63, (void *)3474, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "<_oN)p'Md[iy'peyDj+@=.JqrcGuVX7t<SP!", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "zoS)o1n][I.$b[7]oFF%q#gLG: M*ex%#1;a]=Q|.<!)3}!OM6|TO&;(v?q]qs?x", 64, (void *)24673, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Qd-x{FVxA|oK`48D{Jz1nfJ~uNX*<o;Bz@ZB[eC[3K#KtGSi", 48, (void *)17826, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "}10m", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "%f(xLhl;s%Pby5pCZ4i!<({S]1Dx8cE5gmG?", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "+r8gUysM/r.oT;`R`mfu$pJ9~mSV6=uV)CcG4h}aIxXl.Jlx,`>)=jGdd)lstAcT^s'=8d^+-NO|O", 77, (void *)20464, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "t2w>4tKytpaEc/e>e}G6+SU9q%/p+S|WU59x;R`*?CF[sz4:ER{T*.4A2VB,KDJ(gZMd/I/~d!7aA7:x", 80, (void *)31234, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ";b6hd[SBNum'7XVDx~q5A8W,UZl-vq)~AA,b<}#g/Y:`D hiZ'K'bLj.dDt#%~};j%tn?(izn", 73, (void *)20839, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "$HUNdpANG~hu@t~7};)K;R", 22, (void *)18433, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "9>cRU<WQt_cUhOP&C7x)d-_|A,diEsnnM", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "_iBZ$gZVZw;(Z8!|2%7$&yQ!ZR.`:~Yc3@c?ikge'e]", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "?'/SH~cMEr10un,ZTfs. `y-5}v<o@ts9#B J s}2WlUZt@*s`0('T[Cak'&R.5f(K%MtG; 9.N^", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "$S}g}!", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "M9).aZ[EVPkF7?'}Xo<A<Kf$#Ri(:4O]<nLYwbx_gV9Cl;[Ac", 49, (void *)30962, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, " tzn#2o:+:,oHLC}p!1iwWq@>N;YT-DZ?l/7FQ>gD)&e", 44, (void *)15783, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "7>(zq/KNmV)A,L+k|w(h'[b0Zj>6F6CKbJA)EnBi/|D)2~dl)~Ef}uC #AfIAb3*", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "-QC4]&qMy.t3:Ic=", 16, (void *)656, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "/{I>Yo_9'2B|]Vn&ln;9[:POOU*.%ky[V2[", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "ES9_qmdr]%0,Nu3E514;Yz<:%I@U%C#t/.O51/4|_[9_qH8!=(|?kq .ZM@:M", 61, (void *)11354, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "kg-[c#x)/MEO>)V>DFuV!94F$4/P:L>4CAqR.qsh]9&;_l{6RL|I{-%Wjxe7Eh{+R/FUOLKy", 72, (void *)8732, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "7p$E8ix1/IpFK%^<BP@NH V@x9k7znrP3~ro", 36, (void *)11433, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "[7#.f6/:lq4.)%`D0k", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "g{~VwfVRrOs{n'dJ=Sh{.@y5[CPvm[d)P}P<b{F@Aj`E<W;`/Xnl{[{E4:4ijnXT''K81[`#", 72, (void *)17426, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Cczmkws+1nIKS.4*B3>CN@j@B[B8rU>5LIqz7mCD2epQCGc/oAevel$ZmFINNoI7LH$>7`XkoMo?Zb5j?TW06?GyV", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "etBX7c*=C+~W46Ia FU;sB$]eR`l46qr*^ymW=[(V9pDj&k+m2t~$XyyF P`4'I#,f|sI6V])ZO<00 w", 80, (void *)17010, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "!C`z83,5@iEe ;}B26ZAr*:c3GGI]* j}C45% 4W#JFB~fk,yH|Idv-E3+BXs*}nw6<Ps2I?I4a=CPS HYDjMO", 86, (void *)7637, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "[x'k%G", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, " ~&ypI:<MTLbB3l)bR0jKb74=MB|Q;*$qIYfNPEE{./oNdRa.u,$qQn7:lGGKd~x.3S`alNG/AP*%blF^4 j|MezH@m5Z", 93, (void *)16087, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "=<s8SZdi6`)4a.~7|bW6{U7!Kw7,(;jUZ7shPmk`'(+vc=I*FwJtp(", 54, (void *)22475, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "sL", 2, (void *)27110, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "TAy<y", 5, (void *)3229, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "^d`l`.TOl:3{~F5YvPg94che[hbA,*J+/sH`@9N7auG_[PLT9>tf!~l4_]9[g>3jX6}FbA", 70, (void *)10183, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "tK{wUq3z}:~3lT-=Go4B/~&;)JYiQ$MMD{kW,n'q!LH9H8Ee#B36|Yn@N*@6TG7],J.StHw1GZnI%9pRLf3w$BWRv</nr", 93, (void *)25644, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "K<{ST{$c|hPb_E2w>4<~]p1,+S09c1)g@oi6Y0PG", 40, (void *)25664, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "do_3a]V!G9n;_}-tjtWUf/4**0eTS!z*<f[U91q`n6i>12fNFZoJJ+QM0k}l!g(g5$Rk{#(xJe*K", 76, (void *)29833, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "7Z'y4x$qT%Mi0t3DtwT6D5on4 XL|4QUN36<NsL", 39, (void *)16134, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "{QWGHK ZSB,9'Uj#XrDinw,%V@KP&t;1?|~(G8r3F(mR{`w_[KZO0Gy`&*ZI^_", 62, (void *)20404, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "l#x1 d-@8=0X-RG.7!<hUbwTEfJ$0@u$X4N!tMR<Hj<_", 44, (void *)17153, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "lOYRXR}6'BR{V)7855K?cWnj:Pl,7Kb]/&ZT/9X#p`SHKvdV,8t;zB/Ut)", 58, (void *)12811, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "8yF9c.E@&6Z?OI&?#([U}PB1Y0B4bc.L%[bB`G(? PP!y`_|Q8/xY>r EiDeC", 61, (void *)18953, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "ZC+6R7&tC[qErF&vg=&", 19, (void *)28524, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "fG!aT&baH'?'*>gb!L+OxA!~Ed5sU0AmsghTUv[J(,QV*.Ez{cx(|_1;ldx4A`7D8G0i*|%rc~F#kMZ I1uKI -rgI/cn", 93, (void *)29095, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "dul5j ,Bzbwon$1iXF!rv8hi/&7,ZeQ[ ./", 35, (void *)1080, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "(k$BZ~XXiSHG3eH", 15, (void *)23614, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "+'+~eZE?_s*QpUR%JxLd~~UPc?w_A[t[2b[c4tCoz<+M!CdV$ ", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "ha#cQ6B$s-Mt{<GM2?L{6+ fUtt+2|c3F1c9^O$:su1j.{^%5lQv@.z96MoC`s[2!#M.9sm0ek{^W", 77, (void *)13016, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "pbI<abG<@$[7v3#yDvxsF2}eHLs@ZV/VmGFl(Y@hff392etHDQ${3kYy4q3uI$JrLMeru0&a$+1.,|Q&[;cJFv] Ke>", 91, (void *)21919, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "5|t", 3, (void *)29273, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "_NvU62{{ia4cahrXf#H}?{HTvyzqj)-lJUB?YF5i-{!7I", 45, (void *)5881, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "||hbd!-Q**<Hc,<puWeNd(DpGI#GSwRy7qHn mjNt", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "8N(JCV@MMUEzWO]T#mm6bxXfJ.|VsvN|hQ^7C^L'+Ax]k4jGipIv:sw44a.[yC)&:bh2ht'Yk2hvU)-dZ 6|%fp@", 88, (void *)10621, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Y{PPfAv(bIfFM`j4q^&.DqW", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "/B72~A}B^V(|{eM@h", 17, (void *)14279, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "pkZ]VvC~`k4}`v|'wn`blCmZ_s#]!'BM:Ucy/P", 38, (void *)23355, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "x[{", 3, (void *)20166, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "ZFi sV0V.u2FdKfUzsV~NBTl0BG4niVJzf Z!5`9!-9E'y`2,Kp%*9Eg*~kM24{fL~OH,E^hQ{; |VKo=U&t'Fx4#P+", 91, (void *)11900, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "G8LitwBSIQ]k)rq[N+a1<uEz[MWp4?nkd(]DmJ<=5V/w$D&P?>Laq&R=w?j*%^g>A1?,Q&Vtm/@:y}F4|p+OAQP08", 89, (void *)518, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "B@rYgEL&6W >L%`Ye-", 18, (void *)32266, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, " s[.64+yA:0G689](~JT;nqJ1p;}-Vk<c96/slA2n;56B@zmb]Fxl2.|;Y-", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ")MasQG*GZA7zRs}pk rlX1*KbU[ 2HtClGVkfG5wT.lLY4Ds@<e6w=*!r0K}t?mdGKI)i", 69, (void *)32370, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "j@EH_DM`xY]~xqUi`[`Zkj3_PuQ<0P^?6`O", 35, (void *)8166, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "a+,l!oDiK{OOl}n33`--`s+uOUhf%cf8w3Z^=h_^_j+'w@qb|fTf('e{CChyln+V7k5pkV", 70, (void *)18492, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "m?l(>pH,&'Z`ka6np.Yw!@6brODJH0DK8IL4?_6~,?H*26ajxe", 50, (void *)2340, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, ")W,uwTXH(y:pj-#cq5)BxHFF<%HLy/ggp!)i#f vUUtzEThT/QybuM.", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc; 
    rslt = aosCharPtree_insert(tree, "/J+ioAnoyeJ+p/GZPm6Q;(n[xt(-)Djn&|:9Z&A>)_kzdt@`*/4r):vJ3D-ngxL>{/  BvFxB'", 74, (void *)19902, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "7ff4{UMgE`6ZT~0O(%]*Ru]EGISL$qe85K8+iXarjjJ<q}(8s,G,$ShR0)x,R9tx>B^uE}}*$V'n|xJ>23/Y`%g]K0Mxq", 93, (void *)11136, 0);
    rslt = aosCharPtree_insert(tree, "g,m~`)j)LDl^Mq WEDs.@X+8vt~Q w", 30, (void *)24334, 0);
    rslt = aosCharPtree_insert(tree, "D6[i[=<(A0*N(lAjbd67FZ6cjP>`_c8v?=knmr,D+bUjS'6hV+!alic!/Sd{91.DU/.C,.V", 71, (void *)19470, 0);
    rslt = aosCharPtree_remove(tree, "hT075pt;u-l0,rz(Pwcx`.JZ*8 dbsiAz*O.p~-JJc+~d%@E*M!WC6xag$C=cT4y.scI[EVL0albf", &deleted);
    rslt = aosCharPtree_insert(tree, "(P'LS+cu9W9A$s` kG@d)05y+#rxGs6@L|y.{~k3SV%<9?MDBZ3eIj>=-.dtKYa`hLo-xYjK", 72, (void *)16665, 0);
    rslt = aosCharPtree_insert(tree, "U[bXp!)!wf0I*O", 14, (void *)3129, 0);
    rslt = aosCharPtree_insert(tree, "le>6", 4, (void *)31211, 0);
    rslt = aosCharPtree_insert(tree, "uqlh,L/]AnNImcalXd)nHlt4{?fn&f0h.kJvFx6#%GAlej2OXJ|", 51, (void *)8337, 0);
    rslt = aosCharPtree_insert(tree, "kF),?]s&Y#q0zV7`aplxD/hGC;hM(D))%8Ma2qYw(^~A]deRD172?B7hNQ71dYx-2)a+R;H*>{V%ZiBn^'OXb@:iIOF*", 92, (void *)7487, 0);
    rslt = aosCharPtree_remove(tree, "tYJ_#?1Y')E-$g|", &deleted);
    rslt = aosCharPtree_remove(tree, "fwt|8Py0QdYX>ESvnfoGaCt", &deleted);
    rslt = aosCharPtree_insert(tree, "&n]K,W_Jy", 9, (void *)22538, 0);
    rslt = aosCharPtree_remove(tree, "brmptF}=+scpHjAGQ(JG-xap]$@=a+ogz8ZoeM=", &deleted);
    rslt = aosCharPtree_insert(tree, "&{c/-]&[(vc?DFKv$JN^X}JsFuL(V", 29, (void *)29698, 0);
    rslt = aosCharPtree_insert(tree, "kx;SCjEbLhc12'p ##HlwXU5[G$7yY8R{)G3gz >2F8}g+ -GPR0xL|d7iwIXpMBY`dxtG5XCIa.RMJ$kvjA7!hCfS|TaW", 94, (void *)31866, 0);
    rslt = aosCharPtree_remove(tree, "~XUlhLuPZFW/eMTw&:G'`", &deleted);
    rslt = aosCharPtree_insert(tree, "U2hY5^u$m_euriVuLFNY:theM/amAh_Dw'Vwb*03$s'Vd  0H,_qBJ", 54, (void *)19861, 0);
    rslt = aosCharPtree_remove(tree, "b$3G0Np9VbNFHAB)IOZYsb)Os", &deleted);
    rslt = aosCharPtree_insert(tree, "]N]T+'4p+$Hn'gN&x9OF-a:4RSW4?s^-BR(jaB^-U<:!Bp{", 47, (void *)16001, 0);
    rslt = aosCharPtree_insert(tree, "E#) F<i:n)ZC'QW6l2cztX=F5}", 26, (void *)18682, 0);
    rslt = aosCharPtree_insert(tree, "G;_NiJP&@Q!*f]#?}x", 18, (void *)24357, 0);
    rslt = aosCharPtree_insert(tree, "%F(>)syxNko]@Wxm bh>Wc'S", 24, (void *)20096, 0);
    rslt = aosCharPtree_insert(tree, "'QAVz>;rA{WGXIsLBMdk@Qc[To%jv[x", 31, (void *)15874, 0);
    rslt = aosCharPtree_insert(tree, "_R?2%~x[m9<*!Qf;jz", 18, (void *)4053, 0);
    rslt = aosCharPtree_insert(tree, "<np6n5c8$()HBZ{G2)q*['SeM_D3KK_Y',J>@hyI%FH%G+wYbok}/+7S1LugARc+&R.psQ", 70, (void *)17054, 0);
    rslt = aosCharPtree_remove(tree, "q%E$jr[E=IN(XI-oXhzvgM`4.", &deleted);
    rslt = aosCharPtree_insert(tree, "sIlK<B", 6, (void *)17458, 0);
    rslt = aosCharPtree_insert(tree, "N7l[H!SxpvVlyHuXt", 17, (void *)30299, 0);
    rslt = aosCharPtree_insert(tree, "-/H6R2}7", 8, (void *)24796, 0);
    rslt = aosCharPtree_insert(tree, "|V '<5,8*|2Wjp>Dras-,U=%:#FIow7c fnKTL", 38, (void *)25970, 0);
    rslt = aosCharPtree_insert(tree, "!D&2+xk!m(tGJ])Tj'ph{y4", 23, (void *)22389, 0);
    rslt = aosCharPtree_insert(tree, "*@iK]dZSMn", 10, (void *)20864, 0);
    rslt = aosCharPtree_insert(tree, "FlHp:xL||&&-YuMc-2^t*4.3[@3_Yy1&)7fJ?XoZXO**O|o~4+E[YnzRp*.3=XDnL,sNe0~9y[", 74, (void *)32196, 0);
    rslt = aosCharPtree_insert(tree, "Ai392wz:,2MO*Y3=CG?ww{TrCjO;2J{kETml***Hmh%-%0_?rP!U}xBw6qr-AVj2pb0x=2tkSE!7+&4OPU", 82, (void *)21168, 0);
    rslt = aosCharPtree_insert(tree, "l8{*){vW<T2/5(eq4j/[Vry<|K{aKL.hZi#9rT6u|+X(|E{+w<nU-WsXqEm_'<@VraCKcaY%R-;G", 76, (void *)8254, 0);
    rslt = aosCharPtree_insert(tree, "7u'}.3", 6, (void *)12410, 0);
    rslt = aosCharPtree_insert(tree, "bQ%UugmE(3Lbe5@", 15, (void *)7863, 0);
    rslt = aosCharPtree_insert(tree, "E", 1, (void *)7701, 0);
    rslt = aosCharPtree_insert(tree, "Sj<ROEPG]#3r&e{j|9$~.$#>~sL J-Ty7ZpHdXZ/k/s`#Zu&", 48, (void *)28928, 0);
    rslt = aosCharPtree_insert(tree, "U<'hHup!r'dmCvkq~`rIH*XC)899[gKHi?<ij@`'!6h5/>-FvD$%UeXBwD@FTuDl", 64, (void *)11087, 0);
    rslt = aosCharPtree_remove(tree, "gmAdD9!Q70mEn!IuJe`;N*48[`2`:*%SrEMok(r", &deleted);
    rslt = aosCharPtree_insert(tree, "+X7@G|-QUnx2d6$JG S((vDjZf&", 27, (void *)30649, 0);
    rslt = aosCharPtree_insert(tree, "N1v,>TG<^A)pxH'k$V_++#.ga>^wzj-,/(>,^N-3*&DLPxqVJS^*z<<Kv^%$~4$V;I{x4T=t,*oQkN}0j%EMEu", 86, (void *)4633, 0);
    rslt = aosCharPtree_remove(tree, "H'EY^}qw;u,GM~/~'UG1]-T|;BgRGBbA>Cn;W';PF 'QO+J_j/ZsEa_U{q'wkF4i>o", &deleted);
    rslt = aosCharPtree_insert(tree, "vBDedBDp7}Q`8YCf7q:neu~re*cAv7K mHS_p+]]PD;SEfz[|dEg#cUf&;D:`gn)=Y=pIf[rMQZ~e!Aa4Dj3Qk=.>^VQ|", 93, (void *)813, 0);
    rslt = aosCharPtree_insert(tree, "~*-N~5D[kkZ}P#U_.w#4a8k?*{P7GM5pabIy?H$m &[B,d.l8+Q0#7keC{anh", 61, (void *)12271, 0);
    rslt = aosCharPtree_insert(tree, "q?ed7ijXDJ0P7~nwyN`H&lM4Y,,(", 28, (void *)16551, 0);
    rslt = aosCharPtree_insert(tree, "yS!x!&6B6&]2x7xs|-E?kL(mOMBDu)zKXWS]^8 I>,irh9v2,lN,vQjOk|Y 9FE'`z}N8>p5SV'<kj=:tR%jQ1ILs+(R:7", 94, (void *)22387, 0);
    rslt = aosCharPtree_insert(tree, "skv/ley3swgaRE.l72 w7@zgw!.>!%%-;QFlO", 37, (void *)18614, 0);
    rslt = aosCharPtree_remove(tree, "fk,E", &deleted);
    rslt = aosCharPtree_remove(tree, "oigulOujiI ,gO |c=l,}F'5*2]<JJD2_BW'(fO[N", &deleted);
    rslt = aosCharPtree_insert(tree, "&lFy7IOL{.zyCv$SGTX.KPHxb@3>H<]M%#c--)4/s'_oq;Aush*-%Tt5C?eIJ4", 62, (void *)32438, 0);
    rslt = aosCharPtree_remove(tree, "C(#0&R>0zpA7xHv3_8U{8TxP}lEq+U7NrT+^JHzaTWw.>tK1*$p!D]$%9LvTstYW!puuQ[d", &deleted);
    rslt = aosCharPtree_insert(tree, "UUx-WR92%", 9, (void *)18923, 0);
    rslt = aosCharPtree_insert(tree, "o1}'~fgBs{>SH'G/qP+v'L='C?8V~^j%qsyR:D^a*Pqy2a{< *2nY]d#/KjLDZHg", 64, (void *)22680, 0);
    rslt = aosCharPtree_insert(tree, "),onA)yy5AVsF_$woB6!Uss)np31ea(twc-.33M`=KAN~: |c-a=[h8?9C!EW5z!C=q;vG", 70, (void *)31586, 0);
    rslt = aosCharPtree_insert(tree, "9/:gVGy#gSA&@uusSia/.<fn_", 25, (void *)21522, 0);
    rslt = aosCharPtree_insert(tree, "2&SxO L`(P.0iC%<kDC!~HMI'TKXb>sE%]Ki;;W2=9]Uv1ah0jeAl_AJ98", 58, (void *)5562, 0);
    rslt = aosCharPtree_insert(tree, "+GNs;A7?h{(/GK)ty^RP}7wm.NR8d.Qdg=78x%X8V.yus)E:EIO*LXGU", 56, (void *)17403, 0);
    rslt = aosCharPtree_remove(tree, "},7jq$a%;8(>H.>,H*}Xz14[EpWQbPisaiT}1s<M5fH>[%T~tey@.`/X{atHm:,GNbc &Ba:-Vr,u'IR{Mj", &deleted);
    rslt = aosCharPtree_remove(tree, "l=;/;eK/{6cD*A[t-+*@Nl8/Si{p:?xQlFP:yU4mrn~k`r-xyCj73?w$-Z]]g!r", &deleted);
    rslt = aosCharPtree_remove(tree, "Qt*GxlkKwf,", &deleted);
    rslt = aosCharPtree_insert(tree, "m$.?'Ib}RS}nd_Ui5ZvT e8)FvG?]O3Z't9_;7GU|$j!rh[pcu25#_-jGNet|$pdN@oS#GD=e*A;4;(d(", 81, (void *)26940, 0);
    rslt = aosCharPtree_insert(tree, ")J)|Zh,zg)I78d~UCko3hI2({5*$KaqS]-$4FyQ8#)Giy_JJYw%)>QlL[HDpH", 61, (void *)8154, 0);
    rslt = aosCharPtree_insert(tree, "+LWSQDP", 7, (void *)28623, 0);
    rslt = aosCharPtree_remove(tree, "3`;hm < NnBC{c(RQ9fkl/gYN_NeX5ASH*:?6Qj~wZs=ZkE", &deleted);
    rslt = aosCharPtree_insert(tree, "bHj$%Q*(<Cp#C/|&A*.}TdQ2x0ncSTA=w?|2_B7AT .9xbRsU9$]qgr^c>q}Lw,bpsq1#]iiPhnw3:5mW", 81, (void *)27428, 0);
    rslt = aosCharPtree_remove(tree, "HpETCrb70x3[!?w6x@gP(fd;d&WQQ;*nkk16440H|C[QTt[;jjQ|(RT>c.a%~fgX42P!Eg[0^-isCV>3?]#", &deleted);
    rslt = aosCharPtree_insert(tree, "L6I/mDYNOO`4 B8%y9:yD^dugU,Ju4w(h4iuvd1QauAs0x[Ck zw{{&Wq mKbEl~vhmwk;pvtE-Xqun]EqgWLzN", 87, (void *)24139, 0);
    rslt = aosCharPtree_insert(tree, "TNpd?+&fBSZicn5gSUF;Ln}z`NHmyQ4N`Ro8$4f@dEi:?NveS.i%G<?lueH!@{]Cja 2,zO1g],1B,?+nCrGhFa4GlYg+", 93, (void *)1451, 0);
    rslt = aosCharPtree_insert(tree, "+D-G0y  yuZt|&Ag9Q+Naa}h<TNA:/HVVC0@qXsks{>Hy8g6wj%{7yZ'4#WF*P]0) eIC#ag)aC[U1hC'?ZWgS", 86, (void *)21666, 0);
    rslt = aosCharPtree_remove(tree, "fjTp)UVuNj'|WxVh;r>)@':i6]&&Lvv1!1dK/H4e#)0w:LokS@e%/", &deleted);
    rslt = aosCharPtree_insert(tree, "7g]QM7h /x,`_g", 14, (void *)26747, 0);
    rslt = aosCharPtree_remove(tree, "g1mJMb?C'(TW?Q@usWiLQ&eY1l2=V_`zadj]V*<C+U4Fk", &deleted);
    rslt = aosCharPtree_remove(tree, "Q~DtoV--gLDRSZq?RUsep|}FX", &deleted);
    rslt = aosCharPtree_insert(tree, "-1NuBQ6b4_in jVv/Fv&:vr+d.BX_9l%t32L7 p@Zc_0:SQ*QY>^=eD.ha7", 59, (void *)21396, 0);
    rslt = aosCharPtree_insert(tree, "m DaEyImg3/>%n8XL^X5[nd]T1d7M9J^aZV~3#Cs&", 41, (void *)23176, 0);
    rslt = aosCharPtree_insert(tree, "l", 1, (void *)17600, 0);
    rslt = aosCharPtree_insert(tree, "!RZ9r.l[~MPX;kLc9n~5|NW[s8;qNCu{8m+]>:<}S", 41, (void *)19917, 0);
    rslt = aosCharPtree_insert(tree, "K ZIbK$I<P+ZUb 'x~eV?~~SR-=^Z.S;_8cu8X=f,y'mwCavs/CJi;&pXkNy''l=(pJ)&AI|gy/dN5$YLf,", 83, (void *)5244, 0);
    rslt = aosCharPtree_insert(tree, "G*FA~A~#aE@2'Dz~y?H~?BM8*ta.C)]yfvffFZtQJ3{)d^$C2p6z:euMuZ8|4_tF9usUQEjn1xEsYo7lKZ)6KwUj#!fA", 92, (void *)16268, 0);
    rslt = aosCharPtree_insert(tree, "7rifnqi|?$FZS{Dq{j/WN?gUL3M5Qleb{FWDj4{'J},-L[0>%*cw&oSbhXT?6A/#nzVm$Mhh%Fy|YhQ+;", 81, (void *)7851, 0);
    rslt = aosCharPtree_insert(tree, "  *~x<'NSBvkP~LH*k>Wa<6BO,?*ZVO7TVBY|RN-?6{+6 SxmCS&Z1`7&/M7~", 61, (void *)12774, 0);
    rslt = aosCharPtree_remove(tree, "licV~uu{;8(gdFh&lEjM#4iGSidp?l4962F::vwE-pYL]R.?XEZ", &deleted);
    rslt = aosCharPtree_remove(tree, "$25&L!AS=z~L1NHrdhBc#N-8PJOI^", &deleted);
    rslt = aosCharPtree_insert(tree, "|*wMoFf|:D.'J", 13, (void *)17358, 0);
    rslt = aosCharPtree_insert(tree, "+>|x:`muKTS]#ZsYP 5c|=5{(Hp/WY.|y+kVmEDh*C)~1cT^n}HgX7zrhH,~VSwjr_M*WZNZAl9GK_f]h?B`eT!!e>Bj;", 93, (void *)30412, 0);
    rslt = aosCharPtree_insert(tree, ".+}<l}hF$GS}<r$7X' K?UVdk{3(0Wr?,QG!Tr*P:XvqI`Ia8Lswyth^Pwp,5DuLIS#:nyl-{abBVt%3BY}&ea8ue7`Ce", 93, (void *)28799, 0);
    rslt = aosCharPtree_insert(tree, "mrSv}d)#esB1x!*L+|zO.>;#~>5'# J-/(}9CB&Z.r-O()_</l6*;+V2>GOmG} (dWh]r1l", 71, (void *)20056, 0);
    rslt = aosCharPtree_remove(tree, "l}i$BaYZF0dvixckXUTA(hedYh1Br4rMQdupO`sx-5aPm'tF@DSo}%3", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "*T{/|C#&%Wgyl'j=xe6uBx!hLUj@sC gV)?isHXLhUALVBLt.l{s'rrY(q#i!>8;#o_i-=$Fl{+P6DMHzK2os+3|", 88, (void *)31959, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "@|pZn>^o(M,`0|),,9K&B'UOUH|1+tO%zL1*.HGKz^bcS~m[WB.ib]-C`$}f`G2_yeJzFDQ!l'CyK00zVw%2}.;iH(](", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "[a-VRXL8Edo#+K", 14, (void *)411, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "D)uifi}(vUIG][}AqPQtInGwgl`kdN|Gpfy-[cuI!fgY|u0$CVWk*TFrP%wr oaoMW=xK~:x'X6fj`1} fh!yA8U_", 89, (void *)23361, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "5'YH!]Oc'F-&x", 13, (void *)19721, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "`(8!8JwvqYGxHDfz%:", 18, (void *)22568, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "@N-=VZkulf8>}sA.NnrNhlums`?#IDtT&t9Pt)fD<jM) `eu:;MYX}7x:g}", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "wdR/mW7@tDRbG{f;BL6 >j1", 23, (void *)17576, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "$9lJpq0`.bs2)4LjC[A$fw1cw+mm) )`'>}UbXZ fYOPcO)ph'Fw5h429xz&!,eX}|1,<rQijIx0n]", 78, (void *)11098, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "pA7OKO5/5knp!r$rN!]F#y!HR", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "7qJGwZQo5Ok!7jNa|,WT8g`%ARLvK_7/Ca3099MD95PC", 44, (void *)11026, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "@!sQxIp9[-4L.J(eNL}I}z4{^d<h9CXAGzS,8h^!}>NzsD#NzymeLyRQoAlJ-UM90A |98yK^w:y<>>fhwkJ-j0", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
#endif
	return true;
}


bool OmnCharPTreeTester::specialTest14_2(aosCharPtree *tree)
{
#if 0
	int rslt;
	int deleted;

    rslt = aosCharPtree_remove(tree, "t1,", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "~9m(8~(E6Z}dFeLcxlUo<p%~avm=:y6D5b7|[FS@ho+7{A_Sy}6*Xx?[j:`0N3Ci%{Q&~^sI4gOcN~Zb=jF/E.`~`|", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "xrPjI<,TM4M'SszW2c#;#L|O;ab4/1(cda^rlQF%S1#.z@5/@in1L+{kIt~+_'3@#D-'N%$'R2G(iQNSa^XUKn]9U6{", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "p(k<CrYB8J@U%H%#>Y;/hcNQHH@o~Nv3AV=8K=e]D!4],cVvA+wp=8mX+/!dTv*_N^S`]h*Ib9eKf~I@GOtnG", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "UGP6>#&jbi}#?;0bS&Ke%M1Cs=2ED^x7m~k]k )3?V aqwIRkko", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "/#&#VXArWh@W/o?-U>%SEFPQ<Ra!kiTYf1srQ=)H,]C':28r=32jvL0l/4hIHTy7q1a]6:FR?~.:D$>Q;u5wA5]>Z(ai", 92, (void *)21847, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "([^!z#z)<AC(FS`vWSv`Zjh1tf*BeDM5<RaW$V}h$LX)wvZGc6F%A',d#", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "XaB4UX4>JW{UKanAU1Q'2wi>d3RF+aWnoS-w&f4_6Z+uyI+JAx", 50, (void *)14700, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Zu#wF 8yRw*E<=`u;Pvn.&B;&J}6U`JuOXk4eXP>.a", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "[xdfY^$MS:PvfacJ*wb9P/7i5atZD;UFa[!O{Q%Y0m^W_{k0g9*S?", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "vo%DbKCqI/ygX8VO4Sxyd&xFYgxO.&]&U~X*%%", 38, (void *)31277, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "#HQJF+,R~XSBs6]Y;R>1hgJ^7[WGkZwqhv04{'aX+4Jz48 >Dxr/i|AvZTMWgw", 62, (void *)23855, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "RV80", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "R] #qz6y>*wo;&u+ZI-kMLTj}Uw)K7w)td5Jr' xYP0^!PgM3t(pPJ?DsJ", 58, (void *)15053, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ";vk.R?!''Xg{AR|=)]%k_u>jX>s_INu;p28RkKw4AKA_zI*3C8*HI/al}-4VR}5(F{j;]", 69, (void *)23070, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "<i[i}:Vo@Rj?#getKkmIauJd8WDC3QV|jN#:pG*v?Z]R;F9", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "ciKuq/qb]_xIc,*+vk`Z^w>+S]d~690* !Sc)LI5>a}Sn~$@Vok&Uje`A(Z^y", 61, (void *)27061, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
// Entries: 250
    rslt = aosCharPtree_insert(tree, ">LQO|WO_hZZgn$H.kTSh9", 21, (void *)31899, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "?S>q4 J&TNP*WQXmz}2g9!{,!*&U]R:V4nY@(Ftgw-u+r]4d|Ui]N|DvK{2v;Zy3uW->})#EDMv }Pc-Ib", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "T+(x`j@U0s8St[rA@+mpk5HPd=TOU@*4H+,~I00<dEM-^W$c%X'(=tu]}9`6EQwo_#TG%q<V1KLho&7!] 1?f*z%", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "~-Q,8CjFAktM75NA(cB4Xlewm,G8BGmj5=x[x('wkW@1z?Fot,", 50, (void *)13472, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "HNA1RIsj((1U){5 FPC;|h][WkPB]d[i-LmfWjjKv+n?hEX-T)oj{(;^9s0mNO{n|Nt7OA{y.:N}b<u9_qom", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "qQ&hb", 5, (void *)14705, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "|u`me0Icih,n_A4z i$yso/dVVA,2iG^gyg-hH])u;F|$(<izZQN>(*((cAGK+jT$msvKK$b-Yp;qHb.ODP&P2iD/=1 ", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ",DT0G0B-[lWE_YHCXUu0zYM7*@dqN,w$k%qj//*Kv%m)3j4M&p)!f$J+N[@b1#;itV'^Iu|Hh,WP* =?fS}.%Sz", 87, (void *)19313, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Vom2P)W>h)`z3Sijz{~_{QkmcVZJ[PiK.YG.`sGYxN=$v]qNN+smqFTO.", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "2/&yU%&qhQ*$Z{/%&}c1W:BS092x", 28, (void *)11227, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "V]eU-tnvAOmu/bub,qtr~pZvw'Sy+$g]H%55hT:DZ?qg SZr+At6[AyO^xj|czctk$@3$,zy<zCM}", 77, (void *)5119, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "q_YY4G<i1s=)EGZO;a8%^KQmKY'>it!Rz(Dsb(Y0th", 42, (void *)4148, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "+*.", 3, (void *)19419, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "}{r5;rd;o.?|^Ba(gGZQ#hC]*%j>Zw", 30, (void *)18089, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "*TaONGR@RtH_}9Y:9B>p<#x|0?1VyY_[S`xu", 36, (void *)17807, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "5v,WQ*-.w:7P6-*t5", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "pV$*JeU-kh#IxBjr!;C!,lp9a4tL)?/Sv^ wjU=!%0V;O", 45, (void *)21445, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "HelmE08Ik/S[&g@6)y.$8LaG~j7q[k6fcoCi>", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "xoCXC 6+}x +l[KV}0x{[htXHl[0VwZ:WBA@#+%eJo$NwA^xjF1|;@0q?:JmZ/RcWB", 66, (void *)3499, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "ct~", 3, (void *)23715, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "'.-]yK.*PQ#(0`+Mj{u3eUGpRDU&$z^5fs;*|F!J6Am/bGhw?o6]?e^29YLCtc{6g+j", 67, (void *)20811, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "d6TF0TN4e#9vT L?Qhq@uKR&3b4LL<d[b~|", 35, (void *)23049, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "U~ fpa4XHdFH4,'ui1A]GcX,xCUikah-;77zno]iW6Mk(i@XGwIr4){U|VtC3]Aib@6e'r.mEWuO:wJ-`U&X*ddHki'F]A", 94, (void *)5968, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "i+wBdE(_s(Q.vSUFsuJItto5>Prods8C2]gxOY.f-1r6$(s2P[p` ]|H+NqufL[bx,", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "MHRXO?7H0>-A_,:5cd*BnFs1", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "<~9QjkH/G4*25 d,yBW<y-C#;?*b}m-G4RzH`.jtzu5b26xh:X_+<R%7O0rFwGIuI&oV)hFLgI1Ub|o.>$h", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "9%Tfi{A-&n:'", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "S7,Hf9YN=g8yRa7W{j^%vm1im:^DI)wfN3p2jbwGH#5AZjNLRBrY-{N]2k<e'Qu Si$,09@Xm]z~Y", 77, (void *)7937, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "}Yvc-#t>kjPr}sKR5Hl%JzJ2o6c,L9emc^tLdgH_z", 41, (void *)15551, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "{%C6C}%KD=$MCB896Ost3|p-eHS1pbG-v/s1Pe", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "-xEIBMC)U0eX@09>DTpLM@zz#`4}=BZPp,o4Uo#<3VH_DcgS3 xT-", 53, (void *)32044, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "az6/FeWsP|fThz6m g].pL-Udj9xk9?6Ws(/k|g'ok.2-mF:cHpb/heB%Q8#0|64,`9PRrE <+", 74, (void *)12099, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "^hudA7'Cr{B,qOW!g8dd'/!`-edfO kH?W~kcm+", 39, (void *)12985, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "_4B>ZU+P/nL~mI|,}nB[Ile{hlOOLE/:MyfJq4xzP%TfBv7s 0~jvL*{Y{3", 59, (void *)17115, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "-H-]j:PO(qq`K{82Xh+_[pE;2Y*;?)#J<j{FRZlxA", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Bs; .#tl#", 9, (void *)17717, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ";8U6gHlX8d!A4g~~nrl2Y't@OviuY]jOk tT<iu+|Y:1tZ)QDpOI}m>~m3C+d:P^n`H", 67, (void *)24397, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "3*6`nop3;<2N6(j;3s-3=1", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "6j:/K", 5, (void *)31301, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "oAxl`[|Lt5C}v19)+$<eK.a?,E[sf2J)Ur", 34, (void *)6523, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Xvd:Q#~[f(Ixv+.>#KVXerRPOSx.?SsUT#a+hF!~HAd/4", 45, (void *)17142, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "vaK+m6:|lc#e|Cnu=.F-x><T7Rwf5#-", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, " k(GLg{qJOq%B2]>P{+}_#c,%$i.uh", 30, (void *)17987, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "GlmO)T}hC!ZgSUz >F<d#wyDmA3Vy3 Z]KX+<", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "620XEJR0<sXcc3=oN,_UoFT~r[h4&T)Zs4L/+E*m_,N{Xcm/'epB(*p]bow(`&;t&3>vs18+q*PUC9k@;ac6Z |}C.lM:", 93, (void *)31728, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "}#KBD5C8X.tyo$16$Z>7-V*>Ll4%I3}mG%ccV&9P]0c&'R/yw7", 50, (void *)170, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "VJam$;|FvtD}Jj;ZZ)rsJ}sG}d5u_J_YSF#", 35, (void *)24192, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "NP`#E_A?e c:ZP[>Q<Bl'P>6Xu%a<z", 30, (void *)31460, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "ESNa<|3[W5Artm+8|Sl.I/m};m6LJ<w:5c[S>c.j~F;*xdzMX", 49, (void *)23912, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "FENs", 4, (void *)8242, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
// Entries: 300
    rslt = aosCharPtree_remove(tree, "{I:aXU6%szL+9k.%B[Cf;&56nHK0 lF9DjW4j7&y@=", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "^Q+=|Ru%fRin^3>zP%>Ro&ChW6*DNvdb#xs)~$7bsbTTa7q3dJ%c^~#Dhx) e,.(,IHG/<GhTg6''=vciV", 82, (void *)5341, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, ">`l]<p--Q^jtyHJWO?2A9')F`:_fEYH5", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "tHf_*e6L;+U'YT>o=-`W|%5l0mfLi}}(JehdfNaEgvXMlE1/wpr;a;S2k%vAE9u:Q.[ tO^b#8BqTREOxb", 82, (void *)30239, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "YvsKCRTSfAc{c@*emMalChy2(3iRAnS@ze&[srK-1nkCO/Q:A2#8.uD0T^*#Y;qUu6y*^~,hJv)d", 76, (void *)7230, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, ")9C(.:%0|HsCk/bq(L0", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "|3oHir$X;@ K^rfC6]?|VmE[l4S@N;P%U=||FdpoRL<*uNHT:NRD#.dY1#;QmP=jb`7DOVoGJCbnRIEi?vIh@d-VseF#", 92, (void *)19261, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "q4a`N!jP=HC%I{3 W]LloPWqn*#1]Ke6=w_T=[uvI1.u,4du,<2S*(gF!%~n!", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Bvt~Fft_&}DN/A:37Nt(@X)?|<#%gmI~eW_F^QTNy$n.AAC$Fx/q.SjN!da*fOGZbl1c?w:'|LBFm2E-}j", 82, (void *)18761, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "6LuPSXru_OehvI^YUr,t<:df,U 8[hm9}f(p+,^PgJ e5*;l9Hy(FY<NHur#J@{Birf0]*P~y6cM", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "O>4rU,6:(ppUtyGI[hiB5T[cLCvS3", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "gGit|S`^!w%(E,O`%%O;^SD6B4! VbwJjWnj-O=;|&M707,C!&t^", 52, (void *)11558, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "fFd`F:U~ykEFR|M#W>;:<9A?LSi*I2Brt}jp:6R5spiMV*z7;<::", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Cg5R", 4, (void *)5083, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "8KULX_/A9WmTeF@NY4j<py29Oha2<Ny+Ae=%MT.62KepjBU!7Ip*?wa", 55, (void *)31957, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "M3Cl'", 5, (void *)31084, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "]dX'z'o2*j8n4U(szQyG)wh9)a5v$hlD15", 34, (void *)11445, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Y(QMQCyZMheJ1J*s~W0L/K^h7~c0:>gO: 6O/+6qVnr+wOv~J#;E Q~DQfHArJ", 62, (void *)14599, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "o)#pTIcH`933XHPIxCjhdx", 22, (void *)32471, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "boL*D.fTR~AE=s~btWAbQ4]-?>=3CQk7M{C^]c5}t`{~8/om7sq<wpvmvB8OnNoPTc]O''1iDP_`d", 77, (void *)13542, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "i1ux)W<ZD`o^I[`", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "VQGrVanTC+q-h |KbiaWCq9T*g. ?dBUbC gh{4eZ4`D8+%=L9Q&Y+9n", 56, (void *)11972, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "uJCE+(,Pe$[_rM^Fq[>`t8;(+x>OknD,L+d#f*+)vKGp7DJwRlwmiY#P#`3Ehu_|SUFdn^Y>>Ei6cv^@&+.", 83, (void *)16295, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Hk/9(wsNB8E}j=AJjW=1>rtD8jk(?AXNN/M:!wOnVy=`qzgojkGU=Wb?P- evdP&GeQaKZKYaI+X3f+V'5[%=dv4WU ", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, ",d$W<g.[?=pG= ,!`?}e]3FFJ%^^$a<#y8dT%[XA7", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "3e|uLhpnCKJsS*^dAXhmj~g5lNU%utV!Q", 33, (void *)4346, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "%g<BwEctN8Ns$B?Bx!nfVAq9^N'4I,Jrt?>NO!;(A{Ma_NqlNXkl1:d `_/E0kwnP", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "h#2^>9_UquxER-|9q=?$xAd^S[BXQg.Zfgx1hwS8Wv@A@7-PJwH(s,;;e9yG_&nV", 64, (void *)17330, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "z,C[89CN9 D_VHZI,!jt<q16ZH", 26, (void *)22148, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "b4(,7`Y' {1MZkw2=sKMi8M{*>Jc?wLtygB{RLn={", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ")',K22z_v%W7S)5aTttd;8}Z` b'4*1]DGw?o(=s<v{z{Wh_'*_Ir0aO`>U-Zj5", 63, (void *)10782, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "rxzDLMpz7{!^2*C_al%&--oo~oOowcOpBm}*0.]zOnks#f)*Nj8]q&F%j5k?rd0<_.a5(k", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "=H2wo7C`#rhF^Sk1{tWU'3LYw>n+9jx#fV~@2c.nlX]l3j'Ph<g0GBz]!_.d^@,}@BG| obcu@ktP`uvqxm9QKuo", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "w)_S{lU-Oyc/+;*Las4@B}wOShzt1>v$FbKvLk930,~l|;ZiFKOy<s", 54, (void *)25264, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Fz4Ow*$ nA8Nf'g4cJJ?V9el;kfD)<Ajw#14tClq)o)0#:vCaSW:5|{EQ}!tY:1EnH_w", 68, (void *)27591, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "_t", 2, (void *)30143, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, " jG12(M#=i|&Su7Eelk<9'AqT9ct8jZVaFG|rw=]U}Y8f4%C5D-kp<{tIL!)v4_ODQCv#HG,%ow", 75, (void *)12214, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "i3KrnfJvolT:s#);qleT_v:RjGJBEn7G06nNhMypf&p,No^Y/)0tL", 53, (void *)2543, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Ce6OIYA~z_9tn@4Y[T2_3>T0'U", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "W8iNxI<7>$$FIOPYV@]/^ne!Wp)TV?7yEi;'U1a", 39, (void *)25610, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "f:@aJmxi~EqKZ68*^-{k&XSpqW!'", 28, (void *)14437, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "%e13l2K[_l=fRrQVU%gMyN<,Q 2[4kh-.AY5#`r}Wo+5kyN7OTu}?phGDz].||TB_?>IwKw^0FdTi[Ur)}jy%4zZX", 89, (void *)22586, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "i%m=nF$iE@P2}?wv[]@H^+bAg4t}MGK=E:imh{^4USW1E{>`[,tM^wl$VCJ=h", 61, (void *)8441, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "D7-^lda)&W<8h:*@I7d)/?G%|!RN.n+yK%]%9vZ?jACZWC'mE", 49, (void *)17198, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "IM o,PTAVvOtmJiR61:PgiHH]eEWm+K>x8lXX.(", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "}>PRB(#f6K%{c#<SAu[*9LP/WC;YN9X?Qmu9#7.UEp;X3CgRqHYpgOA4kM+wbrR&YR:zd(WrtgncbN@t7$@W", 84, (void *)26361, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "U/f1j|", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "_8m%:fSu}_m9p2b7g6'b^RJ2kD'*mP7Fs4f(xxM/dSt:5ofin Y,IvLxbby@1Vf9/0K54VfPG", 73, (void *)32698, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "m[aeBLSI(mTWap+N1]BB7@|'Pe", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "N,bZ-II3=1 hRr1'8~3=", 20, (void *)2770, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
// Entries: 350
    rslt = aosCharPtree_insert(tree, "cU,';/P'JxTrYjV Op#:/3Z;o]~]5bK5MU8", 35, (void *)5296, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "N=Rk~~$!uP/", 11, (void *)4301, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, ";S9|n3~?2}+R~e1X8D[xvQ%", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "&NALM(", 6, (void *)716, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "fhREUo'E.`-L]i6i'2O v=IkODV`JL|f~>ZaSMG|1Q)`s7eUW*=.4/`&}P*9nI'RIc$(PVm<K3'b;$`0,`.RNy;U", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "fOoPDp6ZI?3m=1^I{n[t}Y|IOp", 26, (void *)5141, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Mj0SU3J?QE(/Oyn8NLa%zq3d*]m{rZh9.g:Cdvl/]mt4>x2D_<i@I4Rq<Q", 58, (void *)21984, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "mP;J]PJ8_[ &D+ZVsqk u`&q7}'(t/A*iJ&GwqZLsmzgm,bij", 49, (void *)21878, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "s&wK~c&Z=4^u+u[4Xuo@,p[>Pn^D@j?6ciXQYZ`B}9U6Dr6}cU2DP8V:fmE{U3_/vG4[TNLYck`N", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "i1(S&95}w7H}Vduc_sjnP8car$S:j+j", 31, (void *)3173, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "g*BM}D}t&<@_$BsO'B`+Ef>{SEOTc<T(BxlMJ%Ti}nYNN5", 46, (void *)13501, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "kfn]8({6WT.F56", 14, (void *)18283, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "{#|yALhX5(dj#]A+n#-LL67R@E0`?,Ttq]oXQyqP9+E7w/'S*/>UThj#Dm=Gqk^F[5*IHiH.1USKG9c((3pkQE`T<", 89, (void *)801, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "lW%lHv=a>He&B:!W`.k]-7LI8IHRt;raWtc9DYh65]*Kt]<bY@i", 51, (void *)18641, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "g>K@[<d={o^BR<n/H5;5%lhDwuk+1'=Z}Sk1", 36, (void *)16843, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "~U[xxknuRMHa(`5[gR*~kYpCEJBM1`Un", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "SS}ZMSKBcpO@7hyv,uEW*Z&2)&b]MT>#4&@6Pi^AkFdO>~HAsdR#D**AWHw/z", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "tT-bQ]YWxjy3hH#M:GLw5UGd0:D&vG'~lks1=>peh#il|5P", 47, (void *)8355, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "46Qd%-njVik=y/]E>BR8qVbk?^E/}eHUtIG", 35, (void *)28351, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "sp4kP6`PNRXN", 12, (void *)15495, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "cUJ|mN7b{!'}z^?0]7q)&9kAz9je2`S6hvo)t", 37, (void *)26929, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "1{kADyK}{[b<xU<h^Yu VU>|ZJVj@K:{#[N~vvcu]*;qO[>}2>y&&*HG jybOBaY>v?", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "ib!`zDK`ZNoW:naU?Kf~NLB$$OzNJxQql]{L:oPu8A,N]t'aIPhQXFTo/gJT>Uv+annT`Ve]Sl;i?cw6SL-=maZ),/oj", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "]", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "E6}V5QSO&2r}", 12, (void *)16137, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "zRlaM;5Xs(EV<c?lg-3Eoy3ll? N'_ [QVZS5v2M7Vl<E&Vc2g!]|4H45~e-ML", 62, (void *)5349, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "|G{k;rX", 7, (void *)(10152-99-1), 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "U=X`C%TH5QZmd_M[Xxy({_yBH5)gfz:uK#6>)C}H?E7+n%b@-$-PA[A", 55, (void *)1295, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "0#W6o^^7_V[_}6&tD)H^I2#Pa]", 26, (void *)28827, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "m.8JT[19lP8&'|2&6j>Y;D*OADGxmf~hN<O3UJER4?WyYll|s=tosq^cqe@zqV", 62, (void *)7661, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "&1xVA9ZTq'|B+w yrsnCT;h&EJEPl%zozSO<N ]aS;_ zq`^DQ$5d1&n<sy", 59, (void *)24029, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "r'lJ?#+cn-k5WZS|&)I[wXf2i", 25, (void *)4365, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "q9ei:mh*HFumP]E:2F+eIu,S7F[u65LABF2=F)L@GHby4^Ij3(j9L=W6cf", 58, (void *)15155, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "-%TiNK{*KY+BQi?Iz-jF;KkBp6{}=5J$#~8DzfrK$|oD Lf*nO=%,,i5e1U?", 60, (void *)30035, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "g]y<5&.pcSb|?repRVv,I_I9Y~C@v=^_}CcGjJUnY9+?V?o(Vm7RmimZ@bhij#53<=y_R +yp&l <$ku", 80, (void *)30015, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "%b'F}?D2VxoWPkK{*>5.dCw:.hmr<t1LQ_r ~}Nq$a", 42, (void *)26889, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "pVjzw0SC,sZw[fY<C=CAH5+|oz_)ffJJjod<F ,FH6Fji3giOtao~]%z{,G5", 60, (void *)24280, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "OJqQ8Y", 6, (void *)15664, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "wd?B,M0=UcG ;Nzx", 16, (void *)23373, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "-K5SVg@@C6,~%o5' ?t,/`D:KXIHVpyt>6PqCKQK@^rTb1syts.U7ul4wV#Sl^/6ZLB/|f~I_A.(FwqL`'=^p", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "@I6=K9w>TxGceS|go!HjK)(XH^xN2>WJo?bd%^bO<d=[DAM*UP@:@YTL,7^Blp),`6I3r7!]qu.Zu%h?+E{b[>", 86, (void *)27926, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Ph>Wd( ly##[o13 6{(tdI*.XrB<yG L0*", 34, (void *)8014, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Ga45ofXm-'Yux,5tZMqIkY?4ZDJ_YKt:(=(Z7qb!>U7E/", 45, (void *)8979, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "|,(c*&)`@y}yg6Yd~3z+EAOHa-zg >;$/9tgHO^", 39, (void *)510, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "l~9,)M=Q)?3,Jso}tI'1IKW {", 25, (void *)4241, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "nJS b}y'z+zT@Ib=UQkl!*&x*GhidbFe~zZ%!~i$x$2&I6V%wA;{=?DYV>c&1_iqWIx@#CfOhT~", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "|", 1, (void *)13885, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "F_JaGddH=Gl;q9?rAr9o=KbwR[Q |maeScrv5{QWxf_$HSNBDl=r@0$6840@ J=T#Se", 67, (void *)7544, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, ":wy/34F-,}V~>GCKT 'O'!WKr#6!]Ethf5't,P_Vo(*<;,  V", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, " ]!GGbt;hf3P6n5", 15, (void *)16974, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
#endif
	return true;
}


bool OmnCharPTreeTester::specialTest14_3(aosCharPtree *tree)
{
#if 0
	int rslt;
	int deleted;

    rslt = aosCharPtree_insert(tree, "?N_@@QCfGN#ZIV2BAKa?*k[ edN@78I?$qwH~EM$/?07Jx;^/M<CwS:'92g%R8V>eI){&!{%9&87", 76, (void *)32603, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "wd!T5K#|V*6(rzyEa~^(o[8rB{WRV$=Z#O8x~L6J4XS2zT[PCybHYbVPmM{:vLZ)-54m49z", 71, (void *)7631, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "=cVg)oND_ ngu)eB{NQBl)?s(y#hc+bj5m3KRz{,&t,f~/qNK/f0XSJ}{{v", 59, (void *)29630, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "kneno[7Fg>", 10, (void *)19189, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "hL;g=Ygx|*&L", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "4TkaHA G|l", 10, (void *)23956, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "BLycA8:lW+KNoe_ZDqW~C(OqagenT:y~XI2;V5h{Ahr+J]-%(zs%l$)ur*D:`>;JUaZv", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "y?mOHJ}~#AUpN5`m?JL", 19, (void *)7861, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "bH_6ABZ6BpOGC3*T_6_jNi'!T{7<2XrX$$){o)?LQP_%D", 45, (void *)25454, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "sdrEqs,[ui%>Xnr{!6pz$!x5Mj=z~eLel0)77<5q:, ", 43, (void *)17219, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "8olUmZFN,z3Q[kEi?_I(QMbAD;xA(,VKE 7^&3?94` 9N.pk}tEKbjJ!^al:MW", 62, (void *)10555, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "d.*Bn,F_=JmNPnH/x}@'etgfdgYqJ]8^*&?6V`v%Kiom[odS,b[IV9qGvB_tJodW@P@dkA", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "O9qI?y/?8ElU}[RJ(rya^uF 6.TCec5It%=d5nS v!U", 43, (void *)30276, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "&6y? 2A{%pPAgt9yv;5I}[6?S!lRRYET'Ywv$I4F.gi5 y*_#0I=/xNxT''_?ZhWvhl", 67, (void *)28820, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "o!j;4'/%WI~_Cdx4Q*", 18, (void *)5838, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "rFOsM2cG1<17#rtCS+2`k*@T!/B5lI]7S", 33, (void *)7685, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "*>Q~9{'", 7, (void *)29786, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "P@Ad$wCw#`ov&/~1EmH!iZXN:LNHVl/JnV&vmF6g.]C`4z", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "0bGj?svZUy", 10, (void *)14213, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Y%p@vNbEyx#^np#1U2$s94L?1A^wx*&3B7+~<U[pJ|*jvw:aJH@Muj<:wOta=wwle?yMN", 69, (void *)19274, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "76]FU.:G,hN^'>y]o.-|J] P9E0@Q5QTB5,TL+'IG<>~N'We02", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "DEKpFZy~g@d2xg+y'kwG.6g;&)F-i/q|S4hmzH$$T1p$zD#~<jqf-FBtNR`U4,DWs(|dH:$f(7v", 75, (void *)18574, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "&L&pY!qEHmpf[;IGtb5/zs>q(P[qlyRe5yL)8A58wl{~E/77iYk;Z@k-J&9Oyf0&06gaN%?HZ#9,oA))B;bqV.Z`CQ2Q2", 93, (void *)14526, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "$8ZP8G[s/he`aOp9!6'Ps", 21, (void *)26746, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "qexSi?4DQi] 9%Qw$]:qRG", 22, (void *)25479, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "|'p3hklJ-M`pf&LY]yf.vV+*<<Bp-12A31E{z0bdeROGuO", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "JK(](}9lf+#yf ~x6ur7}tqdG}Wc~bLkk,:]hBx<wr.LD`Zdc", 49, (void *)20619, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "FQmtL*{YOU>1qoWogS3v%5|X>f_V8mlua~D[OT&[ @YPk;r(+rn!L@X50k}<$2z!U", 65, (void *)9956, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "=*'k}PXhlSKR+T%slHRnfz)!>s-H+zdoj 7$TRXHpdsxcgK4u", 49, (void *)10349, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "9%W;k(3Gvh>]$rqo BS}7A}@S_{uqne;zdV}+l.;:q_<Eiq8Rt]B-CU73", 57, (void *)14441, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "x(;nT0[8%$gGAqi^,/+K;@eEm@rlC8g{{>'<#R!J)+;j2r@!oeUuLE8tz@N73{)'", 64, (void *)30918, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "STwzJf~+CHm*l$$LuC|ZjD^yRRCH=$5osDY}cT;D'+wf<D1N", 48, (void *)11643, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Y({ougGK_5[f7w*TCLI/vo}6Q", 25, (void *)14825, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "rT}3!Pd[OD~ *@,Xe5{:", 20, (void *)25622, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "}S/W+wgan_tug[|ha@Q$ 4+IV Gq^Ec^iJi=E.BnEf:T|JJAF5Lp", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "_l)2] ]-(:+T)Kc(FgfM9WFf#6OFIRzjeci5'c1B_j<pv|QalE!%^?}tRw%#fJx,Jpe|WUY", 71, (void *)15343, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "9+;@QvE;oXI a*}!QAvC@ 5Egs<gk*W076WW3%Guwsj(SBa+`L^6F94;=T2b~W)!NFqMh7ys)6Bm[kG@Q(", 82, (void *)28787, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "YGk{bcs7SjB 3cOo|$LK|D |95^w#)G*$pV,^5YOkj|@,425-|.A<.m8&5[?$7Fua`cn1_s ]4^!d:hmI-%", 83, (void *)26770, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ",6/J/<j)I{4WQirclm7,~N[Q..`dHFc[c<NnxM1wi.m", 43, (void *)3023, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "MZHIP*:uLL.CL4u:F-$]`uBP", 24, (void *)25207, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "fs 71>LZ|U7~@K", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "4.[?(# z7XCC/]Sq<0bZ/bC a,z8P3AH&(G1SX'ttBy,Aw Z8rZLCWrSNp@E{xXj`VmW", 68, (void *)18322, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "JF^e6sFyW^s+~QG @Pvym)l=.f.dQ4I+4t1rFGZB*^_jl34G95w[!j] ^bH&/t 'iV(!'eJ/jX!G'(V!y4", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "e_5Nwg`Pv;%>L}F'PDH[_2S[7LPw~_Zxuc#=C`oC5W]i{.*z4+/6", 52, (void *)12791, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "r(?)Du1wETQ'TATQU0I4ZH=9K'E", 27, (void *)684, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "`9T<X9^6s&a#UCk1*KYoMa", 22, (void *)19755, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "(gF'Ts51:l1KK-(f3DT*D<^]5'4FMu2p'.Mw8Q`*Ez_;A`2M", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "rf{129RB|fYTj!.H7t!x^AfKyg4R9p;~,s-9}yp~]MO&63;rSxb=TvYQ", 56, (void *)25415, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "'oymIo~>n+{MGw#9K^w/Ol>ac/`Ft}CYpJ#9G(lJvv5D+U:fdv}Y[FF5Eq9 47zc*pr97", 69, (void *)3165, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ",6;7V1)0AxL79Ze/|5  0@`OazW", 27, (void *)18491, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
// Entries: 450
    rslt = aosCharPtree_insert(tree, "C!jgynmP;rQQy'-O(|38k@<@=", 25, (void *)19350, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "1%Xj}f@0+@.wa^)ZsD.,?KDBHSxRFTzDCgAwOzs#mRL^n=NF+KHWRJhO@3 dtcj|6DCW", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Bxm#|v)sRG==jcNdlnd`D^[#t+UG2A: ]GrWzZW?qSJx", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "4j6yV8Wz@r*{O#sgel}:hBd]dg-?<phP!w:+", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "M^Gzqak^zhpMfTpV5=UDLD;e.>8ISF~rc=_fqC=Fa be;i#%{EA5Nhm;?Rq87^{j(N%BG;ellyL3Mxle+%Xu", 84, (void *)20583, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "qG F<>09!r[V$m;FsI:v!(e}/'(xcg|kd^Fs|<`u1M:%]`)jaUhr{YO", 55, (void *)19935, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "lC~[2z*85*t>)bfjd?TMReJFsx[iq0rrn5EK8F_FS;C,,Nek6j(XeAW", 55, (void *)31240, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "`64SW^O=|U|d}9Kh_'s]&c,2::qN8bn@~/={y", 37, (void *)16949, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "(!,o!aP Icd{Bjh}0^J{D,)Ydq=N", 28, (void *)15269, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "vjYb Hj4N|J60/94|_Ji)qp2gN[D{wgfQ[9", 35, (void *)11551, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "3ah*^NAD)NCSA76[KAEH9a4mviw9mAP|>J*- D0e8J wQE[V3.Px'DfT!/-#$77Z}Cw5zMEq6TC^CMGY Df_<Jg#H]vh", 92, (void *)22633, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "NrxnKa)t'%@yg:U(!Q3jW[IMsr%O{9^1X:&'W0dBpjW!ZSM bAJ'y0q0&;MHGmN", 63, (void *)20041, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "FpLX^p0uk8Cn $lk5", 17, (void *)28037, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "f5R7?)vj?Y/<C].P4^14zi=}ea~MytrP]Hc|Eas,.M!ZMR8K{lho!Vt-X;]>5xWz0T#wZb}E@TgE*T", 78, (void *)15746, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "2va&wls,R]*cVDvNl5DfS8|NLhO$W?][-,_(j|X]>e8`3dzL/0Sz", 52, (void *)27288, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "^Mjj`T?", 7, (void *)27351, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Oy;", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "MSh~,@~NB!r%8hIO(.{sOI", 22, (void *)8258, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "RmL;w,_5#~w}&O-BWP/ xxc`*<M^g1@=KyS:cG-++", 41, (void *)5123, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc; 
    rslt = aosCharPtree_insert(tree, "/oFNEM~U{$6NG{p:9{Wx|4<A+aCfoo}@P*#SCq.h9Y~jhgha/mO&X*/TyVW+xB'&'Oo!*,DI", 72, (void *)2599, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "c3?St|xWzSI^d!!KzmA~'G,qvO(dqKH+ePK6lv6Ph@|/0X}&1%cu?~AHK_Nl_GYG!0iWc-xZBNH<%:NeGOad&h[^2OS8*H", 94, (void *)30743, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "v@#f{9uu2LtG`}QGo5TipDCtad{mf,4Oa#S-S(_PZrU2=pM$(HsG'1Xf-P}v!/J4P3R6t'($~N", 74, (void *)17567, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "j)a'G|&I$Mw'x@4kM54I}k]iX#mYWgiTV+-bomf>4Z Mo7]e", 48, (void *)15089, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ".Q5ysl1B<i!qQtLoWEYW#wC=}'T/-/Ip", 32, (void *)16102, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "#|wW", 4, (void *)17361, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Rc+y", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "y?>,mbL]Or}F_Wm^-b2z|6xGzT.x0`Zm%~B*b1a'Z", 41, (void *)6230, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, ".AQZ+?#|S?Db?G=m _MDP,`&idP!DD'`/Rj=//'[eQ^p,$Vjn80MmQc6tO~H$atM} ", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "O;Z!^5C+a#6.Sj=?;,X!GKWnD=>}[AoY?C~a@wV,4ch#_BU|kE}WZUP", 55, (void *)13301, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "NL}vST|NXWcE|oi_WJQ*E7lb4g&vP{X+RQ:7YR*vN_1[r]D5Hw1vy`", 54, (void *)21550, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "7~uy4fk", 7, (void *)28096, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "s.S*ep:V(wk=LwZi8~8~zg2!", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "HK-.`GmUs$}|9`5]6>O/`c`+", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "(c| :'vmTUB1A&r~S~3js<HhRTcfQKm?,2J}?z[BvD$x/iVy#X_&b)'<f8A'#)umXz&9>|o'i8LFZIay^c6,O9%oS", 89, (void *)23320, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "K^Wot04*dU<<oO0FU-``/x3D9|]R*ZORSpbv=piz l';HGS-^Nt'/Y' fRsGzM'AUDF{gT079bSV_hE1}IpqSfJV%", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "1BCr6ZZ[cpdL_.,ad@0@M>sC +(WC6", 30, (void *)19854, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "4D&wnkgEFi$IDHDWJ&+W8tCF6-C_y81u/+qU/u`u:>pX=F;Ynk+6*KM0Tc6Sl&3Ihh8L+5LU;R|vo,DIH[tE", 84, (void *)30633, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "DzP]9%(L,i8GA#C$bExc_e*Buff.3Y<Y:G/%]yfK", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "j]I0()4)DY?v.<DpMxJq-&_YB93X]Q0Sd4aSEZ8y'C4g9>U&cv|*~PQYMm-KjLEyk@mg2]d4$", 73, (void *)22432, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc; 
    rslt = aosCharPtree_insert(tree, "Bt8%BDj6{E=HXbSeUG*[JGfbn/S@dF)}wLra/_k>pj)!`>R^]VW,1mD5~zW@`Z7a.V{U!JRf*/R%Jg", 78, (void *)29047, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "f%7GCu|OOi'(`gez/Y5_mhN]0%._,Lu0g%_&YodXj]w74w^7p:MQH$:TsAv-o}w3{M1ic#NlI*BM", 76, (void *)25625, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "4[hYk^0a|x4t|M(", 15, (void *)8096, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "ayfydzGZ:mCk_b>WG>Y!5TNU?]rQeRTz4Qj%m8t!+d>B:mP%-Xldwny7hyZ=", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "}gP&8hBt>sf[M.{;,09L_<#]nxl$", 28, (void *)10797, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "rGgZ)ntJsbIU?I!k{9<fFd6{lW-*R-|0mlu1?ZWh", 40, (void *)1851, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "_h- i|l=iWi/11`[,` : RS?TbYe~3zlT*2IP@Hdv|y]lYRe_%.'jy%$g{e1t/7vTvCP|5V2,x0", 75, (void *)2624, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "qyPsRa!Pm^zIgL`Mk<o%IM5*rd j4pd;%+NuVfK[Rb", 42, (void *)9336, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "w>vESvbD6Nd3$F=}!}u{M*K7&k-:7ZD`au'?jy.v2Fy", 43, (void *)1866, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Apk8.A8*p``w4L}91@)<AE)P|", 25, (void *)31183, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "6R&Ez-xZhAi-uUgcm^I3Ber$O'<~6|*9Ctu#Q}wx|M0MebzGw#[R~wD?$3_#(Uxs@nOwiAV8I4jW2Y`+(c)&2||r.3%6$", 93, (void *)30353, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
// Entries: 500
    rslt = aosCharPtree_insert(tree, ".[3)SAl #<(shO3[~}aa5c2swA~Cz}0ar)8&P8,G(C;9'Nw*`EmF}Q[:=ExU:t9", 63, (void *)28398, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "5.t!3*R0=f;;!65C$@}Ka1bQZ>~gd&v;?!Rzqp*F/qMs*'LD@oNq40JMvq9mK @D{NTI0N8/9??}GH >2fAe1M2z%%n", 91, (void *)3438, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "yxGQtkJo", 8, (void *)4361, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "m4'{W1aJ^`,tFl=.[&mR7M'QJHf~_-%G=KH:uB3Fq>UV%:~)AXiMVB9x&MR{", 60, (void *)25482, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "*<+p~HNe(`,'v;%&5!R+Ll+0^#kYTDmzj ZM5i`0nc[.i<lgSQCS]$]gqA#xP>35&L<-i3ZIw}#", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "@Q}H;;<L`SdbhOzEVwc$8P`U}W9;#,0Tn!|L_~]4u-j$PM`:Us9", 51, (void *)5141, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "3=S+I&ujoe?JcF|aW}dYK;B.`]wQMW77]E(M-,2?16-gVdYOp9V.#Re^TI", 58, (void *)17957, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "O?Ee<:b'j2wD,;q&[G%{<y.OuR;TwWzFK_", 34, (void *)6632, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "HYiwsn8>E.YB_IcFgp'5Z=5#bN6T)Fqubt2 8e#p+LSQ(X]FAioX:1KMu2IuH=j`82O]5:c,woui", 76, (void *)29561, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "+=gM&z4unMlBj9QT6F>=h>$tDu`LxEra/djw0r^V37I", 43, (void *)17221, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Qp+}%E_wnspy 5h7txwkyXYR.ikG1~G?9ZV 7Fsq*kz*G:8 )8mj.N%a_E]jA(^F~gUXN", 69, (void *)17360, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "7Ta^h:q|s:f!3*884:9-]EXJ+_Ed+-|{l0>ssKaFG>(i", 44, (void *)9758, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "fQ[kMU", 6, (void *)5056, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Bb)eG63Xb*p{tjkoT0QMxN6;:<z5&*u4BJ,>Ja<dJ;N=O[1HS'.yID0~18|aDJ}<FH-v%c5kF~<", 75, (void *)18510, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Ug+J8B)6C9?+^?&x^8@{wt1L=B|wp_;%*74!&-[fk$!K'Pt!y2g|$IH&c8a`O&5y5>Y/z+{1E&V*|", 77, (void *)15496, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "&s$UX3>3wj}CO~,s`IU|{o;ElpBdQ>oHRX:<F-7/gZRClE^]#on", 51, (void *)9398, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "5_Ep@6iFk|Vji2Potx;~q'XqE&&F?bh}/8", 34, (void *)26480, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "xTx@m!c'GB%*]3y(<(L@i^b&.6wj{.K9-6jo{@4>k6l@^#bb[UP4OHr|.>pGiu7i%S:*T1NE}Ijzf.5", 79, (void *)18019, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "EJr+yQh(t}]$G8.soz&H-UvZ?b,@pnM8(p',-iq/yyFP=#,/A)", 50, (void *)13407, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "$P'rWWYM<mOco/pDRi-~]/,O4O)sDU5a^}jVvETS}}m9W'h.A", 49, (void *)15619, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "}x=/TJX(jhllnU~arRy'_y*r`*|bO$fulz*P", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc; 
    rslt = aosCharPtree_remove(tree, "?m_~+[5fP", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "tciI>PZ|K)Uu$@pY)Ijp^", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "/;}ZP!iNO", 9, (void *)4231, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "xyvVYI/BReZCJ-i?uUJ|bzxJp]*,", 28, (void *)8362, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Y}2[GQ`/TI77e:9#VCS&Ss", 22, (void *)27479, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "cX$W6]t4F`.yJ<->'L@@Dd+6TvW!I@hn)|_0n{-eWbt[.rgH:c%m3u*bqGrKG4s2{x.B!U[^nS2Tu77Z@", 81, (void *)27215, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "[34R?W=;?vR", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "]:4'^7@&]xa_KdX?i]YR5@}% ULBh0DfxJ[A+@]*b~FeW&<a7G_", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Kx`?zCf|b2;{BQy(Ygec=h;!~13)oiH1%s#@:6r2/XKF;W7}6{i6[n)RWP4cpo];lv9_-;BtAHGlYQ@>[C,/>", 85, (void *)24718, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ")`<>xRfMYd1YcUp};@fl4/d^", 24, (void *)32480, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "-y}yi~<y>{oM#UuP:A^Vg$$~0j1&_J}$Qf]Sp!4ofVEbd'1n%b^|M.t,H<_", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "<s=X_7<SGs|!</O#WAY[$G!}Wa.T", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "3^!R+c;q>%<aZZaJz^e$E54_`xitJ2[0wG/CphpG{t]JkBY^>P", 50, (void *)11448, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "';aB!pQDZMbGewKBol;L)YTE`~W#5_b66|O8j)P@|'R{0P&Kx;KmL<{R>4L_g@EuD[Air`ygyhb)<g;%,^h@!]", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "/Lb9(?aQ7q]%;/}H)22rP&o,f_KR$-][TAY~':,s|8y^1csea43+Vn1Fv{_eHInpG_f9T.gSVTD{.$^GU,+,TCfVaM", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "z-M.1|hy?[>R?7vpz)/r66,8f<3IF%HH%paWR;FGrHEA'qLjU#mFM.ZHWv9j#i)", 63, (void *)18583, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "5_U`w3u`'X0cX", 13, (void *)20378, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "xm#]%ke/d7W7<e4^fqBZ.Ta~c8/T|Uq!P%HAop72[i#PoQgN(,;l!n,)0PUkk3dPm:AK72z_8Don%*!Lmg92", 84, (void *)12415, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "U~UW,DznA+Kw:{qT(H~<WB!W'y>4RS`", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "wWS}yWK'*vki&_(x!6", 18, (void *)8413, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "/8~B^Io53x+1pBY=e3;5apij{.!Zk/?czpxI:NG", 39, (void *)24495, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "5}K'*[AOGw<]8u020v;2`NO", 23, (void *)15777, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "u9Zp'VqpssB]O&,x,08`bQ9[o4Em7~Y1$tzCuyBd~DNuBvW8qavOrS- ", 56, (void *)12835, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "TR|,ie4I7o7xpg}N8E2cHjm)TiiRCX@Aq)Oxh`#", 39, (void *)(18099+1), 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "(V=d(M?e5#<VFgTGV7DT0 Qn&#q4Ee?qwq$_1K3(.g6U&miMEm|#u.}Ab:?;iZG#b<twJn^c9'[", 75, (void *)10912, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ",M6:zImpn@@C", 12, (void *)15034, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "96jdeK#1KGY}7d7H1N.lRY=Y=IUgL`b8C=V3JbS}`Ioo%Bb]2W7xN{VqCn?CQx6te8l", 67, (void *)21894, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "g_#Uc==r65s5 ;}-(ac}ph1)M-CF9o%D6},l+=;cILnLUzmfYVfV!J%EX45h(3xF,h8%Ax}&AjU~2~fz8nBi1s`&", 88, (void *)15650, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "f%EWLeZk.LPprQ2@vl", 18, (void *)15107, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "j;tB:", 5, (void *)12876, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "2%A.}O-1)+b%EZ Okm0N+SU:iCu277thaR6>Ez<4:vaabvg>|iR&4@a<", 56, (void *)7642, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "e>gnM<K^i[LK%eM/_S(BWzD45_D", 27, (void *)4639, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "n.IK;;06h:#6}{h.X(s'LmlG!M5=,G<Jx?MmNO838L", 42, (void *)20344, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "8,a]Loq.t2Lm5ua1N-d`-a*^=^9E rU[##7I*p*]}W.ipnc$U7N%qnh>MmB18<B,<3Hf1)M[g/]rh4 )f 2wUg!z+^R32}", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "vPbq|~(0-v~_j5+", 15, (void *)28852, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "F*sb5`1+;M*Z<XN*eL=HW]lA=", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "]zJmlG(z8-sp*xwo|ZrxGZo23tNfV')j%NKnh4hgd_bnO/khxF~", 51, (void *)10150, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "29%?}r3T`3g;ChJe*-oZww657^gx9CtO Z$0DJ", 38, (void *)25046, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "X=<?]/KGb4W&f/Bl2c7sn'_Ib*UPrJQZ<^3?=tP", 39, (void *)2001, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "X0j[[TKbcACV%Cv>V&2GI=%<'xFh1n*id1NIdgU;", 40, (void *)6804, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "K&`!LQ)KYd(<# _m#js1OX_8:y|KQrpK2&Wpb*?6Xcz+-ykuxw%BF!aP%V:9/ETTCr=M4inH@B]LVZf_E3$1", 84, (void *)21400, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "jI!UK?*tc^Eoe9|a%_R6/tq3^eBTEx$HkPi!mNu.L;_Z9H],)", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "k6)P|&(#c1JgZ6<??uH6rOMmJ7*|Ys!,('=SaL%uy2hLi {a&TwEmM&Cm_J1", 60, (void *)6795, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "c*PNa>WgbbbKWk$])@y5QA", 22, (void *)12784, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "5jq;9n3u{tsKwXYftWeMwxJabU1)6'Eih@VJXSyV> %w+E|o2BkF!Xy1BC&;1`5Mqv>GxB@OW", 73, (void *)20708, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "tH=GtP+p6?%!x//'Y>Ve=O`sX*zH3y([Bnw+9dRj%U(WaPM aI7!8pFfdm('cjv?0dZTEPL*", 72, (void *)26812, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ">~dAdyk@e|ktY%OHM#5xA;#c9g@?V/G~lKUpZcP#)uG%(`e/7Bfik5,)", 56, (void *)17028, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "T}66W~%D2<I.^:.L,e|+|k9B84mubdGx:Uzsy1?%T8P-{?sQLx.#)p8RBj88~AONY.AgNng*bs8u$!5JqIX:tsBI(", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "]XaD%!xG13fukkUg?)+>ZkbKMXe&9Oa<[OmI;_`", 39, (void *)17861, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "@4IGoH&?(QL{ *<5;3<vSLNFLS+p nZTBpCKx]Gtf_45Wz-^8E-DOqBM$PtQeR? jA`<Z9:jS&[(oOtC]aII", 84, (void *)18465, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "9Nh~ yD*7tZGpC[h|PuU8,CC|d", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "d~SmzI9Hupa>Zdf^t/2J^GRw8Xe69<-pKkJ!zo/M{b04V_u-.wD79UM*4q0nad5r-#?& =ZhzR >0^+7hJ5-D", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "B{IBSwIN/@5r?PE626[a86U(bm2=NU'N-$w'PBM2Y[$~+j_Ho~@tnW6i!9-i)b<.@;;SS-O1H=XNGT}9fH8nbn)*,?ruI", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "1*-$7cvm-#&Dr8{QA(", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, " TkH7[(feZ3O)u';.7M,a8:scN>qS4I'y~8k&F,N7Y}'Q&pTB|Hb/sQ un-$7E&Z9s`)kP.S&9.J-Hhla@4ZKu;j3= 9u", 93, (void *)15815, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "I9=BgcWa;Ra0@t m", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "cS-Z", 4, (void *)12022, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "4/kcm2!/l# }rh@$v}0[PAJy;lQmaLjTh;YjUa]:B63!q[a(:n5~:-}5@u f];=|41VnCw]u))#%z^>z[f{OOQ", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "qtHjk%BbD*w.T_bego_@u(j]IEWoCQ-/.i1|UvM-|v7/-]m``0&u9PJJ^ht@)}p{EQp~q;q/", 72, (void *)13477, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "7<-_s2Q}FHtmEF1)#I?E(lBzDz<?K?HU&zI", 35, (void *)13457, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "xA ,XNe~7]B0[bU78{u99fZ%1WU4NW3y iqW':y^3;;:iHi7lS", 50, (void *)17387, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "s5@%>)XloDW!Z=LyC,1!j8NHnIh@eqKq22llYZLb)=,]@oa9@/B^P", 53, (void *)14677, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "=r", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "Q~", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "%~J's^ZK$LQ4x-:{:o&7Xx>%wkaLIh_B]GWrHxvG,BOTsMH}8@#7XO4E8Mxd0evw?:Z1/zGc", 72, (void *)3893, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "LH<jecB.X7F2!27MrR1nz-MC( fWX)v-$pv`jgn^+Lnb##ryG8*n6Acm@0", 58, (void *)32629, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "r`K~j))9bS@*Alrnb_z$e6(fPjY?'i>ox/[dY/{fR9Cj1FJ[-e", 50, (void *)3998, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "7*lmE-m}qZQ/v20]ZT|{f $^JRBhEs*!Y 0DG1hyn_s", 43, (void *)8113, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "@1IR||HSfG=}Z3Tdh}A_D;=6WQ[fNJ", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "'pv?0i++>ga<IFnOq.8BVz,[k", 25, (void *)6745, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "0#}%u", 5, (void *)9984, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "R.^", 3, (void *)11979, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "KBFA+-q[I >'`c?iA`1b:Efb^v[;b+}4y6>{b[3g4sK5pt5RW", 49, (void *)14651, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ".fEC2s$l0W4OxgOba ^Th@#r.gu", 27, (void *)28431, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "cn4J{R* N0!OVY(==+FH5lRG%q>0nr7>|2bx9Mpety ^5XS4Tm@U&vO?IbxSST8j96m", 67, (void *)18623, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "OEF=9mLtue.F)J7bC:R|3v3jY]}ihLslQ<_#%I/dLFkbFi~1Oc- HdT]sC~q[I", 62, (void *)14535, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ":mHeR3I&/=FdfnMSblR00-'wrL", 26, (void *)26030, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "T1;(d$", 6, (void *)22693, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "#RxddIJB.2miPv<Q@!.6V`+nDoBEF4#3j;(W'g>h:$$z^K[k^VxKKsQ~7JxJ0;eNK?j&(hyp;bH", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "xI(iCsu//wgp>dVnB&(BWcWE|*YZ](d%M/cQ0[>]8bHlH|y)l:FllCdf](X&#=`f8WP2fKa94nfr<rK", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "qlm$'z=}^x.VEAz8)2#:h`i;E9zlW0$B=Q=", 35, (void *)27939, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "V&4GAd[3", 8, (void *)19570, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "wWO%.,_.F_3[Ed>'5fl$,C^mjQU_/}H/0>SFP}L", 39, (void *)26196, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "ZnRIh;Cgr=}';y}J'Y#.Qi`I(2)8$!xI;w.DJ}(F&", 41, (void *)31179, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "tt;W2]2:IZ`{|K]", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "LT6GxM.h&bRmX}Yy! ;DM<$0U4MW]!]B-|g0O!Y7ITKjB}", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Do#UNCC)9Y$7jo]]Ol1j8L $:fFBudj1^u%z70VM3iH~9t q2GEE]MfbRn*cS{;4kA|-O{N=j_+fJ", 77, (void *)20356, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ".&ruOGO$6-=umJ&3$Tz", 19, (void *)19888, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "od4M/HLKi6_AFmQ=lEXls&~IS}:G#kk#{Ze&2a#V}KT69|3nhk8E+2FO$~,ERkoAZ;LJ5 |fgA-}/zn9", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "^y0> H1bYgj@N^?9d7qP`mLHD3g_fVw;jlurtfw1( :.`_MJ[Z9#kI?Eal$}%/g3x.hK9jZweveYS E;", 80, (void *)14636, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "T-KI{1Bb&4Xk<Jzy8(|Gh63k2(", 26, (void *)9759, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "gv3'R:S}BV&>~]Ra+kx`EIjc4MiYd8FC!5sTG]}5*h", 42, (void *)18631, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "GlXUs&7xR3.0GB,g-qON_B{i>x2[oRUkRjX|2ZPP}dXh dPoc,05#Ot$rN yAb4uUf=.}qH)U3FTOz?8<+U0oq?&", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, " 9{vqVCpUl&", 11, (void *)15691, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "YJc1EopoW&fur5@]JrTbp[a7HwkB>J82zB`ad5OvB,Cl#:_%wehB{tJYr#}.5_15]GAq<Rcyl=.#*,t=BKD82X3iXJ00)", 93, (void *)9439, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "[g@zI=rS>}mj#*skVw}o7-2![{a[Ej-mm^vC7`W>J,jHr( -E6U!='7&Rgll@`<;Kn4 3`7#m4_J>VO{6.k", 83, (void *)2842, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "(s@5vy,# @nbAGl*Ioh6=Vp!C5aVD20U4^YjIXS=nawZ7yef*a", 50, (void *)13753, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "cL=vTRNa7u?GGC", 14, (void *)13676, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "vikttZMnbQN4;kH@u+", 18, (void *)28118, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, ":oSbiJPnDo=,.$0[R-5enj$9*_;D.4IZL<`XC^R{C!Mvc>rg.Z", 50, (void *)28201, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "A>Rmi)}p1(wI1,L/;5w", 19, (void *)14767, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "<c1 _H89>qx3M7??F'O8+,Y:zWF]]H}(sNI{#[>nH`O.jn2?4F YJa|UI", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "e{U^fd.Gw2AK)^@@", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Az4n$_?_y._DuE'^M:,i<2MZb*IEH?<IqjswTFO", 39, (void *)3650, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "y'{0GpHarHh{oNLv(Fo,To#/L:Jj@", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "H:@j:_O(@(p~7a", 14, (void *)29158, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "?+krnfdDy", 9, (void *)13551, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "j&O/CD0.^%u", 11, (void *)11903, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "CO;:%$ww:shu;QPJ!5n=gLUK><*(}f89K4 Ca?hGY3m $@]@c`i", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "k<k22*)@gT^&!2hAi%,J;XdSyew9;Q).VWjZ-u'9xMmK:XBA&DT&bzK-(d45UP;lFkx2W", 69, (void *)9538, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "dE8(E.S`x5Vu#7D@MW<;I11", 23, (void *)8191, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "My56IJ}lZ.ER*:PKQ/Iwx?", 22, (void *)20928, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "n9?[@rY88,|sk|DDu;_0vg{3TVt13uO", 31, (void *)21470, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "LTaZL:P!SOr^WTc8KMXF|THonKPr*S<=YJ)318[/37", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "6%vtqi:pwNDs:;&gF9", 18, (void *)16559, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "<O|RJ-~5v9:'!=FNlS}TsF6aF]vGz.fI90klNKS", 39, (void *)2952, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_remove(tree, "+hg Fz%.cx-(2`Zl:!u5(#", &deleted);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "Ni&{?", 5, (void *)26710, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "v,*CQoHmJHa'<}%#NYuOq$*D*,Kh!OMJIDWq%kx`m4)e3&gL5FLnw~u;", 56, (void *)7821, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "~9_$X@{t9Q$`0~3pdYyAI#%(&@,%`!*wD!xjsSqxzle3JS=5fdt/", 52, (void *)14370, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
    rslt = aosCharPtree_insert(tree, "tGTr:uqTXT?<_LH=b&Kk ua9]=>gP,u9qY]OWTOQZHxxUED+(3o7=8jr%o4D;[$6eo7)a-Q0U{2&tj", 
		strlen("tGTr:uqTXT?<_LH=b&Kk ua9]=>gP,u9qY]OWTOQZHxxUED+(3o7=8jr%o4D;[$6eo7)a-Q0U{2&tj"), (void *)28284, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

    rslt = aosCharPtree_insert(tree, "E", 1, (void *)1511, 1);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

#endif
	return true;
}


bool OmnCharPTreeTester::specialTest15()
{
#if 0
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest15";
	struct aosCharPtree * tree = aosCharPtree_create();
 
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
	int rslt = aosCharPtree_insert(tree, "", 0, (void *)(99+1), 0);
	OmnTC(OmnExpected<int>(eAosAlarmEmptyKey), OmnActual<int>(rslt)) << endtc;
 
	rslt = aosCharPtree_insert(tree, "", 0, (void *)(99+1), 1);
	OmnTC(OmnExpected<int>(eAosAlarmEmptyKey), OmnActual<int>(rslt)) << endtc;

	int deleted;
	rslt = aosCharPtree_remove(tree, "", &deleted);
	OmnTC(OmnExpected<int>(eAosAlarmEmptyKey), OmnActual<int>(rslt)) << endtc;

	int index; 
	void *node;
	rslt = aosCharPtree_get(tree, "", &index, &node);
	OmnTC(OmnExpected<int>(eAosAlarmEmptyKey), OmnActual<int>(rslt)) << endtc;

#endif
	return true;
}


bool OmnCharPTreeTester::specialTest16()
{
#if 0
	int index; 
	void *node;
	OmnBeginTest << "Test CharPTree";
	mTcNameRoot = "TArray-SpecialTest15";
	struct aosCharPtree * tree = aosCharPtree_create();
 
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(tree != 0)) << endtc;
 
	rslt = aosCharPtree_get(tree, "test", &index, &node);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	OmnTC(OmnExpected<int>(99+1), OmnActual<int>((int)node)) << endtc;
	OmnTC(OmnExpected<int>(3), OmnActual<int>(index)) << endtc;

	rslt = aosCharPtree_insert(tree, "test", 4, (void *)200, 1);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	rslt = aosCharPtree_get(tree, "test", &index, &node);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	OmnTC(OmnExpected<int>(200), OmnActual<int>((int)node)) << endtc;
	OmnTC(OmnExpected<int>(3), OmnActual<int>(index)) << endtc;

	rslt = aosCharPtree_insert(tree, "test", 4, (void *)300, 0);
	OmnTC(OmnExpected<int>(eAosAlarmEntryAlreadyExist), OmnActual<int>(rslt)) << endtc;
	OmnTC(OmnExpected<int>(200), OmnActual<int>((int)node)) << endtc;
	OmnTC(OmnExpected<int>(3), OmnActual<int>(index)) << endtc;

#endif
	return true;
}


