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
#include "myTree.h"

//initializes global variables
int active_features [n_features] = {0};
int countdir = 0;
int countfiles = 0;
int max_depth = 1000;

void help(){
		FILE *fp;
		char ch;
		fp = fopen("help.txt", "r");
		if (fp == NULL) {
			printf("Errore nell'apertura del file.\n");
			exit(1);
		}
		while ((ch = fgetc(fp)) != EOF) {
			printf("%c", ch);
		}
		fclose(fp);
}

void flagChecker(int argc, char* argv[]){
    int opt;
	int long_index;
	const char *short_options = "hadfpsugDrtL:";
    const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"inodes", no_argument, NULL, 0},
        {"dirsfirst", no_argument, NULL, 0},
        {NULL, 0, NULL, 0}};
	

    while ((opt = getopt_long(argc, argv, short_options, long_options, &long_index)) != -1) {
		switch (opt)
		{
		case 'h':
			active_features[0] = 1;
			break;
		case 'a':
			active_features[1] = 1;
			break;
		case 'd':
			active_features[2] = 1;
			break;
		case 'f':
			active_features[3] = 1;
			break;
		case 'p':
			active_features[4] = 1;
			break;
		case 's':
			active_features[5] = 1;
			break;
		case 'u':
			active_features[6] = 1;
			break;
		case 'g':
			active_features[7] = 1;
			break;
		case 'D':
			active_features[8] = 1;
			break;
		case 0:
			// Handle long options
			if (strcmp("inodes", long_options[long_index].name) == 0)
			{
				active_features[9] = 1;
			}
			else if (strcmp("dirsfirst", long_options[long_index].name) == 0)
			{
				active_features[12] = 1;
			}
			break;
		case 'r':
			active_features[10] = 1;
			break;
		case 't':
			active_features[11] = 1;
			break;
		case 'L':
			active_features[13] = 1;
			max_depth = atoi(optarg);
			break;
		case '?':
			active_features[0] = 1;
			break;
		}
    }
	if (optind < argc) {
		dirPath = argv[optind];
    }
}

char* convert_time(int tempo_unix){
		char *final_date =  (char*)calloc(1000,sizeof(char));
		time_t seconds_since_epoch = tempo_unix;
		struct tm *local_time = localtime(&seconds_since_epoch);
		strftime(final_date,1000,"%d-%m-%Y %H:%M:%S", local_time);
		return final_date;
	  }

void numeric_to_string_mode(mode_t mode, char* buf) {
    const char chars[] = "rwxrwxrwx";
    for (size_t i = 0; i < 9; i++) {
        buf[i] = (mode & (1 << (8 - i))) ? chars[i] : '-';
    }
    buf[9] = '\0';
}
//compare name file/directory and sort them alpha
int compare_name_reverse(const void *a, const void *b) {
    return strcmp(((fileinfo*)b)->name, ((fileinfo*)a)->name);
}
//compare name file/directory and sort them reverse alpha
int compare_name(const void *a, const void *b) {
    return strcmp(((fileinfo*)a)->name, ((fileinfo*)b)->name);
}
//compare name file/directory and sort them by last mod
int compare_mtime(const void *a, const void *b) {
    time_t mtime_a = ((fileinfo*)a)->mtime;
    time_t mtime_b = ((fileinfo*)b)->mtime;
    if (mtime_a > mtime_b) return -1;
    if (mtime_a < mtime_b) return 1;
    return 0;
}
//compare name file/directory and sort them with dirs first
int compare_dir_first(const void *a, const void *b) {
    int is_dir_a = ((fileinfo*)a)->is_dir;
    int is_dir_b = ((fileinfo*)b)->is_dir;
    if (is_dir_a && !is_dir_b) return -1;
    if (!is_dir_a && is_dir_b) return 1;
    return compare_name(a, b);
}
//print the resulted tree with the relative identation
void print_tree(fileinfo fileinfo,char *dirname,int level,int num_files,int index){
	//inizializ. varie
    char percorso[10000] = {""};
	char status[10000] = {""};
	int flag_d = 0;
	int sum = 0;
	char name_bold[50];

	//controllo profondità
	if(level > max_depth){
		return;
	}
	if(fileinfo.is_dir){
		countdir++;
	}
	else{
		//conto i file . solo quando -a attivo
		if(strncmp(fileinfo.name, ".",1) == 0 && active_features[1] == 1){
			countfiles++;
		}
		//conto tutti i file sempre
		if (strncmp(fileinfo.name, ".",1) != 0)
		{
			countfiles++;
		}
		
	}
	//controlli speciali
	
	if (active_features[1] == 1 && active_features[3] == 1){
		active_features[1] = 0;
	}
	
	if (active_features[2] == 1 && active_features[3] == 1){
		//abilita caso speciale per -d
		flag_d =1;
	}
	for(int j = 4; j < 10; j++){
		if(active_features[j]==1){
			sum++;
		}
	}
	if (sum>=1 && active_features[1]+active_features[2]==0){
		active_features[14] = 1;
	}
	if (active_features[1] == 0 && active_features[2] == 0 && active_features[3] == 0){
		active_features[14] = 1;	
	}
	//controllo delle features...	

	if(fileinfo.is_dir){
    	sprintf(name_bold, "\033[1;34m%s\033[0m", fileinfo.name);
	}

	//crea path assouluto per gestire la stat fuori dalla directory corrente
	strcat(percorso,dirname);
	strcat(percorso,"/");
	strcat(percorso,fileinfo.name);

	//features stat
	if (sum >= 1){
		if (active_features[1] == 0 && strncmp(fileinfo.name, ".",1) == 0)
		{
			return;
		}
		
		if (!(active_features[2]==1 && fileinfo.is_dir == 0)){
			strcat(status,"[ ");
			
			//permissions
			if(active_features[4] == 1){
				strcat(status,fileinfo.permissions);
				strcat(status," ");
			}
			
			//size
			if(active_features[5] == 1){
				char s[100];
				sprintf(s, "%d", fileinfo.size);
				strcat(status,s);			
				strcat(status," ");
			}
			
			//uid
			if(active_features[6] == 1){
				char uid[20];
    			struct passwd *pwd = getpwuid(fileinfo.user);
				sprintf(uid, "%d", fileinfo.user);
				strcat(status,pwd->pw_name);
				strcat(status,"/");
				strcat(status,uid);			
				strcat(status," ");
			}
			
			//gid
			if(active_features[7] == 1){
				char gid[20];
				struct group *grp = getgrgid(fileinfo.group);
				sprintf(gid, "%d", fileinfo.group);
				strcat(status,grp->gr_name);	
				strcat(status,"/");
				strcat(status,gid);		
				strcat(status," ");
			}
			
			//last mod
			if(active_features[8] == 1){
				strcat(status,fileinfo.last_mod);			
				strcat(status," ");
			}
			
			//inode
			if(active_features[9] == 1){
				char i[100];
				sprintf(i, "%d", fileinfo.inode);	
				strcat(status,i);			
				strcat(status," ");
			}
			strcat(status,"] ");
				
		}
	}
	//caso speciale -a == 0 && -d == 0
	if (active_features[14] == 1){	
		if (fileinfo.is_dir){
			// [name directory]
			strcat(status," ");
			strcat(status,name_bold);
			strcat(status,"");
		}
		else
		{

			if (strncmp(fileinfo.name, ".",1) != 0)
			{
				strcat(status," ");
				strcat(status, fileinfo.name);
			}
		}
	}

	//feature -a	(mutu. eclusivo con -d) 
	if (active_features[1] == 1){	
		active_features[2] = 0;
		if (fileinfo.is_dir){
			// [name directory]
			strcat(status," ");
			strcat(status,name_bold);
			strcat(status,"");
		}
		else{
			strcat(status," ");
			strcat(status,fileinfo.name);				
			}
		}
		
		
	//feature -d	(mutu. eclusivo con -a) solo le directory
	if(active_features[2] == 1 && flag_d == 0){	
		if(fileinfo.is_dir){
			strcat(status," ");
			strcat(status,name_bold);
			strcat(status,"");
		}
	}
	
	
	//feature -f stampa path assoluto associato all'entita'
	if(active_features[3] == 1 && active_features[2] == 0){
		if (fileinfo.is_dir){
			// [name directory]
			strcat(status," ");
			strcat(status,dirname);
			strcat(status,name_bold);
			strcat(status,"");
			}
		else{
			strcat(status,"");
			strcat(status,dirname);
			strcat(status,"/");
			if(strncmp(fileinfo.name, ".",1) == 0 && active_features[1] == 1){
				strcat(status,fileinfo.name);
			}
			else if(strncmp(fileinfo.name, ".",1) != 0){
				strcat(status,fileinfo.name);
			}
		}
	}
	
	
	// caso speciale -f -d
	if(flag_d == 1){
		if (fileinfo.is_dir){
			// [name directory]
			strcat(status," ");
			strcat(status,dirname);
			strcat(status,name_bold);
			strcat(status,"");
			}
	}
	

	if(strcmp(status,"")!=0){
    	for (int j = 0; j < level - 1; j++) {
            printf("│   ");
        }
        if (level > 0) {
            if (index == num_files - 1) {
                printf("└──");
            } else {
                printf("├──");
            }
            for (int j = 0; j < level - 1; j++) {
                printf("──");
            }
        }
		printf("%s\n",status);
	}
	
}

void tree(char *basePath, int depth, int sort_type) {
    fileinfo *files = NULL;
    int num_files = 0;
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            char path[1000];
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

			if (dp->d_type == DT_DIR && strncmp(dp->d_name,".",1) == 0 && active_features[1] == 0){
            	continue;
			}
            struct stat stbuf;
            stat(path, &stbuf);

            //last mod 
            char* date;
			int last_mod = stbuf.st_mtime;
			date = convert_time(last_mod);

            //permissions
            char permessions[20];
			numeric_to_string_mode(stbuf.st_mode & 0777, permessions);


            fileinfo info = {
                .name = strdup(dp->d_name),
                .size = stbuf.st_size,
                .user = stbuf.st_uid,
                .group = stbuf.st_gid,
                .inode = stbuf.st_ino,
                .last_mod = date,
                .permissions = permessions,
                .mtime = stbuf.st_mtime,
                .is_dir = S_ISDIR(stbuf.st_mode),
            };
            files = realloc(files, sizeof(fileinfo) * (num_files + 1));
            files[num_files++] = info;
        }
    }

    closedir(dir);

    switch (sort_type) {
        case 0:
            qsort(files, num_files, sizeof(fileinfo), compare_name_reverse);
            break;
        case 1:
            qsort(files, num_files, sizeof(fileinfo), compare_mtime);
            break;
        case 2:
            qsort(files, num_files, sizeof(fileinfo), compare_dir_first);
            break;
        default:
            break;
    }
    
    for (int i = 0; i < num_files; i++) {
        
        print_tree(files[i],basePath,depth,num_files,i);

      

        //####################################################
        if (files[i].is_dir) {
            char path[1000];
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, files[i].name);
            tree(path, depth + 1, sort_type);
        }
        if(files[i].is_dir == 0){
        }
        
        free(files[i].name);
    }

    free(files);
}

