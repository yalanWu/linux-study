#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <uuid/uuid.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

int main(int argc, char * argv[])
{
	struct stat buf;
	char* file = argv[1];
	int ret=stat(file,&buf);
	//buf->st_mode;
	//buf->st_uid;
	//buf->st_gid;
	//buf->st_size;
	char ctype[10]={0};
	int um = buf.st_mode&S_IFMT;
	if(um == S_IFREG)
	{
		ctype[0]='-';
	}
	else if(um == S_IFIFO)
	{
		ctype[0]='p';
	}
	else if(um == S_IFCHR)
	{
		ctype[0]='c';
	}
	else if(um == S_IFDIR)
	{
		ctype[0]='d';
	}
	else if(um == S_IFBLK)
	{
		ctype[0]='b';
	}
	else if(um == S_IFLNK)
	{
		ctype[0]='l';
	}
	else if(um == S_IFSOCK)
	{
		ctype[0]='l';
	}
	// 读写权限
	um = buf.st_mode & S_IRWXU;
	if(um & S_IRUSR)
	{
		ctype[1]='r';
	}
	else
	{
		ctype[1]='-';
	}
	if(um & S_IWUSR)
	{
		ctype[2]='w';
	}
	else
	{
		ctype[2]='-';
	}
	if(um & S_IXUSR)

	{
		ctype[3]='x';
	}
	else
	{
		ctype[3]='-';
	}	


	um = buf.st_mode & S_IRWXG;
	if(um & S_IRGRP)
	{
		ctype[4]='r';
	}
	else
	{
		ctype[4]='-';
	}
	if(um & S_IWGRP)
	{
		ctype[5]='w';
	}
	else
	{
		ctype[5]='-';
	}
	if(um & S_IXGRP)

	{
		ctype[6]='x';
	}
	else
	{
		ctype[6]='-';
	}

	char* time = ctime(&buf.st_mtime);
	char mtime[512]={0};
	strncpy(mtime,time,strlen(time));

	struct passwd* upd = getpwuid(buf.st_uid);
	struct group* gpd = getgrgid(buf.st_gid);
	printf("%s %d %s %s %d %s %s\n ",ctype, (int)buf.st_nlink, upd->pw_name,gpd->gr_name, (int)buf.st_size, mtime, file);	




	return 0;
}
