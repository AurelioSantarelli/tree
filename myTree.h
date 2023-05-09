#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <pwd.h>
#include <grp.h>


typedef struct {
    char *name;
    char *full_path;
    char *permissions;
    int size;
    int user;
    int group;
    time_t mtime;
    int inode;
    char *last_mod;
    int is_dir;

} fileinfo;


//iniz. contatori 
extern int countdir;
extern int countfiles;
extern int max_depth;
extern char *dirPath;
extern int sort;

//features
#define n_features 15
extern int active_features [n_features];


//functions
void help();
void flagChecker(int argc, char* argv[]);
char* convert_time(int tempo_unix);
void numeric_to_string_mode(mode_t mode, char* buf);
int compare_name_reverse(const void *a, const void *b);
int compare_name(const void *a, const void *b);
int compare_mtime(const void *a, const void *b);
int compare_dir_first(const void *a, const void *b);
void print_tree(fileinfo fileinfo,char *dirname,int level,int num_files,int index);
void tree(char *basePath, int depth, int sort_type);