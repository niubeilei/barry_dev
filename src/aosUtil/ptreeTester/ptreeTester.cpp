#include <stdio.h>
#include "aosUtil/CharPtree.h"
int main( int argc, char**argv)
{

	struct aosCharPtree *tree = aosCharPtree_create();
	void * entry;
	int ret;
	int i;
	
	aosCharPtree_init(tree);

	aosCharPtree_insert_pack(tree,"xxxx",0,(void*)0,0,1);
	aosCharPtree_insert_pack(tree,"www.yahoo.com",0,(void*)0,0,0);
	aosCharPtree_insert_pack(tree,"www.yahoo.com.cn",0,(void*)1,0,0);
	aosCharPtree_insert_pack(tree,"www.google.com",0,(void*)2,0,0);
	aosCharPtree_insert_pack(tree,"mail.google.com",0,(void*)3,0,0);
	aosCharPtree_insert_pack(tree,"mail.google",0,(void*)4,0,0);
	aosCharPtree_insert_pack(tree,"abc",0,(void*)5,0,0);
	aosCharPtree_insert_pack(tree,"a",0,(void*)6,0,1);
	aosCharPtree_insert_pack(tree,"ab",0,(void*)7,0,0);
	aosCharPtree_insert_pack(tree,"abd",0,(void*)8,0,1);
	aosCharPtree_insert_pack(tree,"ac",0,(void*)9,0,0);
	aosCharPtree_insert_pack(tree,"aww.google.com",0,(void*)10,0,0);
	aosCharPtree_insert_pack(tree,"wx",0,(void*)11,0,0);
	aosCharPtree_insert_pack(tree,"wx123",0,(void*)12,0,0);
	aosCharPtree_print(tree);
	printf("========================\n");

	aosCharPtree_remove_pack(tree,"wx",&entry);
	aosCharPtree_remove_pack(tree,"mail.google",&entry);
	aosCharPtree_remove_pack(tree,"xxxx*",&entry);
	aosCharPtree_remove_pack(tree,"xxxx*",&entry);
	aosCharPtree_print(tree);


	ret = aosCharPtree_get_pack_recursion(tree,"http://www.google.com/search?hl=zh-CN&q=xxxx&btnG=Google+%E6%9C%E7%B4%A2&lr=",&i,&entry);
	if(ret==0)
	{
		printf("Match\n");
	}
	else
	{
		printf("Not Match\n");
	}


	struct aosCharPtreeNode * node;
	node = aosCharPtree_listFirst( tree, tree->mRoot );

	while(node)
	{
#define LEN 200
		char key[LEN];

		ret = aosCharPtree_fullKey(tree, node, key, LEN);
		if(ret<=0)
		{
			printf("\nthe length of key too small\n");
			key[0]=0;
		}
		key[LEN-1]=0;
		printf("Next Node:%c,%s \tkey:%s\n", 
				node->mBranchChar+tree->mMinChar, 
				node->mAddition,
				key	
				);
		node = aosCharPtree_listNext( tree, node, 0);
	}



	return 0;
}



