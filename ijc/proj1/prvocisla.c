/*
 * Soubor:  prvocisla.c
 * Datum:   21.3.2011
 * Autor:   Pavel Frýz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1, pøíklad b)
 * Popis:   Eratosthenovo síto, makra pro práci s bitovým polem
 */

#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "error.h"

// Pocet bitu unsigned long
#define ULONG_BIT (sizeof(unsigned long)*CHAR_BIT)
// Velikost pole unsigned long s minimalne N bity
#define ARRAY_SIZE(N) ((N)/ULONG_BIT + (((N)%ULONG_BIT>0) ? 1:0))
// Makro pro vytvoreni bitoveho pole, jmeno_pole[0]=velikost
#define BitArray(jmeno_pole,velikost) unsigned long jmeno_pole[ARRAY_SIZE(velikost)+1]={velikost}

#ifdef USE_INLINE
/**
 * Nastaví bit v bitovém poli na urèené pozici.
 * @param jmeno_pole Jméno bitového pole
 * @param index Pozice bitu v poli
 * @param vyraz 0->0, !0->1
 */
static inline void SetBit(unsigned long *jmeno_pole,unsigned long index,char vyraz)
{
	if((index)>=jmeno_pole[0]) Error("Index %ld mimo rozsah 0..%ld\n", index, jmeno_pole[0]-1);
	if(vyraz)
	{
		jmeno_pole[(index)/ULONG_BIT+1] |= 1<<((index)%ULONG_BIT);
	}
	else
	{
		jmeno_pole[(index)/ULONG_BIT+1] &= ~(1<<((index)%ULONG_BIT));
	}
} 
/**
 * Vrací hodnotu bitu v bitovém poli na urèené pozici.
 * @param jmeno_pole Jméno bitového pole
 * @param index Pozice bitu v poli
 * @return Hodnota bitu
 */
static inline char GetBit(unsigned long *jmeno_pole,unsigned long index)
{
	if((index)>=jmeno_pole[0]) Error("Index %ld mimo rozsah 0..%ld\n", index, jmeno_pole[0]-1);
	return ((jmeno_pole[(index)/ULONG_BIT+1] & (1<<((index)%ULONG_BIT)))==0) ? 0:1;
}
#else
//Makro pro nastavení bitu v bitovém poli na urèené pozici.
#define SetBit(jmeno_pole,index,vyraz)\
		do{\
			if((index)>=jmeno_pole[0]) Error("Index %ld mimo rozsah 0..%ld\n", index, jmeno_pole[0]-1);\
			if(vyraz)\
			{\
				jmeno_pole[(index)/ULONG_BIT+1] |= 1<<((index)%ULONG_BIT);\
			}\
			else\
			{\
				jmeno_pole[(index)/ULONG_BIT+1] &= ~(1<<((index)%ULONG_BIT));\
			}\
        }while(0)
//Makro pro zji¹tìní hodnoty bitu na urèené pozici.
#define GetBit(jmeno_pole,index)\
		(((index)>=jmeno_pole[0])?\
		Error("Index %ld mimo rozsah 0..%ld\n", index, jmeno_pole[0]-1),-1\
		:((jmeno_pole[(index)/ULONG_BIT+1] & (1<<((index)%ULONG_BIT)))==0) ? 0:1)
#endif
   
#define SIZE 90000000L
#define FACTOR_COUNT 20
#define SQRTSIZE sqrt(SIZE)
     
int main(void)
{
	BitArray(array,SIZE);
	unsigned long i, factor[FACTOR_COUNT];
	int count;
	for(i=2;i<=SQRTSIZE;i++)
	{
		if(!GetBit(array,i))
			for(unsigned long n=i*i;n<array[0];n+=i) SetBit(array,n,1);
	}
	for(count=FACTOR_COUNT-1,i=array[0]-1;count>=0;i--)
		if(!GetBit(array,i))
		{
			factor[count]=i;
			count--;
		}
	for(count=0;count<FACTOR_COUNT;count++)
		printf("%ld\n",factor[count]);	 
}
