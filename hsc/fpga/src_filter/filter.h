#ifndef __FILTER__
#define __FILTER__

#include <ac_int.h>
#include "../../cpu/common.h"

#define MCU_SIZE        512

typedef ac_int<PIXEL_WIDTH,false> t_pixel;
typedef ac_int<32,false>          t_mcu_data;
typedef ac_int<1,false>           t_bool;
typedef ac_int<2,false>           t_quad;
typedef ac_int<9,false>           t_col;
typedef ac_int<8,false>           t_row;
typedef ac_int<4,false>           t_frame;
typedef ac_int<17,false>          t_hist;
typedef ac_int<4,false>           t_state;

void filter(t_pixel &in_data, bool &in_data_vld, t_pixel &out_data, t_mcu_data mcu_data[MCU_SIZE]);

#endif
