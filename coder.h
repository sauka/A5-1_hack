#ifndef CODER_H
#define CODER_H

#include "config.h"

#define arrayGetBit(_array,bit) (((_array) & (ONE<<(bit)))>>(bit))
#define arraySetBit(_array,bit) ((_array) ^=(ONE<<(bit)))

typedef struct coder_w_key {
	struct coder_w_key *next;
	WORD reg_x;
    WORD reg_y;
    WORD reg_z;
} coder_w_key_t;

typedef struct key_spase {
    uint32_t sp_size;
    coder_w_key_t* keys_list;
} key_space_t;

typedef struct bin_tree_lvl {
    uint16_t level;
    uint16_t leafs_cnt;
    key_space_t *leafs;
} bin_tree_lvl_t;

/* functions */
void init_all_keys(coder_w_key_t* keys);
void calc_coder_next_state(coder_w_key_t* coder);
uint8_t majority_func(coder_w_key_t* c);
void next_state_X_reg(coder_w_key_t*, uint8_t);
void next_state_Y_reg(coder_w_key_t*, uint8_t);
void next_state_Z_reg(coder_w_key_t*, uint8_t);
uint8_t compare_states(coder_w_key_t*, coder_w_key_t*);
uint8_t coder_out(coder_w_key_t* coder);
void next_tree_lvl(bin_tree_lvl& tree_lvl, key_space_t* new_leafs);
void remove_same_keys(key_space_t*);
void process_key(coder_w_key_t*, uint16_t&, uint16_t&, coder_w_key_t*&,coder_w_key_t*&, key_space_t*, key_space_t*);
#endif // CODER_H