#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
getname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), 0, DIRSIZ-strlen(p));
  return buf;
}

void
find(char *path, char *name)
{
  //printf("path=%s name=%s\n",path,name);
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  if(!strcmp(getname(path),name)){
      printf("%s\n",path);
  }

  switch(st.type){
  case T_FILE:
    break;

  case T_DIR:
    
    strcpy(buf, path);
    //printf("buf=%s len=%d\n",buf,strlen(buf));
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("ls: path too long\n");
            break;
        }
        if(de.inum == 0 || !strcmp(de.name,".") || !strcmp(de.name,".."))
            continue;
        //printf("de.name=%s\n",de.name);
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ]=0;
        find(buf,name);
      //printf("%s\n", getname(buf));
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
    if(argc==3){
      find(argv[1],argv[2]);
      exit(0);
    }
    exit(0);

}