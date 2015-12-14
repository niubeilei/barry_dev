#include "../include/Match.h"
#include "../include/MD5.h"
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <cstring>
#include <iostream>
using namespace std;
Match::Match(){}
Match::~Match(){}
bool Match::cmp(target t1,target t2)
{
    return t1.t < t2.t;
}
void Match::build_hash_table(Sum_Struct *s)//以16位的校验码为key，建立hash表
{
    int i;
    target tem_target;
    for (i=0;i<s->count;i++)
    {
        tem_target.i = i;
        tem_target.t = gettag(s->sums[i].sum1);
        targets.push_back(tem_target);
    }
    sort(targets.begin(),targets.end(), Match::cmp);
    for (i=0; i<TABLESIZE; i++)
        tag_table[i] = NULL_TAG;

    for (i=s->count-1;i>=0;i--)
    {
        tag_table[targets[i].t] = i;
    }
}
int Match::match_sums(Sum_Struct *s,char *block_buf,int len,uint32 checksum1)
{
    tag t = gettag(checksum1);
    int j = tag_table[t];
    if(j != NULL_TAG)// find this 16bit value in hashtable
    {
        int i = targets[j].i;
        if(checksum1 == s->sums[i].sum1)// this checksum1 exit,then compare MD5
        {
            char md5[33];
            bzero(md5,33);
            Checksum::get_checksum2(block_buf,len,md5);
            //MD5::TransTo32ByteMd5(block_buf,md5);
            for(int k = 0; k < s->count; k++)
            {
                if(strcmp(s->sums[k].sum2,md5) == 0)
                {
                    return k;
                }
            }
        }
    }
    return -1;
}





