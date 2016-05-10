#include "filter.h"
#include "addr_space.h"
#include "../cpu/common.h"

#include <iostream>
using namespace std;
#include "mc_scverify.h"
#include "../cpu/cpu.h"

/***************************************************************************
 Funkce otsu() vypocte hodnotu prahu na zaklade histogramu pixelu snimku.

 Vstupy:
    hist    ukazatel na histogram
    n       pocet polozek histogramu
 Vystupy:
    hodnota vypocteneho prahu
***************************************************************************/
int otsu(unsigned long *hist, int n) {

   unsigned long total = 0;
   float sum = 0;
   float sumB = 0, varMax = 0, varBetween;
   unsigned long wB = 0,  wF = 0;
   int threshold = 0;
   float mB, mF;  
   int   t;

   for (t=0 ; t<n ; t++) {
      sum += t * hist[t];
      total += hist[t];
   }

   for (t=0 ; t<n; t++) {

      wB += hist[t];             /* Weight Background */
      if (wB == 0) continue;

      wF = total - wB;           /* Weight Foreground */
      if (wF == 0) break;

      sumB += (float) (t * hist[t]);

      mB = sumB / wB;            /* Mean Background */
      mF = (sum - sumB) / wF;    /* Mean Foreground */

      /* Calculate Between Class Variance */
      varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);

      /* Check if new maximum found */
      if (varBetween > varMax) {
         varMax = varBetween;
         threshold = t+1;
      }
   }

   return threshold;
}

/***************************************************************************
 Hlavni program Testbench souboru. Zajistuje generovani vstupnich vektoru,
 volani puvodniho (referencniho) kodu a modifikovaneho kodu a porovnani
 jejich vystupu.
***************************************************************************/
CCS_MAIN(int argv, char *argc){
   t_pixel pix_in_hw, pix_out_hw = 0;
   t_pixel_sw pix_in_sw, pix_out_sw = 0;
   int pix_out_sw_vld;
   bool valid = true, non_valid = false;
   t_mcu_data mcu_data[MCU_SIZE];
   short i;
   unsigned long histogram[PIXELS];
   
   for(int i=0;i<MCU_SIZE;i++)
      mcu_data[i] = 0;

   /* Poslani zpravy FPGA, ze MCU je ready */
   mcu_data[FPGA_ADDR_READY] = 1;

   for(int f=0;f<FRAMES;f++){
      for(int r=0;r<FRAME_ROWS;r++){
         if(r==10) { /* MCU provest jednou za snimek, nutne pockat par pixelu nez FPGA posle data */ 
            if(mcu_data[FPGA_ADDR_CONTROL] == 1){
               for(i=0; i<PIXELS; i++){
                  histogram[i]=mcu_data[FPGA_ADDR_HIST+i];
               }
         
               mcu_data[FPGA_ADDR_THRESH] = otsu(histogram, PIXELS);
               mcu_data[FPGA_ADDR_CONTROL] = 0;
            }
         }
         for(int c=0;c<FRAME_COLS;c++){
            /* Generovani vstupniho pixelu */
            pix_in_sw = gen_pixel();
            pix_in_hw = (t_pixel)pix_in_sw;
            /* Volani puvodniho (referencniho) kod */
            pixel_processing(pix_in_sw, &pix_out_sw, &pix_out_sw_vld); 
            /* Volani modifikovaneho kodu */
            CCS_DESIGN(filter)(pix_in_hw, valid, pix_out_hw, mcu_data); 

            /* Porovnani vysledku referencniho vs. modifikovaneho kodu */
            if(pix_out_sw_vld && (f>=100) && (pix_out_hw != pix_out_sw)) {
               cout << endl << "ERROR: Chyba na pozici: (" << (f+1) << ":" << (r+1) << "," << (c+1) << "), ";
               cout << "In: out_sw/out_hw = " << pix_in_hw << ": " << (int)pix_out_sw << "/" << pix_out_hw <<  endl;
               CCS_RETURN(1);
            }
         }
      }
   }
   cout << "INFO: ***********************************************************" << endl;
   cout << "INFO: Test referencniho kodu oproti upravenemu probehl v poradku." << endl;
   cout << "INFO: ***********************************************************" << endl;
   CCS_RETURN(0);
}
 
