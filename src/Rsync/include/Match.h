#ifndef MATCH_H
#define MATCH_H

#include <sys/types.h>
#include "Rsync.h"
#include <vector>

typedef unsigned int uint32;
typedef unsigned short tag;
#define TABLESIZE (1<<16)
#define NULL_TAG ((tag)-1)
#define gettag(sum) (((sum)>>16) + ((sum)&0xFFFF))

typedef struct
{
    tag t;
    int i;
}target;

class Match
{
    public:
        int false_alarms;
        int tag_hits;
        int matches;
        vector<target> targets;

        tag tag_table[TABLESIZE];
        off_t last_match;
    public:
        Match();
        virtual ~Match();
        static bool cmp(target t1, target t2);
        void build_hash_table(Sum_Struct *s);//以16位的校验码为key，建立hash表
        void matched(int f,Sum_Struct *s,char *buf,off_t len,int offset,int i);
        void hash_search(int f,Sum_Struct *s,char *buf,off_t len);
        int match_sums(Sum_Struct *s,char *block_buf,int len,uint32 checksum1);
    protected:
    private:
};

#endif // MATCH_H
