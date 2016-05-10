/**
 * IZU projekt 2012 
 * 
 * File:     queue.c
 *
 * Author:   Horacek (ihoracek@fit.vutbr.cz) 
 *           Malacka (imalacka@fit.vutbr.cz)
 *           Samek (samejan@fit.vutbr.cz)
 *
 * Revision: $Id: queue.c 6 2012-03-18 19:58:49Z samek $
 */
 
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

queueItem *addItem(queueItem *listpointer, int data) {

    queueItem *lp = listpointer;

    if (listpointer != NULL) {

        while (listpointer->link != NULL)
            listpointer = listpointer->link;

        listpointer->link = (struct queueItem *) malloc(sizeof (queueItem));
        listpointer = listpointer -> link;
        listpointer->link = NULL;
        listpointer->dataitem = data;
        return lp;
    } else {
        listpointer = (struct queueItem *) malloc(sizeof (queueItem));
        listpointer->link = NULL;
        listpointer->dataitem = data;
        return listpointer;
    }
}

queueItem *removeItem(queueItem *listpointer) {
    queueItem *tempp;
    tempp = listpointer->link;
    xfree(listpointer);
    return tempp;
}

void printQueue(queueItem *listpointer) {
    if (listpointer == NULL) {
        printf("Queue is empty!\n");
    } else {
        while (listpointer != NULL) {
            printf("%d", listpointer->dataitem);
            listpointer = listpointer->link;
            if ( listpointer != NULL ) {
                printf(",");
            }
        }
        printf("\n");
    }
}

void clearQueue(queueItem *listpointer) {
    while (listpointer != NULL) {
        listpointer = removeItem(listpointer);
    }

}

void xfree(void *ptr) {
    if (ptr != NULL) {
        free(ptr);
    }
    ptr = NULL;
}
