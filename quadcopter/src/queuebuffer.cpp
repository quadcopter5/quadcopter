/*
	queuebuffer.c

	QueueBuffer data structure - stores data byte-per-byte in a FIFO structure.
*/

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "queuebuffer.h"

void qb_initialize(QueueBuffer **qbuf) {
	if (!qbuf)
		return;

	*qbuf = (QueueBuffer *)malloc(sizeof(QueueBuffer));

	(*qbuf)->head = (QueueBufferNode *)malloc(sizeof(QueueBufferNode));
	(*qbuf)->tail = (*qbuf)->head;

	(*qbuf)->head->front =
	(*qbuf)->head->back  = (*qbuf)->head->buffer;
	(*qbuf)->head->next  = NULL;
}

void qb_free(QueueBuffer **qbuf) {
	if (!qbuf || !*qbuf)
		return;

	QueueBufferNode *current = (*qbuf)->head, *temp;
	while (current != NULL) {
		temp = current->next;
		free(current);
		current = temp;
	}

	free(*qbuf);
	*qbuf = NULL;
}

void qb_push(QueueBuffer *qbuf, const void *buffer, size_t len) {
	if (!qbuf)
		return;

	const char *cbuffer = (const char*)buffer;
	int i = 0,
	    delta;

	while (i < len) {
		// If the current node is full, add a new node to the back
		if (qbuf->tail->back - qbuf->tail->buffer >= QB_BUFSIZE) {
			qbuf->tail->next =
					(QueueBufferNode *)malloc(sizeof(QueueBufferNode));
			qbuf->tail = qbuf->tail->next;

			qbuf->tail->front = 
			qbuf->tail->back  = qbuf->tail->buffer;
			qbuf->tail->next  = NULL;
		}

		// If not enough space in this block, only fill to end of the block
		if (len - i > QB_BUFSIZE - (qbuf->tail->back - qbuf->tail->buffer))
			delta = QB_BUFSIZE - (qbuf->tail->back - qbuf->tail->buffer);

		// Otherwise, copy the remainder of the buffer into this block
		else
			delta = len - i;

		memcpy(qbuf->tail->back, cbuffer + i, delta);
		qbuf->tail->back += delta;
		i += delta;

		// Byte-by-byte... probably not as efficient
//		*qbuf->tail->back = cbuffer[i];
//		++qbuf->tail->back;
//		++i;
	}
}

int qb_pop(QueueBuffer *qbuf, void *buffer, size_t numbytes) {
	if (!qbuf)
		return 0;

	QueueBufferNode *current = qbuf->head;
	char *cbuffer = (char *)buffer;
	int i = 0,
	    delta;

	while (i < numbytes) {
		// Check if the head node is empty
		if (qbuf->head->front == qbuf->head->back) {
			if (qbuf->head->next == NULL) {
				// There is no more data; buffer is now empty
				return i;
			} else {
				// Move onto the next node, freeing the old head
				QueueBufferNode *temp = qbuf->head->next;
				free(qbuf->head);
				qbuf->head = temp;
			}
		}

		delta = qbuf->head->back - qbuf->head->front;
		if (numbytes - i < delta)
			delta = numbytes - i;

		memcpy(cbuffer + i, qbuf->head->front, delta);
		qbuf->head->front += delta;
		i += delta;

		// Byte-by-byte... probably not as efficient as memcpy
//		cbuffer[i] = *qbuf->head->front;
//		++qbuf->head->front;
//		++i;
	}

	return i;
}

int qb_getSize(QueueBuffer *qbuf) {
	if (!qbuf)
		return 0;

	int size = 0;
	QueueBufferNode *current = qbuf->head;
	while (current != NULL) {
		size += current->back - current->front;
		current = current->next;
	}

	return size;
}

