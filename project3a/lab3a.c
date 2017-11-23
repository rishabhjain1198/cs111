#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include "ext2_fs.h"
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>

int indirectionTableSize, fileSystem;
__u32 bsize;

int mainBlockOffset = 0;

int bitMasker(int8_t mainByte, int nthBit)	{
	return (mainByte & (1<<nthBit)  );
}

void processBlock(int inode,unsigned int parentBlock, int recursionLevel, int realRecursionLevel);

int main(int argc, char** argv)	{

if(argc < 2)	{
	fprintf(stderr, "please input image file name!\n");
	exit(1);
}

char *fileName = argv[1];

//fprintf(stderr, "%s\n",fileName);

//READ THE SUPERBLOCK
	
fileSystem = open(fileName, O_RDONLY);
if(fileSystem < 0)	{
	fprintf(stderr, "Erorr in opening image file!\n");
	exit(1);
}

void* superBlockBuffer = malloc(sizeof(struct ext2_super_block));

if(pread(fileSystem, superBlockBuffer, sizeof(struct ext2_super_block), 1024) < (unsigned) sizeof(struct ext2_super_block))	{
	fprintf(stderr, "Error in reading super block!\n");
	exit(1);
}

struct ext2_super_block *superBlock = (struct ext2_super_block *) superBlockBuffer;

//fprintf(stderr, "%d\n", superBlock -> s_first_data_block);

//PRINTING CONTENTS OF SUPERBLOCK

bsize = EXT2_MIN_BLOCK_SIZE << superBlock -> s_log_block_size;

fprintf(stdout, "%s", "SUPERBLOCK");
fprintf(stdout, ",%u", superBlock -> s_blocks_count);
fprintf(stdout, ",%u", superBlock -> s_inodes_count);
fprintf(stdout, ",%u", EXT2_MIN_BLOCK_SIZE << superBlock -> s_log_block_size);
fprintf(stdout, ",%u", superBlock -> s_inode_size);
fprintf(stdout, ",%u", superBlock -> s_blocks_per_group);
fprintf(stdout, ",%u", superBlock -> s_inodes_per_group);
fprintf(stdout, ",%u\n", superBlock -> s_first_ino);

//DONE PRINTING SUPERBLOCK

//STARTING GROUP SUMMARY

void *groupTableBuffer = malloc(sizeof(struct ext2_group_desc));

if(pread(fileSystem, groupTableBuffer, sizeof(struct ext2_group_desc), bsize*2) < (unsigned) sizeof(struct ext2_group_desc))	{
	fprintf(stderr, "Error in reading group descriptor table!\n");
	exit(1);
}

struct ext2_group_desc *groupTable = (struct ext2_group_desc *) groupTableBuffer;

fprintf(stdout, "%s", "GROUP");
fprintf(stdout, ",%u", 0);
fprintf(stdout, ",%u", superBlock -> s_blocks_count);
fprintf(stdout, ",%u", superBlock -> s_inodes_count);
fprintf(stdout, ",%u", groupTable -> bg_free_blocks_count);
fprintf(stdout, ",%u", groupTable -> bg_free_inodes_count);
fprintf(stdout, ",%u", groupTable -> bg_block_bitmap);
fprintf(stdout, ",%u", groupTable -> bg_inode_bitmap);
fprintf(stdout, ",%u\n", groupTable -> bg_inode_table);

//FINISHED GROUP SUMMARY

//STARTING READING BITMAP FOR BFREE

__u32 blockBitmapOffset = bsize * (groupTable -> bg_block_bitmap);
__u32 inodeBitmapOffset = bsize * (groupTable -> bg_inode_bitmap);


void *blockBitmapBuffer = malloc(bsize);
if(pread(fileSystem, blockBitmapBuffer, bsize, blockBitmapOffset) < (unsigned) bsize)	{
	fprintf(stderr, "Error in reading block bitmap!\n");
	exit(1);
}

int8_t *blockBitmap = (int8_t *) blockBitmapBuffer;


void *inodeBitmapBuffer = malloc(bsize);
if(pread(fileSystem, inodeBitmapBuffer, bsize, inodeBitmapOffset) < (unsigned) bsize)	{
	fprintf(stderr, "Error in reading inode bitmap!\n");
	exit(1);
}
int8_t *inodeBitmap = (int8_t *) inodeBitmapBuffer;

//REMEMBER TO USE BITMASKS TO PARSE BITMAPS
//ONLY CARE ABOUT FREE FILES, IE. ONLY CARE ABOUT ZEROS



//BFREE, 40 iS BEING OUTPUTTED, and BFREE, 60 isnt. FIX IT

//DONT KNOW WHY, BUT JUST INCREMENT LOOP COUNTER BY 1


int blockBitmapCounter = 0, j = 0;

while ( (unsigned) blockBitmapCounter < superBlock -> s_blocks_per_group)	{

		int busyBit = bitMasker(blockBitmap[blockBitmapCounter/8], j);
		if(busyBit == 0)	{
			//TIME TO FIND BLOCK NUMBER
			fprintf(stdout, "BFREE,");
			fprintf(stdout, "%d\n", blockBitmapCounter+1);
		}

		j = (j+1)%8;
		blockBitmapCounter++;

}

//INODE BITMAP SUMMARY TIME

int inodeBitmapCounter = 0;	j = 0;

while( (unsigned) inodeBitmapCounter < superBlock -> s_inodes_per_group)	{
		int busyBit = bitMasker(inodeBitmap[inodeBitmapCounter/8], j);
		
		if(busyBit == 0)	{
			fprintf(stdout, "IFREE,");
			fprintf(stdout, "%d\n", inodeBitmapCounter+1);
		}

		j = (j+1)%8;
		inodeBitmapCounter++;
}


//PROPER INODE SUMMARY TIME

//unsigned int inodeSizeRead = (superBlock -> s_inode_size) * (superBlock -> s_inodes_per_group);

unsigned int inodeSizeRead = superBlock -> s_inode_size;

void* inodeTableBuffer = malloc(inodeSizeRead);

unsigned int inodeCounter = 0;

indirectionTableSize = bsize/sizeof(__u32);

while(inodeCounter < superBlock -> s_inodes_per_group)	{

if(pread(fileSystem, inodeTableBuffer, inodeSizeRead, (bsize * groupTable -> bg_inode_table) + (inodeCounter * (inodeSizeRead))) < (unsigned) inodeSizeRead)	{
	fprintf(stderr, "Error in reading inode table!\n");
	exit(1);
}

	struct ext2_inode *inodeTable = (struct ext2_inode *) inodeTableBuffer;

	struct ext2_inode currentInode = *(inodeTable);

	if(currentInode.i_mode == 0 || currentInode.i_links_count == 0)	{
		inodeCounter++;
		continue;
	}


	fprintf(stdout, "INODE");
	fprintf(stdout, ",%u", inodeCounter+1);
	
	char modeChar = '?';
	if((currentInode.i_mode & 0xF000 )== 0x8000)
		modeChar = 'f';
	else if((currentInode.i_mode & 0xF000 )== 0xA000)
		modeChar = 's';
	else if((currentInode.i_mode & 0xF000 )== 0x4000)
		modeChar = 'd';

	fprintf(stdout, ",%c", modeChar);
	fprintf(stdout, ",%o", currentInode.i_mode & 0x0FFF);
	fprintf(stdout, ",%u", currentInode.i_uid);
	fprintf(stdout, ",%u", currentInode.i_gid);
	fprintf(stdout, ",%u", currentInode.i_links_count);

	//CONVERTING UNIX TIMESTAMP TO GMT STRING
	
	time_t t = currentInode.i_mtime;
	struct tm lt;
	static const char format[] = "%D %T";		
	gmtime_r(&t, &lt);
	char res[32];
	strftime(res, sizeof(res), format, &lt);

	time_t kt = currentInode.i_atime;
	struct tm lm;
	gmtime_r(&kt, &lm);
	char resu[32];
	strftime(resu, sizeof(resu), format, &lm);
	fprintf(stdout, ",%s", resu);
	fprintf(stdout, ",%s", res);
	fprintf(stdout, ",%s", resu);

	//TIME HANDLING COMPLETE

	fprintf(stdout, ",%u", currentInode.i_size);
	fprintf(stdout, ",%u", currentInode.i_blocks);
	
	int indirectionPrinterCounter = 0;
	while(indirectionPrinterCounter < EXT2_N_BLOCKS)	{
		fprintf(stdout, ",%u",currentInode.i_block[indirectionPrinterCounter]);
		indirectionPrinterCounter++;
	}

	fprintf(stdout, "\n");

	//DO DIRECTORY ENTRIES HERE
	if(modeChar =='d')	{

		int directoryEntryCounter = 0;
		int directoryEntryOffset = 0;
		while(1)	{

			void* directoryEntryBuffer = malloc(sizeof(struct ext2_dir_entry));
			if(pread(fileSystem, directoryEntryBuffer, sizeof(struct ext2_dir_entry), (bsize*currentInode.i_block[directoryEntryCounter] ) + directoryEntryOffset) < (unsigned) sizeof(struct ext2_dir_entry))	{
				fprintf(stderr, "Error reading directory entry!\n");
				exit(1);
			}
			struct ext2_dir_entry directoryEntry = *((struct ext2_dir_entry *) directoryEntryBuffer);

			if(directoryEntry.inode == 0)	{
				break;
			}

			fprintf(stdout, "DIRENT");	
			fprintf(stdout, ",%u", inodeCounter+1);
			fprintf(stdout, ",%u", (directoryEntryCounter*bsize) + directoryEntryOffset);
			fprintf(stdout, ",%u", directoryEntry.inode);
			fprintf(stdout, ",%u", directoryEntry.rec_len);
			fprintf(stdout, ",%u", directoryEntry.name_len);

			fprintf(stdout, ",'");

			int namePrinterCounter = 0;
			while(namePrinterCounter < directoryEntry.name_len)	{
				fprintf(stdout, "%c", directoryEntry.name[namePrinterCounter]);
				namePrinterCounter++;
			}

			fprintf(stdout, "'\n");
			directoryEntryOffset += directoryEntry.rec_len;

			if((unsigned) directoryEntryOffset >=  bsize)	{

				directoryEntryCounter++;
				directoryEntryOffset = 0;
			}



		}

	}


	//DO INDIRECT BLOCK REFERENCES HERE
	if(modeChar == 'f' || modeChar == 'd')	{
		int lola = inodeCounter + 1;
		mainBlockOffset = 12;
		processBlock(lola,currentInode.i_block[12], 1, 1);
		processBlock(lola,currentInode.i_block[13], 2, 2);
		processBlock(lola,currentInode.i_block[14], 3, 3);
	}
	inodeCounter++;

}

return 0;
}




void printBlockData(int inode, unsigned int parentBlock, __u32 currentBlock, int recursionLevel)	{

	fprintf(stdout, "INDIRECT");
	fprintf(stdout, ",%u", inode);
	fprintf(stdout, ",%d", recursionLevel);
	//PRINT LOGICAL BLOCK OFFSET HERE
	fprintf(stdout, ",%u", mainBlockOffset);
	//PRINT LOGICAL BLOCK OFFSET HERE
	fprintf(stdout, ",%u",parentBlock);	
	fprintf(stdout, ",%u",currentBlock);	
	fprintf(stdout, "\n");
	return;
}

void processBlock(int inode, unsigned int parentBlock, int recursionLevel, int realRecursionLevel)	{

if(recursionLevel != 0)	{
	void* blockBuffer = malloc(bsize);
	if(pread(fileSystem, blockBuffer, bsize, bsize*parentBlock) < (unsigned) bsize)	{
		fprintf(stderr, "Error reading indirection table!\n");
		exit(1);
	}

	__u32 *indirectionTable = (__u32 *) blockBuffer;
	int i = 0;
	while(i < indirectionTableSize)	{
		
		if(indirectionTable[i] == 0)	{
			i++;


		if(recursionLevel == 1)	{
			mainBlockOffset++;
		}
		else if(recursionLevel == 2)	{
			mainBlockOffset += indirectionTableSize;
		}
		else if(recursionLevel == 3)	{
			mainBlockOffset += (indirectionTableSize*indirectionTableSize);
		}
	
			continue;
		}

		//MAIN BLOCK OFFSET MIGHT BE CHANGED WHILE 
		//PROCESSING ANOTHER INDIRECTION TABLE, SO
		//SAVE THAT STUFF. I COULD ALSO PRINT THE DETAILS BEFORE 
		//PROCESSING THE NEXT BLOCK, BUT I WROTE MY PRINTING
		//FUNCTION A WEEK AGO, AND I'M SCARED IF I CALL IT EARLIER
		//EVERYTHING MIGHT BREAK.

		int oldMainBlockOffset = mainBlockOffset;

		printBlockData(inode, parentBlock, indirectionTable[i], recursionLevel);

		
		
		processBlock(inode, indirectionTable[i], recursionLevel-1, realRecursionLevel);

		int newMainBlockOffset = mainBlockOffset;
		mainBlockOffset = oldMainBlockOffset;
		

		mainBlockOffset = newMainBlockOffset;

		i++;
	}	

}
else	{
mainBlockOffset++;
}


return ;
}
