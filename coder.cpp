#include "stdafx.h"
#include <unordered_map>
#include "coder.h"

/* generate all possible keys and save them in coders */
void init_all_keys (coder_w_key_t* keys) {
	WORD reg_x = ONE;
	uint32_t n = 0;
	for (uint16_t i=0; i<MAX_X; i++) {
        WORD reg_y = ONE;
		for (uint16_t j=0; j<MAX_Y; j++) {
            WORD reg_z = ONE;
			for (uint16_t k=0; k<MAX_Z; k++) {
				//i save a little space :)
				keys[n].reg_x = reg_x; keys[n].reg_y = reg_y; 
				keys[n].reg_z = reg_z;
				if (n!=0) keys[n-1].next = &keys[n];
                reg_z++; 
				n++;
            }
            reg_y++;
        }	
        reg_x++;
    }
}

void calc_coder_next_state(coder_w_key_t* c) {
	uint8_t bit1 = arrayGetBit(c->reg_x, LX-CTRL_X-1);
	uint8_t bit2 = arrayGetBit(c->reg_y, LY-CTRL_Y-1);
	uint8_t bit3 = arrayGetBit(c->reg_z, LZ-CTRL_Z-1);
	uint8_t maj_out = (bit1&bit2)|(bit1&bit3)|(bit2&bit3);
	//must shift only if second parametr=0
	next_state_X_reg(c, maj_out^bit1);
	next_state_Y_reg(c, maj_out^bit2);
	next_state_Z_reg(c, maj_out^bit3);
}

void next_state_X_reg(coder_w_key_t* coder, uint8_t shift) {
	WORD reg_x =  coder->reg_x;
	uint8_t bit1 = arrayGetBit(reg_x, LX-FEED_BACK_INDS_X[0]-1);
	uint8_t bit2 = arrayGetBit(reg_x, LX-FEED_BACK_INDS_X[1]-1);
	if (!shift) reg_x /= 2;
	if (bit1^bit2) {
		arraySetBit(reg_x, LX-1);
	}
	coder->reg_x = reg_x;
}
void next_state_Y_reg(coder_w_key_t* coder, uint8_t shift) {
	WORD reg_y =  coder->reg_y;
	uint8_t bit1 = arrayGetBit(reg_y, LY-FEED_BACK_INDS_Y[0]-1);
	uint8_t bit2 = arrayGetBit(reg_y, LY-FEED_BACK_INDS_Y[1]-1);
	if (!shift) reg_y /= 2;
	if (bit1^bit2) {
		arraySetBit(reg_y, LY-1);
	}
	coder->reg_y = reg_y;
}
void next_state_Z_reg(coder_w_key_t* coder, uint8_t shift) {
	WORD reg_z =  coder->reg_z;
	uint8_t bit1 = arrayGetBit(reg_z, LZ-FEED_BACK_INDS_Z[0]-1);
	uint8_t bit2 = arrayGetBit(reg_z, LZ-FEED_BACK_INDS_Z[1]-1);
	if (!shift) reg_z /= 2;
	if (bit1^bit2) {
		arraySetBit(reg_z, LZ-1);
	}
	coder->reg_z = reg_z;
}

uint8_t compare_states(coder_w_key_t* c1, coder_w_key_t* c2) {
	return ((c1->reg_x)^(c2->reg_x))||((c1->reg_y)^(c2->reg_y))||((c1->reg_z)^(c2->reg_z)); 
}

uint8_t coder_out(coder_w_key_t* c) {
	return (arrayGetBit(c->reg_x,0)^arrayGetBit(c->reg_y,0)^arrayGetBit(c->reg_z,0));
}

//modify current tree
void next_tree_lvl(bin_tree_lvl& tree_lvl, key_space_t* new_leafs) {

	uint16_t new_lvl = tree_lvl.level+1;
	uint16_t leafs_cnt = 0;
	uint16_t max = tree_lvl.leafs_cnt;
	new_leafs = (key_space_t*)malloc(max*sizeof(key_space_t));

	for (uint16_t i=0; i<tree_lvl.leafs_cnt; i++) {
		key_space_t key_sp = tree_lvl.leafs[i];
		if (key_sp.sp_size > 2) {
			//create two subspaces
			key_space_t *space_4_nulls, *space_4_ones;
			space_4_nulls = (key_space_t*)malloc(sizeof(key_space_t));
			space_4_ones = (key_space_t*)malloc(sizeof(key_space_t));
			//and counters for them
			uint16_t size_sp0=0, size_sp1=0;
			coder_w_key_t *key0_last=NULL, *key1_last=NULL;

			coder_w_key_t* key = key_sp.keys_list;
			process_key(key, size_sp0, size_sp1, key0_last, key1_last, space_4_nulls, space_4_ones);

			do {
				key = key->next;
				process_key(key, size_sp0, size_sp1, key0_last, key1_last, space_4_nulls, space_4_ones);
			} while(key->next != NULL);

			space_4_nulls->sp_size = size_sp0;
			space_4_ones->sp_size = size_sp1;

			if (size_sp0>1) {
				//remove all keys w/ same registers state
				remove_same_keys(space_4_nulls); 
				if (space_4_nulls->sp_size>1) {
					//add keyspaces to tree spaces array
					++leafs_cnt;
					if (leafs_cnt >= max) {
						max <<= 1;
						new_leafs = (key_space_t*)realloc(new_leafs, sizeof(key_space_t)*max);
					}
					new_leafs[leafs_cnt-1] = *space_4_nulls;
				}
			}
			if (size_sp1>1) {
				//remove all keys w/ same registers state
				remove_same_keys(space_4_ones); 
				if (space_4_ones->sp_size>1) {
					//add keyspaces to tree spaces array
					++leafs_cnt;
					if (leafs_cnt >= max) {
						max <<= 1;
						new_leafs = (key_space_t*)realloc(new_leafs, sizeof(key_space_t)*max);
					}
					new_leafs[leafs_cnt-1] = *space_4_ones;
				}
			}
		}
	}
	//update tree
	tree_lvl.level = new_lvl;
	tree_lvl.leafs_cnt = leafs_cnt;
	free(tree_lvl.leafs); //free memory with old leafs
	tree_lvl.leafs = new_leafs;
	printf("\n---===new [%d] level was built===---",new_lvl);
}

void remove_same_keys(key_space_t* ks) {
	typedef std::unordered_map<uint32_t,coder_w_key_t*> hash_t;
	hash_t keys_tbl;
	coder_w_key_t* c = ks->keys_list;
	uint16_t cntr = 0;
	do {
		uint32_t k = (c->reg_x<<(LZ+LY))|(c->reg_y<<LZ)|(c->reg_z);
		hash_t::iterator it = keys_tbl.find(k);
		if (it == keys_tbl.end()) {
			keys_tbl.insert(hash_t::value_type(k, c));
		}
		c = c->next; 
		cntr++;
	} while(cntr < ks->sp_size); //while(c->next!=NULL)
	//update keyspace keys list
	printf("\nkeys num = %d", (uint32_t)keys_tbl.size());
	hash_t::iterator it = keys_tbl.begin();
	ks->keys_list = c = it->second;
	uint16_t j = 1;
	while (++it != keys_tbl.end()) {
		c->next = it->second;
		c = it->second;	
	}
	ks->sp_size = (uint32_t)keys_tbl.size();
	c->next = NULL;
}

void process_key(coder_w_key_t* key, uint16_t& size_sp0, uint16_t& size_sp1, coder_w_key_t*& key0_last, 
			coder_w_key_t*& key1_last, key_space_t* space_4_nulls, key_space_t* space_4_ones) {

	// loop over all keys in this keyspace
	calc_coder_next_state(key);	//////
	//////////////////////////////////////
	if(coder_out(key)) {
		if (!size_sp1) {
			space_4_ones->keys_list = key;
		} else {
			key1_last->next = key;
		}
		key1_last = key;
		size_sp1++;
	} else {
		if (!size_sp0) {
			space_4_nulls->keys_list = key;
		} else {
			key0_last->next = key;
		}
		key0_last = key;
		size_sp0++;				
	}
}