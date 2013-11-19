#include <fcntl.h>
 
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <string.h>

/*
 * Program to print information about a FAT file system.
 */

/*
 * Main function.
 */
typedef struct filesystem_info {
    int sectorSize;
    int clusterSize;
    int numRootEntries;
    int numSecFAT;
    int numReservedSec;
    int numHiddenSec;
    int firstTableSec;
    int firstRootSec;
    int firstClusterSec;
} filesystem_info;

char * appendFileNames(char * str1, char * str2){
	char * new_str;
	new_str = malloc(strlen(str1) + strlen(str2) + 5);
	new_str [0] = '\0';
	strcat(new_str, str1);
	strcat(new_str, "/");
	strcat(new_str, str2);
	return new_str;
}

int getFATEntry(int fp, int startingCluster, filesystem_info * info);
void setFATTable(filesystem_info * fs, char buffer[]);
int validateName( char buffer[], int size);
int validateName(char * buffer, int size){
		
		if(*buffer == (0x20)) {
			return 0;
		}
		int i;
    for (i = 0; i < size; i++) {
        char a = buffer[i];
        
        // check that file name is valid
        if (a == (char) 0x22 || a == (char) 0x2a || a == (char) 0x2b ||
                a == (char) 0x2c || a == (char) 0x2e || a == (char) 0x2f ||
                a == (char) 0x3a || a == (char) 0x3b || a == (char) 0x3c ||
                a == (char) 0x3d || a == (char) 0x3e || a == (char) 0x3f ||
                a == (char) 0x5b || a == (char) 0x5c || a == (char) 0x5d ||
                a == (char) 0x7c) {
            return 0;
        }
    }
		return 1;
	
}

void setFATTable(filesystem_info * fs, char buffer[]){

    fs->sectorSize = buffer[11] + buffer[12]*256;
    fs->clusterSize = buffer[13];
    fs->numReservedSec = buffer[14] + buffer[15]*256;
    fs->numRootEntries = buffer[17] + buffer[18]*256;
    fs->numSecFAT = buffer[22] + buffer[23]*256;
    fs->numHiddenSec = buffer[28] + buffer[29]*256;
    fs->firstTableSec = fs->numReservedSec;
    fs->firstRootSec = fs->firstTableSec + buffer[16]*fs->numSecFAT;
    fs->firstClusterSec = fs->firstRootSec +
        (fs->numRootEntries*32+fs->sectorSize/2)/fs->sectorSize - 2*fs->clusterSize;
}

int getFATEntry(int fp, int startingCluster, filesystem_info * info){
	int offset = info->firstTableSec * info->sectorSize + (startingCluster /2 * 3);

	unsigned char tableBuffer[3];
	block_read(fp, tableBuffer, 3, offset);
	int FATentries = (tableBuffer[2] << 16) + (tableBuffer[1] << 8) + tableBuffer[0];
	int entry = ((startingCluster % 2 == 0) ? FATentries % 4096 : FATentries / 4096);
	return entry;
}

void print_directory(int fd,  filesystem_info * info,char fileName[], int startingCluster, char * parent_path){
	printf("File Name: %s/%s [DIRECTORY]\n", parent_path, fileName);
	char * newName = appendFileNames(parent_path, fileName);

	printf("First Cluster: %d\n", startingCluster);
	printf("Size: 0\n");
	printf("All Clusters: ");
 
  // Check for all clusters in current file
	int entry = startingCluster;
	while (entry > 0 && entry < 4080) {
		printf(" %d ", entry);
		entry = getFATEntry(fd, entry, info);
	}
	printf("\n\n"); 


  int entries = 0;
  unsigned char entryBuffer[32];
  //int offset = info->firstClusterSec * info->sectorSize + startingCluster * info->sectorSize * info->clusterSize;

	entry = startingCluster;
	while(entry > 0 && entry < 4080){
  	int offset = info->firstClusterSec * info->sectorSize + entry * info->sectorSize * info->clusterSize;
		
  	while (entries < info->clusterSize * info->sectorSize / 32) {
				block_read(fd, entryBuffer, 32, offset);
  	    entries++;
  	    offset += 32;
  	    print_root_dir(fd, info, entryBuffer, newName);
  	}
		entry = getFATEntry(fd, entry, info);
	}
}

void print_file(int fd, filesystem_info* fs, char fileName[], char ext[], int size, int startingCluster, char * parent_path) {

    // Print file name and size
    printf("File name: %s/%s.%s [FILE]\n", parent_path, fileName, ext);
    printf("Size: %d\n", size);
		printf("First Cluster: %d\n", startingCluster);
		printf("All Clusters: ", startingCluster);
 
    // Check for all clusters in current file
		int entry = startingCluster;
		while (entry > 0 && entry < 4080) {
			printf(" %d ", entry);
			entry = getFATEntry(fd, entry, fs);
		}
		printf("\n\n"); 
}
 
void print_root_dir(int fd, filesystem_info* fs, unsigned char buffer[], char * parent_path) {
    char fileName[9];
    int i;

    for (i = 0; i < 8; i++) {
        char a = buffer[i];
        
        // check that file name is valid
        if (a == (char) 0x22 || a == (char) 0x2a || a == (char) 0x2b ||
                a == (char) 0x2c || a == (char) 0x2e || a == (char) 0x2f ||
                a == (char) 0x3a || a == (char) 0x3b || a == (char) 0x3c ||
                a == (char) 0x3d || a == (char) 0x3e || a == (char) 0x3f ||
                a == (char) 0x5b || a == (char) 0x5c || a == (char) 0x5d ||
                a == (char) 0x7c) {
            return;
        }
    
        fileName[i] = buffer[i];
    }
    fileName[i--] = '\0';
    // Remove filler spaces
    while (i >= 0 && fileName[i] == (char) 0x20) {
        fileName[i--] = '\0';
    }
    
    char ext[4];
    for (i = 0; i < 3; i++) {
        ext[i] = buffer[8+i];
    }
    ext[i--] = '\0';
    // Remove filler spaces
    while (i >= 0 && ext[i] == (char) 0x20) {
        ext[i--] = '\0';
    }
    
    int attr = buffer[11];
    int isDir = (attr >> 4) & 0x1;
    int startCluster = buffer[26] + buffer[27]*256;
    int size = buffer[28] + buffer[29]*256 + buffer[30]*(256^2) + buffer[31]*(256^3);
    
    // Do not print if it is a deleted file
    if (!isDir && (fileName[0] == (char) 0xe5 || size == 0)) {
        return;
    // Adjust directory name for delete character
    } else if (isDir && fileName[0] == (char) 0x05) {
        fileName[0] = 0xe5;
    }
    
    // Print depending on whether directory or file
    if (!isDir) {
        print_file(fd, fs, fileName, ext, size, startCluster, parent_path);
    } else {
        print_directory(fd, fs, fileName, startCluster, parent_path);
    }
    
}

int main(int argc, char *argv[])
{

    if (argc < 2) {
        printf("Error. Missing file name parameter.\n");
        return 1;
    }
    
    int fd = open(argv[1], O_RDONLY);
    if(!fd) {
		return 1;
		}

		char buffer[512];
		//read(fd, buffer, 512);
    
		block_read(fd, buffer, 512, 0);
    // Print boot disk information
    filesystem_info *fs = malloc(sizeof(filesystem_info));
    setFATTable(fs, buffer);

    printf("Sector size: %d\n", fs->sectorSize);
    printf("Cluster size in sectors: %d\n", fs->clusterSize);
    printf("Root directory size (nb of entries): %d\n", fs->numRootEntries);
    printf("Sectors per fat: %d\n", fs->numSecFAT);
    printf("Reserved sectors: %d\n", fs->numReservedSec);
    printf("Hidden sectors: %d\n", fs->numHiddenSec);
    printf("Fat offset in sectors: %d\n", fs->firstTableSec);
    printf("Root directory offset in sectors: %d\n", fs->firstRootSec); 
    printf("First cluster offset in sectors: %d\n\n", fs->firstClusterSec);
    
    int offset = fs->firstRootSec*fs->sectorSize;
    int entries = 0;
    unsigned char entryBuffer[32];
		char * rootPath = "";
    
    while (entries < fs->numRootEntries) {
        //lseek(fd, offset, SEEK_SET);        
        //read(fd, entryBuffer, 32);
				block_read(fd, entryBuffer, 32, offset);
        entries++;
        offset += 32;
        
        print_root_dir(fd, fs, entryBuffer, rootPath);
    }

    
    
    close(fd);
    return 0;
}
