#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

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

// Print information for files
void print_file(int fd, filesystem_info* fs, char fileName[], char ext[], int size, int startCluster) {

    // Print file name and size
    printf("File name: \\%s.%s\n", fileName, ext);
    printf("Size: %d\n", size);
 
    // Check for all clusters in current file
    int tableOffset = fs->firstTableSec * fs->sectorSize + (startCluster / 2 * 3);
    unsigned char tableBuffer[3];
    lseek (fd, tableOffset, SEEK_SET);
    read(fd, tableBuffer, 3);

    int tableEntries = (tableBuffer[2] << 16) + (tableBuffer[1] << 8) + tableBuffer[0];
    int entry = ((startCluster % 2 == 0) ? tableEntries % 4096 : tableEntries / 4096);
    printf("Clusters: %d", startCluster);
    
    // For all clusters in use
    while (entry > 0 && entry < 4080) {
        printf(", %d", entry);
        tableOffset = fs->firstTableSec * fs->sectorSize + (entry / 2 * 3);
        lseek (fd, tableOffset, SEEK_SET);
        read(fd, tableBuffer, 3);
    
        tableEntries = (tableBuffer[2] << 16) + (tableBuffer[1] << 8) + tableBuffer[0];
        entry = ((entry % 2 == 0) ? tableEntries % 4096 : tableEntries / 4096);
    }
    
    printf("\n\n");    
}

// Print information for directorys
void print_dir(int fd, filesystem_info* fs, char fileName[], int startCluster) {
    // Print file name and clusters for directory
    printf("File name: \\%s\n", fileName);
    printf("This file is a directory.\n");
    printf("Clusters: %d", startCluster);
    
    char* clusters = (char*) startCluster;
    
    int tableOffset = fs->firstTableSec * fs->sectorSize + (startCluster / 2 * 3);
    unsigned char tableBuffer[3];
    lseek (fd, tableOffset, SEEK_SET);
    read(fd, tableBuffer, 3);

    int tableEntries = (tableBuffer[2] << 16) + (tableBuffer[1] << 8) + tableBuffer[0];
    int entry = ((startCluster % 2 == 0) ? tableEntries % 4096 : tableEntries / 4096);
    
    // Check if cluster is for file within directory or for directory itself
    int fileOffset = fs->firstClusterSec*fs->sectorSize + startCluster*fs->sectorSize*fs->clusterSize;
    unsigned char entryBuffer[32];
    lseek(fd, fileOffset, SEEK_SET);        
    read(fd, entryBuffer, 32);
    
    int i;
    int sameDir = 1;
    for (i = 0; i < 8; i++) {
        if (fileName[i] != '\0' && fileName[i] != entryBuffer[i]) {
            sameDir = 0;
            break;
        }
    }
    
    // Print all clusters used by directory
    while (entry > 0 && entry < 4080 && sameDir) {
        printf(", %d", entry);
        strcat(clusters, (char*) entry);
        
        tableOffset = fs->firstTableSec * fs->sectorSize + (entry / 2 * 3);
        lseek (fd, tableOffset, SEEK_SET);
        read(fd, tableBuffer, 3);
    
        tableEntries = (tableBuffer[2] << 16) + (tableBuffer[1] << 8) + tableBuffer[0];
        entry = ((entry % 2 == 0) ? tableEntries % 4096 : tableEntries / 4096);

        fileOffset = fs->firstClusterSec*fs->sectorSize + entry*fs->sectorSize*fs->clusterSize;
        lseek(fd, fileOffset, SEEK_SET);        
        read(fd, entryBuffer, 32);
        
        sameDir = 1;
        for (i = 0; i < 8; i++) {
            if (fileName[i] != '\0' && fileName[i] != entryBuffer[i]) {
                sameDir = 0;
                break;
            }
        }
    }
    
    printf("%d\n\n",entry);
    
    // Print file name and clusters for current directory . (exact same as above)
    printf("File name: \\%s\\.\n", fileName);
    printf("This file is a directory.\n");
    printf("Clusters: %d", startCluster);
   
    lseek (fd, tableOffset, SEEK_SET);
    read(fd, tableBuffer, 3);

    tableEntries = (tableBuffer[2] << 16) + (tableBuffer[1] << 8) + tableBuffer[0];
    entry = ((startCluster % 2 == 0) ? tableEntries % 4096 : tableEntries / 4096);
    
    fileOffset = fs->firstClusterSec*fs->sectorSize + startCluster*fs->sectorSize*fs->clusterSize;
    lseek(fd, fileOffset, SEEK_SET);        
    read(fd, entryBuffer, 32);
    
    sameDir = 1;
    for (i = 0; i < 8; i++) {
        if (fileName[i] != '\0' && fileName[i] != entryBuffer[i]) {
            sameDir = 0;
            break;
        }
    }
    
    while (entry > 0 && entry < 4080 && sameDir) {
        printf(", %d", entry);
        
        tableOffset = fs->firstTableSec * fs->sectorSize + (entry / 2 * 3);
        lseek (fd, tableOffset, SEEK_SET);
        read(fd, tableBuffer, 3);
    
        tableEntries = (tableBuffer[2] << 16) + (tableBuffer[1] << 8) + tableBuffer[0];
        entry = ((entry % 2 == 0) ? tableEntries % 4096 : tableEntries / 4096);

        fileOffset = fs->firstClusterSec*fs->sectorSize + entry*fs->sectorSize*fs->clusterSize;
        lseek(fd, fileOffset, SEEK_SET);        
        read(fd, entryBuffer, 32);
        
        sameDir = 1;
        for (i = 0; i < 8; i++) {
            if (fileName[i] != '\0' && fileName[i] != entryBuffer[i]) {
                sameDir = 0;
                break;
            }
        }
    }
    
    printf("\n\n");
    
    // Print file name and clusters for parent directory ..
    printf("File name: \\%s\\..\n", fileName);
    printf("This file is a directory.\n");
    printf("Clusters: %d\n\n", 0);

}

// Print information for files and directories in root directory
void print_root_dir(int fd, filesystem_info* fs, unsigned char buffer[]) {
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
        print_file(fd, fs, fileName, ext, size, startCluster);
    } else {
        print_dir(fd, fs, fileName, startCluster);
    }
    

}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error. Missing file name parameter.\n");
        return 1;
    }
    
    int fd = open(argv[1], O_RDONLY);
    if(!fd) {
		fprintf(stderr, "Error: %s. \n", strerror(errno));
		return 1;
	}

	char buffer[512];
	read(fd, buffer, 512);
    
    // Print boot disk information
    struct filesystem_info *fs = malloc(sizeof *fs);
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
    
    while (entries < fs->numRootEntries) {
        lseek(fd, offset, SEEK_SET);        
        read(fd, entryBuffer, 32);
        entries++;
        offset += 32;
        
        print_root_dir(fd, fs, entryBuffer);
    }
    
    
    close(fd);
    return 0;
}


