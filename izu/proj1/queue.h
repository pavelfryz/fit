/**
 * IZU projekt 2012  
 * 
 * File:     queue.h
 *
 * Author:   Horacek (ihoracek@fit.vutbr.cz) 
 *           Malacka (imalacka@fit.vutbr.cz)
 *           Samek (samejan@fit.vutbr.cz)
 *
 * Revision: $Id: queue.h 6 2012-03-18 19:58:49Z samek $
 */

#ifndef QUEUE_H
#define	QUEUE_H

typedef struct queueItem {
    int dataitem;
    struct queueItem *link;
} queueItem;

queueItem *addItem(queueItem *listpointer, int data);
queueItem *removeItem(queueItem *listpointer);
void printQueue(queueItem *listpointer);
void clearQueue(queueItem *listpointer);
void xfree(void *ptr);

#endif	/* QUEUE_H */
