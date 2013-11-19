#include <fcntl.h>
 
 #include <sys/types.h>
 #include <sys/stat.h>

/*
 * blockio.c
 *
 * Low-level functions used to perform  i/o operations on blocks. All of them
 * take as parameter an object  that contains information about the file that
 * is being accessed, the size of a sector, etc.
 */


#include "blockio.h"
void block_read( int fd, char * buffer, int size, int position){
	lseek(fd, position, SEEK_SET);
	read(fd, buffer, size);
}

