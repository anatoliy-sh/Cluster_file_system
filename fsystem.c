#define FUSE_USE_VERSION  26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

static int cl_getattr(const char *path, struct stat *stbuf);
static int cl_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int cl_open(const char *path, struct fuse_file_info *fi);
static int cl_release(const char *path, struct fuse_file_info *fi);
static int cl_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info * fi);
static void *cl_init(struct fuse_conn_info * conn);
static void cl_destroy(void *a);
static int cl_truncate(const char *path, off_t size);
static int cl_create(const char *path, mode_t mode, struct fuse_file_info *fi);
static int cl_write(const char *path, const char *content, size_t size, off_t offset, struct fuse_file_info *fi);

static struct fuse_operations oper = {
    .readdir = cl_readdir,
    .create = cl_create,
    .open = cl_open,
    .read = cl_read,
    .write = cl_write,
    .truncate = cl_truncate,
    .release = cl_release,
    .flush = NULL,
    .getattr = cl_getattr,
    .destroy = cl_destroy,
    .init = cl_init
};

typedef struct Cluster* LinkCl; 
struct Cluster 
 {
    int id;
    char* name; 
    char content[100]; 
    LinkCl nextCluster; 
};

typedef struct Cluster ClusterType;


//typedef struct Node* Link; 
struct  Node
{
    char* name; 
    int idCluster;
    char* path;
    //Link nextNode; 
    //Link* childs; 
    //int childCount;
};

typedef struct Node NodeType;


static LinkCl cluster;
static NodeType files[100];
static int file_ind;



int main(int argc, char *argv[]) {

    cluster = (LinkCl)malloc(sizeof(ClusterType));//createClusters();
    cluster->id = 0; 
    //clusters->content = "";
    cluster->nextCluster = 0;
    file_ind = 0;
    NodeType myFile;
    myFile.name = "test";
    myFile.idCluster = 1;
    myFile.path = "/hello";
    files[file_ind] = myFile;

    return fuse_main(argc, argv, &oper, NULL);
}

/*LinkCl createClusters()
{
    LinkCl clusters = (LinkCl)malloc(sizeof(struct Node));
    clusters->id = 0; 
    clusters->content = 0;
    clusters->nextCluster = 0;
    return clusters;
}*/

static void *cl_init(struct fuse_conn_info * conn) {
    /* 
      Initialize filesystem
      ...
    */
    printf("Filesystem has been initialized!\n");
    return NULL;
}

static void cl_destroy(void *a) {
    /*
      Close filesystem 
      ...
    */
    printf("Filesystem has been destroyed!\n");
}

static int cl_getattr(const char *path, struct stat * stbuf) {
    printf("getattr: %s\n", path);
    /*
      Get file info
      ...
    */

    int res = 0; 
    memset(stbuf, 0, sizeof(struct stat)); 
    if (strcmp(path, "/") == 0) { 
      stbuf->st_mode = S_IFDIR | 0755; stbuf->st_nlink = 2; 
    } else if (strcmp(path, files[file_ind].path) == 0) { 
      stbuf->st_mode = S_IFREG | 0444; stbuf->st_nlink = 1; 
      stbuf->st_size = strlen("hello_str"); 
    } else 
    res = -ENOENT; 
    return res; 

    //memset(stbuf, 0, sizeof (struct stat));
    //stbuf->st_mode = /*mode*/
    //stbuf->st_nlink = /*count of links*/
    //stbuf->st_size = /*file size*/

    return 0;
}

static int cl_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info * fi) {
    /*
      Get list of files in directory
      ...
    */
  (void) offset; 
  (void) fi;
  if (strcmp(path, "/") != 0) 
    return -ENOENT; 
  filler(buf, ".", NULL, 0); 
  filler(buf, "..", NULL, 0); 
  filler(buf, files[file_ind].path + 1, NULL, 0);
  return 0;

    // foreach file
    //filler(buf, fileName, NULL, 0); // add filename in directory *path* to buffer

    return 0;
}

static int cl_open(const char *path, struct fuse_file_info * fi) {
    printf("open: %s\n", path);

    /*
      Get list of files in directory
      ...
    */
    if (strcmp(path, files[file_ind].path) != 0) 
      return -ENOENT;
       if ((fi->flags & 3) != O_RDONLY)
        return -EACCES; 
      return 0; 
    //fileInfo->fileInode = // set inode;
    //fileInfo->fullfileName = // specify file name;
    //fileInfo->isLocked = false; // set is file locked
/*if (strcmp(path, hello_path) != 0)
return -ENOENT;
if ((fi->flags & 3) != O_RDONLY)
return -EACCES;*/
    

    printf("open: Opened successfully\n");

    return 0;
}

static int cl_release(const char *path, struct fuse_file_info * fi) {
    printf("release: %s\n", path);
    /*
      Unlock file
      ...
    */
    return 0;
}

static int cl_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info * fi) {
    printf("read: %s\n", path);
    /*
      Read file
      ...
      byte *fileContent = readFile(fileInode);
      memcpy(buf, fileContent, size);
    */  
    printf("read: %s\n", path); 
    size_t len; (void) fi; 
    if(strcmp(path, files[file_ind].path) != 0) 
      return -ENOENT; 
    len = strlen("hello_str");
      if (offset < len) { 
        if (offset + size > len) 
          size = len - offset; 
        memcpy(buf, "hello_str" + offset, size); 
      } else size = 0; 
    return size; 
}

static int cl_truncate(const char *path, off_t size) {
    /*
      truncate file
      ...
    */

    printf("truncate: Truncated successfully\n");
    return 0;
}

static int cl_create(const char *path, mode_t mode, struct fuse_file_info *fi) {

  printf("create: %s\n", path);
    /*
      Create inode
      ...
    */
  return 0;
}

static int cl_mknod(const char* path, mode_t mode, dev_t dev)
{
 
    return 0;
}


static int cl_write(const char *path, const char *content, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("write: %s\n", path);
    /*
      Write bytes to fs
      ...
    */
    return 0; // Num of bytes written
}
