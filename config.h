#ifndef CONFIG_H
#define CONFIG_H
#include "stdafx.h"
#include "inttypes.h"
#include <math.h>

//register lengths
#define LX 7//3
#define LY 8//4
#define LZ 11//6

//number of possible bit combinations 
//for every register
#define MAX_X 127//7
#define MAX_Y 255//15
#define MAX_Z 2047//63

#define KEY_SPACE_SIZE 66292095//6615

//indexes of control bits
#define CTRL_X 3
#define CTRL_Y 2
#define CTRL_Z 6//3

#define WORD_SIZE 16
#define WORD uint16_t
#define WORD_EXP 4
#define ONE 1U

//indexes of feedback connections
//lsb is Xt12, msb is X0
//[bitN|..|..|..|..|bit0]
//[lsb|..|..|..|..|msb]  output of register is always msb
const char FEED_BACK_INDS_X[] = {4, 6};//{1, 2};
const char FEED_BACK_INDS_Y[] = {5, 7};//{1, 3};
const char FEED_BACK_INDS_Z[] = {8, 10};//{2, 5};

#endif // CONFIG_H_INCLUDED
