#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
extern int errno;

#ifndef false
#	define false 0
#endif
#ifndef true
#	define true !false
#endif

#define	regular_concat(a,b)	a ## b
#define value_concat(a,b)	reglar_concat(a,b)
#define line_var(prefix)	value_concat(value_concat(prefix, _), __LINE__ )

#define debug(...)	do	\
			{	\
				fprintf(stderr, "[debug:%s:%4u:%s] ", __FILE__, __LINE__, __FUNCTION__);	\
				fprintf(stderr, __VA_ARGS__);	\
				fputc('\n', stderr);	\
			} while(false);
#define DIR_MODE	(S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH)
typedef uint_fast8_t bool;

bool usable_dir(char *path)
{
	struct stat dirstat;

	if(stat(path, &dirstat))
	{
		debug("stat failed for directory %s", path);
		return false;
	}
	if(!S_ISDIR(dirstat.st_mode))
	{
		debug("%s not a directory", path);
		return false;
	}
	if(access(path, X_OK))
	{
		debug("access failed for %s", path);
		return false;
	}
	return true;
}

bool usable_file(char *path)
{
	struct stat filestat;

	if(stat(path, &filestat))
		return false;
	if(!S_ISREG(filestat.st_mode))
		return false;
	if(access(path, R_OK))
		return false;
	return true;
}

int mkdir_p(const char *path)
{
	char *work;
	char *ptr;
	bool end = false;
	struct stat dirstat;

	debug("starting with %s", path);

	/* check operands */
	if(!path || !*path)
	{
		errno = ENOENT;
		return -1;
	}

	/* create working copy of path */
	ptr = work = alloca(strlen(path) * sizeof(char));
	strcpy(work, path);

	/* loop through path components */
	while(*ptr)
	{
		/* find next slash or end */
		for(;*ptr && *ptr != '/';ptr++);

		if(!*ptr)	/* found end of string */
			end=true;
		else		/* treminate string on slash */
			*ptr = '\0';

		/* check directory existance */
		debug("checking %s", work);
		if(stat(work, &dirstat))
		{
			debug("%s doesn't exist", work);
			/* if it doesn't exist, try mkdir */
			if(errno == ENOENT && mkdir(work, DIR_MODE))
				return -1;
		}
		else if(!S_ISDIR(dirstat.st_mode))
		{	/* might as well be nice and set errno */
			debug("%s is not a directory", work);
			errno = ENOTDIR;
			return -1;
		}

		/* replace slash */
		if(!end)
		{
			*ptr = '/';
			ptr++;
		}
	}

	/* return success */
	debug("should have %s now", path);
	return 0;
}

int get_cache_file_16(char *base, uint16_t id, char *file, int flags)
{
	char *path;
	char *ptr;
	char hex[5];
	size_t base_len = strlen(base);

	/* alloc our path */
	path = (char *)alloca(base_len + strlen(file) + /* strlen("/FF/FF/")*/ 7 + 1);

	/* if flags isn't set, do read only */
	if(!flags)
		flags = O_RDONLY;

	/* get a hex representation of id */
	sprintf(hex, "%04X", id);

	/* start with base */
	strcpy(path, base);
	ptr = path + base_len;

	/* add msb */
	*ptr = '/';
	ptr++;
	*ptr = hex[0];
	ptr++;
	*ptr = hex[1];
	ptr++;

	/* add lsb */
	*ptr = '/';
	ptr++;
	*ptr = hex[2];
	ptr++;
	*ptr = hex[3];
	ptr++;
	*ptr = '\0';

	debug("directory path is %s", path);

	/* check dir and create it if possible */
	if(!usable_dir(path) && (!(flags & O_CREAT) || mkdir_p(path)))
		return -1;

	/* add file on */
	*ptr = '/';
	ptr++;
	strcpy(ptr, file);

	debug("file path is %s", path);

	/* open the file */
	return open(path, flags);
}

int main(int argc, char ** argv)
{
	int fd;

	/* get cache file for id */
	fd = get_cache_file_16("./keys", argc == 2 ? atoi(argv[1]) : 257, "to.der", O_CREAT | O_RDWR);
	if(fd < 0)
	{
		perror("could not open cache file");
		return -1;
	}

	/* close the file if we got it */
	if(close(fd))
	{
		perror("could not close cache file");
		return -1;
	}

	/* return success */
	return 0;
}
