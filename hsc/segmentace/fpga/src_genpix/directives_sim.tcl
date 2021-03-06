//  Catapult University Version 2011a.126 (Production Release) Wed Aug  8 00:52:07 PDT 2012
//  
//  Copyright (c) Calypto Design Systems, Inc., 1996-2012, All Rights Reserved.
//                       UNPUBLISHED, LICENSED SOFTWARE.
//            CONFIDENTIAL AND PROPRIETARY INFORMATION WHICH IS THE
//          PROPERTY OF CALYPTO DESIGN SYSTEMS OR ITS LICENSORS
//  
//  Running on Windows XP student@FITKIT-PROJEKT Service Pack 3 5.01.2600 i686
//  
//  Package information: SIFLIBS v17.0_1.1, HLS_PKGS v17.0_1.1, 
//                       DesignPad v2.78_0.0
//  
//  This version may only be used for academic purposes.  Some optimizations 
//  are disabled, so results obtained from this version may be sub-optimal.
//  
project new
flow package require /SCVerify
solution file add {./src_genpix/genpix.cpp} -type C++
solution file add {./src_genpix/tb_genpix.cpp} -type C++ -exclude true
solution file add {./../cpu/cpu.c} -type C++ -exclude true
solution file add {./../cpu/cpu.h} -type CHEADER -exclude true
directive set -OPT_CONST_MULTS -1
directive set -CLOCK_OVERHEAD 40.000000
directive set -UNROLL no
directive set -MEM_MAP_THRESHOLD 32
directive set -REGISTER_THRESHOLD 256
directive set -MERGEABLE true
directive set -SPECULATE true
directive set -COMPGRADE fast
directive set -PIPELINE_RAMP_UP true
directive set -OLD_SCHED false
directive set -DESIGN_GOAL area
directive set -ASSIGN_OVERHEAD 0
directive set -RESET_CLEARS_ALL_REGS true
directive set -SAFE_FSM false
directive set -NO_X_ASSIGNMENTS true
directive set -REG_MAX_FANOUT 0
directive set -FSM_ENCODING none
directive set -START_FLAG {}
directive set -DONE_FLAG {}
directive set -IDLE_SIGNAL {}
directive set -REGISTER_IDLE_SIGNAL false
go analyze
directive set -TRANSACTION_DONE_SIGNAL true
directive set -CLOCK_NAME clk
directive set -CLOCKS {clk {-CLOCK_PERIOD 40.0 -CLOCK_EDGE rising -CLOCK_UNCERTAINTY 0.0 -CLOCK_HIGH_TIME 20.0 -RESET_SYNC_NAME rst -RESET_ASYNC_NAME arst_n -RESET_KIND sync -RESET_SYNC_ACTIVE high -RESET_ASYNC_ACTIVE low -ENABLE_NAME {} -ENABLE_ACTIVE high}}
directive set -CLOCK_NAME clk
directive set -TECHLIBS {{mgc_Xilinx-SPARTAN3-4_beh_psr.lib {{mgc_Xilinx-SPARTAN3-4_beh_psr part 3s50tq144}}}}
directive set -DESIGN_HIERARCHY genpix__FRbT1R31ac_int__tm__17_XCiL_1_3XCbL_1_0T1
go compile
directive set /genpix/core/main -PIPELINE_INIT_INTERVAL 1
go architect
go extract
