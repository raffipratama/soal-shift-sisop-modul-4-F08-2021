# soal-shift-sisop-modul-4-F08-2021

## Anggota Kelompok :
Anggota | NRP
--------|-------------
M. Rayhan Raffi P. | 05111940000110
M. Akmal Joedhiawan | 05111940000125
Nur Moh. Ihsanuddien | 05111940000142

## Penjelasan no 1
**a** Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
**b** Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
**c** Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.
**d** Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]
**e** Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

Solusi
````
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
````
Fungsi diatas adalah fungsi untuk melakukan encript menggunkan atbash cypher

````
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
````
Fungsi diatas adalah fungsi untuk melakukan decript menggunkan atbash cypher

````
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
````
Bagian diatas digunakan untuk mengecek apakah ekstensi file sehingga tidak ikut terenkripsi

````
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
```` 
Dilakukan loop untuk melakukan proses enkripsi dari setiap huruf yang ada dari nama sebuah file

````
static int xmp_getattr(const char *path, struct stat *st){
    printf("getattr %s\n",path);
    
    int res;
    char fpath[1000];
    char paths[100];
    char name[1000];
    strcpy(paths, path);
    strcpy(name,path);
    if(strstr(path, "/AtoZ_")!=NULL)
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
````
Untuk proses dari dekripsi sendiri yaitu terletak fungsi `xmp_getattr` dimana apabila terdapat folder yang memiliki nama file `AtoZ/` maka akan dilakukan proses dekripsi 

````
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, 
    off_t offset, struct fuse_file_info *fi){
    printf("readdir\n");
    char fpath[1000];
     int mode=0;
    if(strstr(path, "/AtoZ_")!=NULL)
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
````
Sedangkan pada fungsi `xmp_readdir`, apabila `de->d_name` bertemu dengan string AtoZ/ maka akan dilakukan proses enkripsi

````
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
````
System-call akan menerima parameter path dan mode_t mode. System-call akan melakukan decrypt_ untuk string AtoZ_ dan menyimpan dalam fpath serta memberikan kontrol untuk membuat directory baru melalui perintah `0777`

Untuk Fungsi lain akan melakukan decipt apabila bertemu dengan string AtoZ_

## Penjelasan no 4
**a** Log system yang akan terbentuk bernama “SinSeiFS.log” pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.
**b** Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.
**c** Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.
**d** Sisanya, akan dicatat pada level INFO.

Solusi

````
int print_info_command(char *command, char *desc){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char mains[1000];
    sprintf(mains,"INFO::%02d%02d%02d-%02d:%02d:%02d::%s::%s\n",
        tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,  
        tm.tm_hour, tm.tm_min, tm.tm_sec, command, desc);
    printf("%s", mains);
    FILE *foutput = fopen(log_path, "a+");
    fputs(mains, foutput);
    fclose(foutput);
    return 1;
}
````
Fungsi diatas akan melakukan perintah print pada file log yang telah dibuat dan menggunakan waktu saat dilakukan decript dan digunakan file bertipe Info
````
int print_warning_command(char *command, char *desc){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char mains[1000];
    sprintf(mains,"WARNING::%02d%02d%04d-%02d:%02d:%02d::%s::%s\n",
        tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,  
        tm.tm_hour, tm.tm_min, tm.tm_sec, command, desc);
    printf("%s", mains);
    FILE *foutput = fopen(log_path, "a+");
    fputs(mains, foutput);
    fclose(foutput);
    return 1;
}
````
Fungsi diatas akan melakukan perintah print pada file log yang telah dibuat dan menggunakan waktu saat dilakukan decript dan digunakan file bertipe Warning

**Kendala**
- Masih belum terlalu paham cara menggunakan fuse
- Terjadi kesalahan penghapus file tanpa mengunmount terlebih dahulu

