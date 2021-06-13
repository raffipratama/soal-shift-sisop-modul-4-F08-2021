#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif


static  const  char *dirpath = "/home/akmal/Downloads";
static const char *log_path = "/home/akmal/SinSeifs.log";
/*
    Fungsi ini akan dipanggil ketika sistem meminta SinSeiFS
    untuk atribu-atribut dari file spesifik.  
*/

char lower[] = "abcdefghijklmnopqrstuvwxyz";
char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void encrypt(char *str)
{
    int str_len = strlen(str);
    int mulai = 0;

    for (int i = strlen(str); i >= 0; i--)
    {
        if (str[i] == '/')
        {
            break;
        }
        else if (str[i] == '.')
        {
            str_len = i;
        }
    }

    for (int i = 1; i < str_len; i++)
    {
        if (str[i] == '/')
        {
            mulai = i;
        }
    }

    for (int i = mulai; i < str_len; i++)
    {
        if (str[i] == '/')
        {
            continue;
        }
        if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97) || (str[i] > 122 && str[i] <= 127)))
        {
            if (str[i] >= 'A' && str[i] <= 'Z')
                str[i] = 'Z' + 'A' - str[i];
            if (str[i] >= 'a' && str[i] <= 'z')
                str[i] = 'z' + 'a' - str[i];
        }
    }
}

void decrypt(char *str)
{
    int str_len = strlen(str);
    int mulai = 0;

    for (int i = 1; i < str_len; i++)
    {
        if (str[i] == '/' || str[i + 1] == '\0')
        {
            mulai = i + 1;
            break;
        }
    }

    for (int i = strlen(str); i >= 0; i--)
    {
        if (str[i] == '/')
        {
            break;
        }
        else if (str[i] == '.' && i == (strlen(str) - 1))
        {
            str_len = strlen(str);
            break;
        }
        else if (str[i] == '.' && i != (strlen(str) - 1))
        {
            str_len = i;
            break;
        }
    }

    for (int i = mulai; i < str_len; i++)
    {
        if (str[i] == '/')
        {
            continue;
        }
        if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97) || (str[i] > 122 && str[i] <= 127)))
        {
            if (str[i] >= 'A' && str[i] <= 'Z')
                str[i] = 'Z' + 'A' - str[i];
            if (str[i] >= 'a' && str[i] <= 'z')
                str[i] = 'z' + 'a' - str[i];
        }
    }
}



int print_info_command(char *command, char *desc){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char mains[1000];
    sprintf(mains,"INFO::%02d%02d%02d-%02d:%02d:%02d::%s::%s::%s\n",
        (tm.tm_year + 1900) % 1000, tm.tm_mon + 1, 
        tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, command, desc, desc);
    printf("%s", mains);
    FILE *foutput = fopen(log_path, "a+");
    fputs(mains, foutput);
    fclose(foutput);
    return 1;
}

int print_warning_command(char *command, char *desc){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char mains[1000];
    sprintf(mains,"WARNING::%02d%02d%02d-%02d:%02d:%02d::%s::%s::%s\n",
        (tm.tm_year + 1900) % 1000, tm.tm_mon + 1, 
        tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, command, desc, desc);
    printf("%s", mains);
    FILE *foutput = fopen(log_path, "a+");
    fputs(mains, foutput);
    fclose(foutput);
    return 1;
}

//info level command
static int xmp_getattr(const char *path, struct stat *st){
    printf("getattr %s\n",path);
    
    int res;
    char fpath[1000];
    char paths[100];
    char name[1000];
    strcpy(paths, path);
    strcpy(name,path);
    if(strstr(path, "AtoZ_")!=NULL)
    {
        decrypt(name);
    }
    else
    {
        strcpy(name,path);
    }
    printf("%s\n", name);
    if(print_info_command("GETATTR", paths)){
        sprintf(fpath, "%s/%s", dirpath, name);
        res = lstat(fpath, st);
        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan getattr sukses\n");
        }
    }
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, 
    off_t offset, struct fuse_file_info *fi){
    printf("readdir\n");
    char fpath[1000];
     int mode=0;
    if(strstr(path, "AtoZ_")!=NULL)
    {
        mode=1;
    }
    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        
        sprintf(fpath, "%s%s", dirpath, path);
    }

    int res = 0;

    DIR *dp;
    struct dirent *de;
    char paths[100];
   
    strcpy(paths, path);
    // (void) offset;
    // (void) fi;
    if(print_info_command("READDIR", paths)){
        dp = opendir(fpath);
        if(dp == NULL){
            return -errno;
        }else{
            while ((de = readdir(dp)) != NULL) {
            struct stat st;
            memset(&st, 0, sizeof(st));
            st.st_ino = de->d_ino;
            st.st_mode = de->d_type << 12;
              char temp[1000];
            strcpy(temp, de->d_name);
            if(mode==1)
            {
                encrypt(temp);
            }
            res = (filler(buf, temp, &st, 0));
                if(res!=0) {
                    break;
                }
            }
            printf("penulisan dan readdir sukses\n");
            closedir(dp);
        }
    }
    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode){
    printf("enter => mkdir\n");
    // printf("%s", path);
    char fpath[500] = "/home/akmal/Downloads";
    strcat(fpath, path);
    char paths[100];
    strcpy(paths, path);
    if(print_info_command("MKDIR", paths)){
        mkdir(fpath, 0777);
        printf("penulisan dan mkdir sukses\n");        
    }
    return 0;
}

static int xmp_rename(const char *from, const char *to){
    printf("enter => rename\n");
    // rename the file, directory, or other object "from" to the target "to"
    // source and target don't have to be in the same directory
    int res;
    char ffrom[1000];
    if (strcmp(from, "/") == 0)
    {
        from = dirpath;
        sprintf(ffrom, "%s", from);
    }
    else 
    {
        char temp[1000];
        strcpy(temp, from);

        if(strncmp(from, "/AtoZ_", 6) == 0)
            decrypt(temp);

        sprintf(ffrom, "%s%s", dirpath, temp);
    }

    char fto[1000];
    if (strcmp(to, "/") == 0)
    {
        to = dirpath;
        sprintf(fto, "%s", to);
    }
    else 
    {
        char temp[1000];
        strcpy(temp, to);

        if(strncmp(to, "/AtoZ_", 6) == 0)
            decrypt(temp);

        sprintf(fto, "%s%s", dirpath, temp);
    }

    char joins[1000];
    sprintf(joins, "%s::%s", ffrom, fto);
    
    if(print_info_command("RENAME", joins)){
        res = rename(ffrom, fto);
        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan rename sukses");
        }
    }
    return 0;
}

static void *xmp_init(struct fuse_conn_info *conn){
    printf("enter => init");
    /*Initialize the filesystem. This function can often be left unimplemented, 
    but it can be a handy way to perform one-time setup such as allocating variable-sized 
    data structures or initializing a new filesystem. The fuse_conn_info structure gives 
    information about what features are supported by FUSE, and can be used to request 
    certain capabilities*/
    FILE *fpointer = fopen(log_path, "rb+");
    if(fpointer == NULL){
        fpointer = fopen(log_path, "wb");
        fclose(fpointer);
    }else{
        fclose(fpointer);
    }
    return 0;
}


static int xmp_readlink(const char *path, char *buf, size_t size){
    printf("enter => readlink\n");
    //If path is a symbolic link, fill buf with its target, up to size
    int res;
    char fpath[1000];

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else 
    {
        char temp[1000];
        strcpy(temp, path);

        if(strstr(path, "/AtoZ_")!=NULL)
            decrypt(temp);

        sprintf(fpath, "%s%s", dirpath, temp);
    }
    char paths[100];
    strcpy(paths, path);
    if(print_info_command("READLINK", paths)){
        res = readlink(fpath, buf, size-1);
        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan readlink success\n");
        }
    }
    return 0;
}


static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){
    printf("enter => mknod\n");
    //make special (device) file, FIFO, or socket
    int res;
   char fpath[1000];

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else 
    {
        char temp[1000];
        strcpy(temp, path);

        if(strstr(path, "/AtoZ_")!=NULL)
            decrypt(temp);

        sprintf(fpath, "%s%s", dirpath, temp);
    }
    char paths[100];
    strcpy(paths, path);
    if(print_info_command("MKNOD", paths)){
        if(S_ISREG(mode)){
        res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0){
            res = close(res);
        }
        }else if(S_ISFIFO(mode)){
            res = mkfifo(fpath, mode);
        }else{
            res = mknod(fpath, mode, rdev);
        }

        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan mknod success\n");
        }
    }

    return 0;
}

static int xmp_chmod(const char *path, uid_t uid, gid_t gid, mode_t mode){
    printf("enter => chmod");
    // Change the given object's owner and group to the provided values.
    int res;
    char fpath[1000];

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else 
    {
        char temp[1000];
        strcpy(temp, path);

        if(strstr(path, "/AtoZ_")!=NULL)
            decrypt(temp);

        sprintf(fpath, "%s%s", dirpath, temp);
    }
    char paths[100];
    strcpy(paths, path);
    if(print_warning_command("CHMOD", paths)){
        res = chmod(fpath, mode);
        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan chmod sukses\n");
        }        
    }
    return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid){
    printf("enter => chown");
    int res;
    char fpath[1000];

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else 
    {
        char temp[1000];
        strcpy(temp, path);

        if(strstr(path, "/AtoZ_")!=NULL)
            decrypt(temp);

        sprintf(fpath, "%s%s", dirpath, temp);
    }
    char paths[100];
    strcpy(paths, path);
    if(print_warning_command("CHOWN", paths)){
        res = chown(fpath, uid, gid);
        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan chmown sukses\n");
        }        
    }
    return 0;
}

static int xmp_truncate(const char *path, off_t size){
    printf("enter => truncate");
    int res;
    char fpath[1000];

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else 
    {
        char temp[1000];
        strcpy(temp, path);

        if(strstr(path, "/AtoZ_")!=NULL)
            decrypt(temp);

        sprintf(fpath, "%s%s", dirpath, temp);
    }
    char paths[100];
    strcpy(paths, path);
    if(print_warning_command("TRUNCATE", paths)){
        res = truncate(fpath, size);
        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan truncate sukses\n");
        }        
    }
    return 0;
}


static int xmp_open(const char *path, struct fuse_file_info* fi){
    printf("enter => open\n");
    int res;
    char fpath[1000];

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else 
    {
        char temp[1000];
        strcpy(temp, path);

        if(strstr(path, "/AtoZ_")!=NULL)
            decrypt(temp);

        sprintf(fpath, "%s%s", dirpath, temp);
    }
    char paths[1000];
    strcpy(paths, path);
    if(print_warning_command("OPEN", paths)){
        res = open(fpath, fi->flags);
        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan open sukses\n");
        }
        close(res);        
    }
    return 0;
}

static int xmp_read(const char* path, char *buf, size_t size, off_t offset, 
    struct fuse_file_info* fi){
    printf("enter => read");
    int fd;
	int res;

    char fpath[1000];

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else 
    {
        char temp[1000];
        strcpy(temp, path);

        if(strstr(path, "/AtoZ_")!=NULL)
            decrypt(temp);

        sprintf(fpath, "%s%s", dirpath, temp);
    }
    char paths[1000];
    strcpy(paths, path);
    if(print_info_command("READ", paths)){
        (void) fi;
        fd = open(fpath, O_RDONLY);
        if (fd == -1){
            return -errno;
        }else{
            res = pread(fd, buf, size, offset);
            if (res == -1){
                res = -errno;
            }else{
                printf("penulisan dan read sukses\n");
                return res;
            }
        }
        close(fd);
    }
    return 0;
}

static int xmp_write(const char* path, char *buf, size_t size, off_t offset, 
    struct fuse_file_info* fi){
    printf("enter => write");
    int fd;
	int res;

    char fpath[1000];

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else 
    {
        char temp[1000];
        strcpy(temp, path);

        if(strstr(path, "/AtoZ_")!=NULL)
            decrypt(temp);

        sprintf(fpath, "%s%s", dirpath, temp);
    }
    char paths[100];
    strcpy(paths, path);

    if(print_info_command("WRITE", paths)){
        (void) fi;
        fd = open(fpath, O_WRONLY);
        if (fd == -1){
            return -errno;
        }else{
            res = pwrite(fd, buf, size, offset);
            if (res == -1){
                res = -errno;
            }else{
                printf("penulisan dan write sukses\n");
                return res;
            }
        }
        close(fd);
    }
    return 0;
}

static int xmp_statfs(const char* path, struct statvfs* stbuf){
    printf("enter => statfs");
    int res;
    char fpath[500] = "/home/akmal/Downloads";
    strcat(fpath, path);
    char paths[100];
    strcpy(paths, path);
    if(print_warning_command("STATFS", paths)){
        res = statvfs(fpath, stbuf);
        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan statfs sukses\n");
        }        
    }
    return 0;
}


static int xmp_release(const char* path, struct fuse_file_info *fi){
    printf("enter => release");
    (void) path;
	(void) fi;
    return 0;
}

static int xmp_fsync(const char* path, int isdatasync, struct fuse_file_info* fi){
    printf("enter => fsync");
    (void) path;
	(void) isdatasync;
	(void) fi;
    return 0;
}

// Warning level command
static int xmp_rmdir(const char *path){
    printf("Enter => rmdir\n");
    int res;
    char fpath[500] = "/home/akmal/Donwloads";
    strcat(fpath, path);
    char paths[100];
    strcpy(paths, path);
    if(print_warning_command("RMDIR", paths)){
        res = rmdir(fpath);
        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan rmdir sukses\n");
        }        
    }
    return 0;
}

static int xmp_unlink(const char *path, mode_t mode){
    printf("Enter => rmdir\n");
    int res;
    char fpath[500] = "/home/akmal/Donwloads";
    strcat(fpath, path);
    char paths[100];
    strcpy(paths, path);
    if(print_warning_command("UNLINK", paths)){
        res = unlink(fpath);
        if(res == -1){
            return -errno;
        }else{
            printf("penulisan dan unlink sukses\n");
        }        
    }
    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .rmdir = xmp_rmdir,
    .unlink = xmp_unlink,
    .rename = xmp_rename,
    .mkdir = xmp_mkdir,
    .init = xmp_init,
    .readlink = xmp_readlink,
    .mknod = xmp_mknod,
    .chmod = xmp_chmod,
    .chown = xmp_chown,
    .truncate = xmp_truncate,
    .open = xmp_open,
    .read = xmp_read,
    .write = xmp_write,
    .statfs = xmp_statfs,
    .release = xmp_release,
    .fsync = xmp_fsync,

};


int main(int argc, char *argv[]){
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
