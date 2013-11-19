/*
 * blockio.h
 *
 * Low-level functions used to perform  i/o operations on blocks. All of them
 * take as parameter an object  that contains information about the file that
 * is being accessed, the size of a sector, etc.
 */

#ifndef BLOCKIO_H
#define BLOCKIO_H

void block_read( int fd, char * buffer, int size, int position);

/* Add Stuff in here */



#endif
