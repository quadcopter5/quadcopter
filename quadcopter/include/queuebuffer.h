/*
	queuebuffer.h

	QueueBuffer data structure - stores data byte-per-byte in a FIFO structure.

	C-Style data structure; declare a pointer of type QueueBuffer and initialize
	it by passing a reference to the pointer to qb_initialize(). Make sure to
	call qb_free() on the QueueBuffer when the structure is no longer needed.
*/

#ifndef QUEUEBUFFER_H
#define QUEUEBUFFER_H

#define QB_BUFSIZE 4096
typedef struct _QueueBufferNode {
	char buffer[QB_BUFSIZE];
	char *front, *back;
	struct _QueueBufferNode *next;
} QueueBufferNode;

typedef struct _QueueBuffer {	
	QueueBufferNode *head, *tail;
} QueueBuffer;

/**
	Initialize a QueueBuffer.

	Provide an uninitialized struct QueueBuffer pointer, and this function
	will initialize it to point to a valid QueueBuffer.
*/
void qb_initialize(QueueBuffer **qbuf);

/**
	Free resources used by given QueueBuffer.
*/
void qb_free(QueueBuffer **qbuf);

/**
	Add len bytes from argument buffer to the provided QueueBuffer.
*/
void qb_push(QueueBuffer *qbuf, const void *buffer, size_t len);

/**
	Pop the next numbytes off the QueueBuffer into the given buffer.
	
	Returns the number of bytes actually popped. This could be less than
	numbytes if the number of bytes stored in the QueueBuffer is less than
	numbytes.
*/
int qb_pop(QueueBuffer *qbuf, void *buffer, size_t numbytes);

/**
	Returns the number of bytes currently stored by qbuf.
*/
int qb_getSize(QueueBuffer *qbuf);

#endif

