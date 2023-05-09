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

int sort = 0;
char *dirPath = ".";


int main(int argc, char **argv) {
    
    //controllo features attivate
    flagChecker(argc,argv);

    //printf("\n%d",max_depth);

    //controllo speciale per help()
    if(active_features[0]==1){
		help();
		return 0;
	}

    //controllo speciale per il sorting
    if(active_features[10]==1){
		sort = 0;
	}
    if(active_features[11]==1){
		sort = 1;
	}
    if(active_features[12]==1){
		sort = 2;
	}

	printf("\033[1;34m%s\033[0m\n",dirPath);
	printf("│   \n");

  tree(dirPath, 1, sort); // sort by name by default

  printf("\n%d directories, %d files \n", countdir, countfiles);

  return 0;
}