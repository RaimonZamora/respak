// ************************************************************************
// ResPak - A file packer for JailGames
//
// Author: Raimon Zamora
//
// ************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define OP_PACK 0
#define OP_UNPACK 1
#define OP_LIST 2

#pragma pack(push,1)

struct DATA_Header {
	char		magic[4];
	uint32_t 	num_files;
	uint32_t 	index_offset;
};

struct DATA_Info {
	uint32_t	offset;
	uint32_t	length;
	char		name[13];
};

struct DATA_File {
	struct DATA_Header	header;
	struct DATA_Info*	index;
};

#pragma pack(pop)

char data_file_name[16] = "data.jrf";
char data_directory_name[16] = "data/";
int op = OP_PACK;

void do_list()
{
	struct DATA_File data_file;
	FILE* data = fopen(data_file_name, "rb");
	fread(&data_file.header, sizeof(struct DATA_Header), 1, data);
	data_file.index = malloc(sizeof(struct DATA_Info) * data_file.header.num_files);
	fseek(data, data_file.header.index_offset, SEEK_SET);
	fread(data_file.index, sizeof(struct DATA_Info), data_file.header.num_files, data);
	fclose(data);
	printf("DATA FILE CONTENTS (%i files):\n==============================\n", data_file.header.num_files);
	int i;
	for (i = 0; i < data_file.header.num_files; i++)
	{
		printf("%s\t(%i bytes)\n", data_file.index[i].name, data_file.index[i].length);
	}
}

void do_unpack()
{
	struct stat st = {0};
	struct DATA_File data_file;
	FILE* data = fopen(data_file_name, "rb");
	fread(&data_file.header, sizeof(struct DATA_Header), 1, data);
	data_file.index = malloc(sizeof(struct DATA_Info) * data_file.header.num_files);
	fseek(data, data_file.header.index_offset, SEEK_SET);
	fread(data_file.index, sizeof(struct DATA_Info), data_file.header.num_files, data);
	printf("Extracting files.");
	if (stat(data_directory_name, &st) == -1) mkdir(data_directory_name, 0700);	
	int i;
	for (i = 0; i < data_file.header.num_files; i++)
	{
		fseek(data, data_file.index[i].offset, SEEK_SET);
		char* buffer = malloc(data_file.index[i].length);
		fread(buffer, data_file.index[i].length, 1, data);
		
		char path[32] = "";
		strcpy(path, data_directory_name);
		strcat(path, "/");
		strcat(path, data_file.index[i].name);
		FILE* f = fopen(path, "wb");
		fwrite(buffer, data_file.index[i].length, 1, f);
		fclose(f);
		printf(".");
	}
	printf("DONE!\n");
	fclose(data);
}

void do_pack()
{
	const char header[12] = "PAK\0\0\0\0\0\0\0\0\0";
	struct DATA_Info index[100];
	int num_files = 0;
	int index_offset = 12;
	
	FILE* data = fopen(data_file_name, "wb");
	fwrite(header, 12, 1, data);
	
	struct dirent *ep;
	DIR* d = opendir(data_directory_name);
	
	if (d != NULL)
	{
		while ((ep = readdir(d)))
		{
			if (ep->d_type != 4)
			{
				char file_name[100] = "";
				strcpy(file_name, data_directory_name);
				strcat(file_name, ep->d_name);
				FILE* f = fopen(file_name, "rb");
				fseek(f, 0L, SEEK_END);
				int file_size = ftell(f);
				rewind(f);
				char* buffer = malloc(file_size);
				fread(buffer, file_size, 1, f);
				fwrite(buffer, file_size, 1, data);
				free(buffer);
				fclose(f);
				index[num_files].offset = index_offset;
				index[num_files].length = file_size;
				strcpy(index[num_files].name, ep->d_name);
				num_files++;
				index_offset += file_size;
			}
		}
		closedir(d);
		fwrite(index, sizeof(struct DATA_Info), num_files, data);
		fseek(data, 4, SEEK_SET);
		fwrite(&num_files, 4, 1, data);
		fwrite(&index_offset, 4, 1, data);
	}	
	fclose(data);
}

int main (int argc, const char * argv[])
{
	if (argc > 3) { strcpy(data_directory_name, argv[3]); }
	if (argc > 2) { strcpy(data_file_name, argv[2]); }
	if (argc > 1) { op = (argv[1][1] == 'u' ? OP_UNPACK : argv[1][1] == 'l' ? OP_LIST : OP_PACK); }
	
	switch (op) {
		case OP_LIST: do_list(); break;
		case OP_UNPACK: do_unpack(); break;
		default: do_pack(); break;
	};
}

