#include <stdio.h>
#include "aosUtil/UrlPtree.h"

int main( int argc, char**argv)
{

	struct aosUrlPtree *tree = aosUrlPtree_create();
	void * entry;
	int ret;
	
	/*
	for(ret=0;ret<256;ret++)
	{
		printf("%d:%c,",ret,ret);
	}
	*/

	aosUrlPtree_insert(tree,"*xxxx*",(void*)0,0);
	aosUrlPtree_insert(tree,"*zzh*",(void*)0,0);
	aosUrlPtree_insert(tree,"www.yahoo.com",(void*)0,0);
	aosUrlPtree_insert(tree,"www.yahoo.com.cn",(void*)1,0);
	aosUrlPtree_insert(tree,"www.google.com",(void*)2,0);
	aosUrlPtree_insert(tree,"mail.google.com",(void*)3,0);
	aosUrlPtree_insert(tree,"mail.google",(void*)4,0);
	aosUrlPtree_insert(tree,"abc",(void*)5,0);
	aosUrlPtree_insert(tree,"a",(void*)6,0);
	aosUrlPtree_insert(tree,"ab",(void*)7,0);
	aosUrlPtree_insert(tree,"abd",(void*)8,0);
	aosUrlPtree_insert(tree,"ac",(void*)9,0);
	aosUrlPtree_insert(tree,"aww.google.com",(void*)10,0);
	aosUrlPtree_insert(tree,"wx",(void*)11,0);
	aosUrlPtree_insert(tree,"wx123",(void*)12,0);

	aosUrlPtree_print(tree);

	ret = aosUrlPtree_get(tree,"http://www.google.com/search?hl=zh-CN&q=xxxx&btnG=Google+7%B4%A2&lr=",&entry);
	if(ret==0)
	{
		printf("Match\n");
	}
	else
	{
		printf("Not Match\n");
	}


	aosUrlPtree_remove(tree,"wx123",0);

	aosUrlPtree_print(tree);

	struct aosCharPtreeNode * node;
	node = aosCharPtree_listFirst( tree->tree1, tree->tree1->mRoot );

	while(node)
	{
	#define LEN 200
		char key[LEN];

		ret = aosCharPtree_fullKey(tree->tree1, node, key, LEN);
		if(ret<=0)
		{
			printf("\nthe length of key too small\n");
			key[0]=0;
		}
		key[LEN-1]=0;
		printf("Next Node:%c,%s \tkey:%s\n", 
				node->mBranchChar+tree->tree1->mMinChar, 
				node->mAddition,
				key	
				);
		node = aosCharPtree_listNext( tree->tree1, node, 0);
	}


	node = aosCharPtree_listFirst( tree->tree2, tree->tree2->mRoot );

	while(node)
	{
	#define LEN 200
		char key[LEN];

		ret = aosCharPtree_fullKey(tree->tree1, node, key, LEN);
		if(ret<=0)
		{
			printf("\nthe length of key too small\n");
			key[0]=0;
		}
		key[LEN-1]=0;
		printf("Next Node:%c,%s \tkey:*%s\n", 
				node->mBranchChar+tree->tree1->mMinChar, 
				node->mAddition,
				key	
				);
		node = aosCharPtree_listNext( tree->tree1, node, 0);
	}



	return 0;
}

