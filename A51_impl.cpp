// A51_impl.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "coder.h"
#include <time.h>

coder_w_key_t keys[KEY_SPACE_SIZE];

int _tmain(int argc, _TCHAR* argv[])
{
	clock_t start, end;
	start = clock();
	printf("Processing...\n");
	init_all_keys(keys);

	/*create initial key space */
	key_space_t* init_space;
	init_space = (key_space_t*)malloc(sizeof(key_space_t));
	init_space->sp_size = KEY_SPACE_SIZE; //initial space contains all posible keys
	init_space->keys_list = &keys[0];

	bin_tree_lvl_t tree_lvl;
	tree_lvl.leafs_cnt = 1;
	tree_lvl.leafs = init_space;
	tree_lvl.level = 0;

	do {
		key_space_t* new_leafs = NULL;
		next_tree_lvl(tree_lvl, new_leafs);
		printf("\nthis tree level consists of %d leafs\n", tree_lvl.leafs_cnt);
	} while(tree_lvl.leafs_cnt != 0);
	end = clock();
	printf("\nDONE!\nWe need %d ciphertext bits to success hacking secret key\n", tree_lvl.level-1);
	printf("key search took %f sec for %d keys\n",(double)(end-start)/CLOCKS_PER_SEC,KEY_SPACE_SIZE);

	return 0;
}

