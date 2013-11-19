#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


typedef struct filesystem_info {
	int sectorSize;
	int clusterSize;
	int numRootEntries;
	int sectorsPerFAT;
	int reservedSectors;
	int hiddenSectors;
	int firstSectorNumTable;
	int firstSectorNumRoot;
	int firstSectorNumCluster;
} filesystem_info;

int getFATEntry(int fp, int startingCluster, filesystem_info * info);
void print_file(int fp, char fileName[], char ext[], int size, int startingCluster, filesystem_info * info);
void print_directory(int fp, char fileName[], int startingCluster, filesystem_info * info);
void printRootDirectoryEntry(int fp, filesystem_info * info, unsigned char  entryBuffer[]);


void printRootDirectoryEntry(int fp, filesystem_info * info, unsigned char buffer[]){
	char fileName[9];
	int i;
	for(i = 0; i < 8; i++){
		char a = buffer[i];
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
	while(i>= 0 && fileName[i] == (char)0x20){
		fileName[i--] = '\0';
	}

	char ext[4];


	for(i = 0; i < 3; i++){
		ext[i] = buffer[8 + i];
	}
	ext[3] = '\0';

	int isDirectory = (buffer[11] >> 4) & 0x1;
	int startingCluster = buffer[26] + buffer[27] * 256;
	int size = buffer[28] + buffer[29]*256 + buffer[30] * (256^2) + buffer[31] * (256^3);

	if(isDirectory){

		print_directory(fp, fileName, startingCluster, info);

	}else{

		print_file(fp, fileName, ext, size, startingCluster, info);
	
	}

}

void print_directory(int fp, char fileName[], int startingCluster, filesystem_info * info){
	printf("File Name: %s\n", fileName);
	printf("File is a directory\n");

	int entry = getFATEntry(fp, startingCluster, info);
	printf("First Cluster: %d\n", entry);
	printf("Size: 0");

}

void print_file(int fp, char fileName[], char ext[], int size, int startingCluster, filesystem_info * info){
	printf("File Name: %s.%s\n", fileName, ext);
	printf("File is not a directory\n");

	int entry = getFATEntry(fp, startingCluster, info);
	printf("First Cluster: %d\n", entry);
	printf("Size: %d\n", size);

}

int getFATEntry(int fp, int startingCluster, filesystem_info * info){
	int offset = info->firstSectorNumTable * info->sectorSize + (startingCluster /2 * 3);

	unsigned char tableBuffer[3];
	lseek(fp, offset, SEEK_SET);
	read(fp, tableBuffer, 3);
	int FATentries = (tableBuffer[2] << 16) + (tableBuffer[1] << 8) + tableBuffer[0];
	int entry;
	if(startingCluster % 2){
		entry = FATentries & 0x0FFF;
	}else {
		entry = (FATentries >> 12) & 0x0FFF;
	}
	return entry;
}

int main(int argc, char *argv[]) {
    //if (argc < 2) {
    //    printf("Error. Missing file name parameter.\n");
    //    return 1;
    //}
    
    //int fd = open(argv[1], O_RDONLY);
    int fd = open("fat_volume.dat", O_RDONLY);

	unsigned char buffer[512];
	read(fd, buffer, 512);
  int i;
	for(i=0;i<512;i++){
		printf("%x", buffer[i]);
	}
   	//filesystem_info *fs_info = malloc(sizeof( filesystem_info));

    //fs_info->sectorSize = buffer[11] + buffer[12]*256;
    //fs_info->clusterSize = buffer[13];
    //fs_info->reservedSectors = buffer[14] + buffer[15]*256;
    //fs_info->numRootEntries = buffer[17] + buffer[18]*256;
    //fs_info->sectorsPerFAT = buffer[22] + buffer[23]*256;
    //fs_info->hiddenSectors = buffer[28] + buffer[29]*256;
    //fs_info->firstSectorNumTable = fs_info->reservedSectors;
    //fs_info->firstSectorNumRoot = fs_info->firstSectorNumTable + buffer[16]*fs_info->sectorsPerFAT;
    //fs_info->firstSectorNumCluster = fs_info->firstSectorNumRoot +
    //    (fs_info->numRootEntries*32+fs_info->sectorSize/2)/fs_info->sectorSize - 2*fs_info->clusterSize;
    //
    //printf("Sector size: %d\n", fs_info->sectorSize);
    //printf("Cluster size in sectors: %d\n", fs_info->clusterSize);
    //printf("Root directory size (nb of entries): %d\n", fs_info->numRootEntries);
    //printf("Sectors per fat: %d\n", fs_info->sectorsPerFAT);
    //printf("Reserved sectors: %d\n", fs_info->reservedSectors);
    //printf("Hidden sectors: %d\n", fs_info->hiddenSectors);
    //printf("Fat offset in sectors: %d\n", fs_info->firstSectorNumTable);
    //printf("Root directory offset in sectors: %d\n", fs_info->firstSectorNumRoot); 
    //printf("First cluster offset in sectors: %d\n\n", fs_info->firstSectorNumCluster);
    //
    //int offset = fs_info->firstSectorNumRoot*fs_info->sectorSize;
    //int entries = 0;
    //unsigned char entryBuffer[32];
    //
    //while (entries < fs_info->numRootEntries) {
    //    lseek(fd, offset, SEEK_SET);        
    //    read(fd, entryBuffer, 32);
    //    entries++;
    //    offset += 32;
    //    
    //    //printRootDirectoryEntry(fd, fs_info, entryBuffer);
    //}
    //
    //close(fd);
    return 0;
}

//int main(){
//	FILE * fp;
//	filesystem_info info;
//
//	fp = fopen("fat_volume.dat", "r");
//	if(!fp){
//		printf("Fuck me");
//		return 1;
//	}
//
//	unsigned char buffer[512];
//	fread(buffer, 512, 1, fp);
//	//read(fp, buffer, 512);
//	int i;
//	for(i = 0; i < 512; i++){
//		//printf("%x", buffer[i]);
//	}
//	info.sectorSize = buffer[11] + buffer[12] * 256;
//	info.clusterSize = buffer[13] * 256;
//	info.reservedSectors = buffer[14] + buffer[15]*256;
//	info.numRootEntries = buffer[17] + buffer[18]*256;
//	info.sectorsPerFAT = buffer[22] + buffer[23]*256;
//	info.hiddenSectors = buffer[28] + buffer[29]*256;
//	info.firstSectorNumTable = info.reservedSectors;
//	info.firstSectorNumRoot = info.firstSectorNumTable + buffer[16] * info.sectorsPerFAT;
//	info.firstSectorNumCluster = info.firstSectorNumRoot + 
//		(info.numRootEntries * 32/info.sectorSize);
//
//	printf("Sector size: %d\n", info.sectorSize);
//	printf("Cluster size in sectors: %d\n", info.clusterSize);
//	printf("Root directory size (nb of entries): %d\n", info.numRootEntries);
//	printf("Sectors per fat: %d\n", info.sectorsPerFAT);
//	printf("Reserved sectors: %d\n", info.reservedSectors);
//	printf("Hidden sectors: %d\n", info.hiddenSectors);
//	printf("Sector Numbre of first copy of FAT: %d\n", info.hiddenSectors);
//	printf("Fat offs_infoet in sectors: %d\n", info.firstSectorNumTable);
//	printf("Root directory offs_infoet in sectors: %d\n", info.firstSectorNumRoot); 
//	printf("First cluster offs_infoet in sectors: %d\n\n", info.firstSectorNumCluster);
//
//	int offs_infoet = info.firstSectorNumRoot * info.sectorSize;
//	int entry = 0;
//	unsigned char entryBuffer[32];
//
//	fs_infoeek(fp, offs_infoet, SEEK_SET);
//	int status = fread(entryBuffer, 32, 1, fp);
//	int x;
//
//	for(x=0; x< 32; x++){
//		printf("%x\n", entryBuffer[x]);
//	}
//	
//
//	//while(entry < info.numRootEntries){
//	//	printf("%d\n", offs_infoet);
//	//	fs_infoeek(fp, offs_infoet, SEEK_SET);
//	//	int status = fread(entryBuffer, 32, 1, fp);
//	//	int x;
//
//	//	for(x=0; x< 32; x++){
//	//		//printf("%x\n", entryBuffer[x]);
//	//	}
//	//	entry++;
//	//	offs_infoet += 32;
//
//	//	//printRootDirectoryEntry(fp, info, entryBuffer);
//	//}
//
//	close(fp);
//	return 0;
//}
