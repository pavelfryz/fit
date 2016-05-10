#include <ac_int.h>
#include "filter.h"
#include "addr_space.h"

#define NOP           0
#define INC_HIST      1
#define INC_HIST_LAST 2
#define MCU_ACT       3
#define READ_THRESH   4
#define CLEAN_HIST_0  5
#define CLEAN_HIST_1  6
#define CLEAN_HIST_2  7
#define CLEAN_HIST_3  8
#define CLEAN_HIST_4  9
#define CLEAN_HIST_5  10
#define CLEAN_HIST_6  11
#define CLEAN_HIST_7  12

/***************************************************************************
 Funkce median() vraci hodnotu medianu ze zadaneho okenka hodnot 3x3 pixelu.
 Jedna se o paralelni verzi algoritmu vhodnou pro hardware. 

 Vstupy:
   window   ukazatel na hodnoty okenka 3x3 pixelu
 Vystupu:
   pix_out  hodnota medianu
***************************************************************************/
t_pixel median(t_pixel *window){

   int i, j;
   t_pixel max[10], max2[10];

   Linit: for(i=0;i<9;i++)
      max[i] = window[i];
   max[9] = MIN_PIXEL;
   
   L2: for(j=0;j<5;j++) {

       L1a: for(i=0;i<5;i++) {
          if(max[i*2]<max[(i*2)+1]) {
              max2[i*2]     = max[i*2]; 
              max2[(i*2)+1] = max[(i*2)+1];
          }
          else {
              max2[i*2]     = max[(i*2)+1]; 
              max2[(i*2)+1] = max[i*2];
          }
       }

       L1b: for(i=0;i<4;i++) {
          if(max2[(i*2)+1]<max2[(i*2)+2]) {
              max[(i*2)+1] = max2[(i*2)+1]; 
              max[(i*2)+2] = max2[(i*2)+2];
          }
          else {
              max[(i*2)+1] = max2[(i*2)+2]; 
              max[(i*2)+2] = max2[(i*2)+1];
          }
       }     
       max[0] = max2[0];
       max[9] = max2[9];
   }

   return max[5];
}

/***************************************************************************
 Procedura buffer() uklada vstupni hodnoty pixelu do dvou-radkovou bufferu
 a vraci hodnoty odpovidajici aktualne zpracovavanemu sloupci 3 pixelu
 (posledni sloupec okenka 3x3).

 Vstupy:
   din   hodnota vstupniho pixelu
   c     cislo sloupce prave zpracovavaneho pixelu
 Vystupy:
   col_window  aktualne zpracovavany sloupec tri pixelu
***************************************************************************/
void buffer(t_pixel din, t_col c, t_pixel *col_window){

   static t_pixel    buf[2][FRAME_COLS];
   static t_bool     sel=1;
   t_pixel           t0, t1;

   if (c==0) sel = !sel;
   t1 = buf[1][c];
   t0 = buf[0][c];
   buf[sel][c] = din;

   col_window[0] = (sel==1) ? t1:t0;
   col_window[1] = (sel==1) ? t0:t1;
   col_window[2] = din;
}

/***************************************************************************
 Procedura clip_window() provadi clipping tj. doplnuje krajni hodnoty okenka
 3x3 na okrajich snimku, kde nejsou pixely k dispozici.

 Vstupy:
   r     aktualni cislo radku
   c     aktualni cislo sloupce
   iw    aktualni hodnoty posuvneho okenka 3x3
 Vystupy:
   ow    upravene hodnoty posuvneho okenka 3x3 po osetreni krajnich hodnot
***************************************************************************/
void clip_window(t_row r, t_col c, t_pixel *iw, t_pixel *ow) {
   
   t_quad first_row, last_row, first_col, last_col;
   t_quad test1, test2, test3, test4;

   first_row = (r == 0);
   first_col = (c == 0);
   last_row  = (r == FRAME_ROWS-1);
   last_col  = (c == FRAME_COLS-1);

   ow[4] = iw[4];
   ow[1] = first_col ? iw[4] : iw[1];
   ow[5] = last_row  ? iw[4] : iw[5];
   ow[7] = last_col  ? iw[4] : iw[7];
   ow[3] = first_row ? iw[4] : iw[3];

   test1 = first_row | (first_col << 1);
   switch(test1) {
      case 3:  ow[0] = iw[4]; break; /* first_row, first_col */
      case 1:  ow[0] = iw[1]; break; /* first_row, not first_col */
      case 2:  ow[0] = iw[3]; break; /* not first_row, first_col */
      default: ow[0] = iw[0]; break; /* not first_row, not first_col */
   }

   test2 = first_row | (last_col << 1);
   switch(test2) {
      case 3:  ow[6] = iw[4]; break; /* first_row, last_col */
      case 1:  ow[6] = iw[7]; break; /* first_row, not last_col */
      case 2:  ow[6] = iw[3]; break; /* not first_row, last_col */
      default: ow[6] = iw[6]; break; /* not first_row, not last_col */
   }

   test3 = last_row | (first_col << 1);
   switch(test3) {
      case 3:  ow[2] = iw[4]; break; /* last_row, first_col */
      case 1:  ow[2] = iw[1]; break; /* last_row, not first_col */
      case 2:  ow[2] = iw[5]; break; /* not last_row, first_col */
      default: ow[2] = iw[2]; break; /* not last_row, not first_col */
   }

   test4 = last_row | (last_col << 1);
   switch(test4) {
      case 3:  ow[8] = iw[4]; break; /* last_row, last_col */
      case 1:  ow[8] = iw[7]; break; /* last_row, not last_col */
      case 2:  ow[8] = iw[5]; break; /* not last_row, last_col */
      default: ow[8] = iw[8]; break; /* not last_row, not last_col */
   }
}

/***************************************************************************
 Procedura shift_window() provadi posun okenka 3x3 o jednu pozici do prava.

 Vstupy:
   window      puvodni hodnoty posuvneho okenka 3x3
   col_window  nove nasouvany sloupec  
 Vystupy:
   window      hodnoty vstupniho pole jsou aktualizovany
***************************************************************************/
void shift_window(t_pixel *window, t_pixel *col_window) {

   window[2] = window[5];
   window[1] = window[4];
   window[0] = window[3];

   window[5] = window[8];
   window[4] = window[7];
   window[3] = window[6];

   window[8] = col_window[2];
   window[7] = col_window[1];
   window[6] = col_window[0];
}

/***************************************************************************
 Funkce system_input() zajistuje zpracovani a bufferovani vstupnich pixelu.
 Vstupni pixel ulozi do radkoveho bufferu a provede posun a clipping posuvneho
 okenka. Funkce rozlisuje mezi vstupnim pixelem a skutecne filtrovanym
 pixelem. Filtrovany pixel je oproti vstupnimu posunut o jeden radek a jeden
 pixel.

 Vstupy:
   din            vstupni pixel
 Vystupy:
   cliped_window  posuvne okenko 3x3 po osetreni okrajovych bodu
   last_pixel     infomace o tom, zda se jedna o posledni pixel snimku
   navratova hodnota ukazu, zda je okenko platne ci nikoliv. Okenko nemusi
   byt platne napr. na zacatku zpracovani, kdy jeste neni v bufferu nasunut 
   dostatek novych pixelu
***************************************************************************/
bool system_input(t_pixel din, t_pixel *cliped_window, bool &last_pixel){

   static t_col      c = 0, c_filter = 0; 
   static t_row      r = 0, r_filter = 0;
   static bool       output_vld = false;
   static t_pixel    window[9];
   t_pixel           col_window[3];

   /* ulozeni pixelu do bufferu, posun okenka a clipping */
   buffer(din, c, col_window);
   shift_window(window, col_window);
   clip_window(r_filter, c_filter, window, cliped_window);

   /* od druheho radku a druheho sloupce je vystup platny */
   if ((r == 1) && (c == 1))
      output_vld = true;

   /* oznaceni posledniho filtrovaneho pixelu snimku */
   last_pixel = ((r_filter == FRAME_ROWS-1) && (c_filter == FRAME_COLS-1));

   /* aktualizace souradnic filtrovaneho pixelu */
   if ((c_filter == FRAME_COLS-1) && (output_vld)) {
      r_filter = (r_filter == FRAME_ROWS-1) ? 0 : r_filter+1;
   }
   c_filter = c;

   /* aktualizace souradnic vstupniho pixelu */
   if (c == FRAME_COLS-1) {
      c = 0;
      r = (r == FRAME_ROWS-1) ? 0 : r+1;
   }
   else  
      c++;

   return output_vld;
}

/***************************************************************************
 Procedura filter() zajistuje zpracovani vstupniho pixelu. Tato jednoducha
 verze provadi pouze kopirovani vstupniho pixelu na vystup.

 Vstupy:
   in_data        vstupni pixel
   in_data_vld    informace o tom, zda je vstupni pixel platny
   mcu_data       pamet pro vymenu dat s MCU

 Vystupy:
   out_data       vystupni pixel
   mcu_data       pamet pro vymenu dat s MCU
***************************************************************************/
#pragma hls_design top

void filter(t_pixel &in_data, bool &in_data_vld, t_pixel &out_data, 
         t_mcu_data mcu_data[MCU_SIZE]){
   static t_pixel threshold = 4;
   static t_frame frame = 1;
   static t_state state = NOP;
   static bool mcu_ready = false;
   t_pixel        pix_filtered, window[9];
   bool           last_pixel;
   bool           data_out_vld;
   
   if(!mcu_ready){
      if(mcu_data[FPGA_ADDR_READY] == 1)
         mcu_ready = true;
   }else if(in_data_vld){
      data_out_vld = system_input(in_data, window, last_pixel);
      if(data_out_vld) {
         /* Filtrace medianem, aktualizace histogramu, prahovani */
         pix_filtered = median(window);
         if((frame % 10) == 0)
            state = INC_HIST;
           
         out_data = (pix_filtered >= threshold) ? MAX_PIXEL : MIN_PIXEL;

         /* Test na posledni pixel predchoziho snimku */
         if (last_pixel) {
            if((frame % 10) == 0) {
               state = INC_HIST_LAST;
            } else if ((frame % 10) == 1) {
               state = READ_THRESH;
               frame = 1;
            }
            frame++;
         }
         
         switch(state){
            case INC_HIST:      mcu_data[FPGA_ADDR_HIST+pix_filtered]++;    state = NOP;          break; 
            case INC_HIST_LAST: mcu_data[FPGA_ADDR_HIST+pix_filtered]++;    state = MCU_ACT;      break; 
            case MCU_ACT:       mcu_data[FPGA_ADDR_CONTROL] = 1;            state = NOP;          break;
            case READ_THRESH:   threshold = mcu_data[FPGA_ADDR_THRESH];     state = CLEAN_HIST_0; break;
            case CLEAN_HIST_0:  mcu_data[FPGA_ADDR_HIST+0] = 0;             state = CLEAN_HIST_1; break;
            case CLEAN_HIST_1:  mcu_data[FPGA_ADDR_HIST+1] = 0;             state = CLEAN_HIST_2; break;
            case CLEAN_HIST_2:  mcu_data[FPGA_ADDR_HIST+2] = 0;             state = CLEAN_HIST_3; break;
            case CLEAN_HIST_3:  mcu_data[FPGA_ADDR_HIST+3] = 0;             state = CLEAN_HIST_4; break;
            case CLEAN_HIST_4:  mcu_data[FPGA_ADDR_HIST+4] = 0;             state = CLEAN_HIST_5; break;
            case CLEAN_HIST_5:  mcu_data[FPGA_ADDR_HIST+5] = 0;             state = CLEAN_HIST_6; break;
            case CLEAN_HIST_6:  mcu_data[FPGA_ADDR_HIST+6] = 0;             state = CLEAN_HIST_7; break;
            case CLEAN_HIST_7:  mcu_data[FPGA_ADDR_HIST+7] = 0;             state = NOP;          break;
            default: break;
         }
      }
   }
}

