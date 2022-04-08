//
// 	libarchive.c
//	IAmLazy
//
//	Created by Lightmann during COVID-19
//

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>
#include <archive.h>
#include <sys/fcntl.h>
#include <archive_entry.h>

// Helpful ref:
// https://stackoverflow.com/a/4204758
int get_file_count(){
	int file_count = 0;

	// get deb count
	struct dirent *entry;
	DIR *directory = opendir("/tmp/me.lightmann.iamlazy/");
	if(!directory) return 0;
	while((entry = readdir(directory)) != NULL){
		// if entry is a regular file
		if(entry->d_type == DT_REG){
			file_count++;
		}
	}
	closedir(directory);

	return file_count;
}

// Helpful ref:
// https://github.com/libarchive/libarchive/wiki/Examples#A_Basic_Write_Example
void write_archive(const char *outname){
	struct archive *a;
	struct archive_entry *entry;
	struct stat st;
	char buff[8192];
	int len;
	int fd;

	// get file count for tmpDir
	int file_count = get_file_count();
	// create string array for filepaths
	const char *filearr[file_count+1];
	// get pointers (required by libarchive)
	const char **ptrs = filearr;

	// add filepaths to array
	struct dirent *ent;
	DIR *directory = opendir("/tmp/me.lightmann.iamlazy/");
	if(!directory) return;
	int count = 0;
	while((ent = readdir(directory)) != NULL){
		// if entry is a regular file
		if(ent->d_type == DT_REG){
			// https://stackoverflow.com/a/8465083
			char *str = malloc(strlen("me.lightmann.iamlazy/") + strlen(ent->d_name) + 1);
			strcpy(str, "me.lightmann.iamlazy/");
			strcat(str, ent->d_name);

			// assign the filepath
			filearr[count] = str;
			count++;
		}
	}

	closedir(directory);

	// change dir to avoid
	// including it in archive
	chdir("/tmp/");

	// go to work
	a = archive_write_new();
	archive_write_add_filter_gzip(a);
	archive_write_set_format_pax_restricted(a);
	archive_write_open_filename(a, outname);
	for(int i = 0; i < file_count; i++){
		const char *file = ptrs[i];

		if(!file) continue;

		stat(file, &st);
		entry = archive_entry_new();
		archive_entry_set_pathname(entry, file);
		archive_entry_set_size(entry, st.st_size);
		archive_entry_set_filetype(entry, AE_IFREG);
		archive_entry_set_perm(entry, 0644);
		archive_write_header(a, entry);
		fd = open(file, O_RDONLY);
		len = read(fd, buff, sizeof(buff));
		while(len > 0){
			archive_write_data(a, buff, len);
			len = read(fd, buff, sizeof(buff));
		}
		close(fd);
		// must free after malloc
		free((char *)filearr[i]);
		archive_entry_free(entry);
	}
	archive_write_close(a);
	archive_write_free(a);
}
