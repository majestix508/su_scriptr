#ifndef __MAIN__
#define __MAIN__ 1
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <map>
#include <vector>
#include <string>
#include "CScript.h"

using namespace std;

map<uint8_t,string> g_cmds;

void printHelp() {
	printf("scriptr Help\n");
	printf("-s [filename]           Parse a SU File\n");
	printf("-j [filename]           Parse a JSON File\n");
	printf("-o [filename]           Output filename\n");
	printf("-p                      Print the content of the script to stdout\n");
	printf("-c                      Check  the Fletcher Checksum for the script provided with -s or -j\n");
	printf("-f                      Create the Fletcher checksum for the script provided with -s or -j\n");
}

int main(int argc, char** argv){

    initCMDMap();

	int cflag = 0;
	int fflag = 0;
	int pflag = 0;
	string s_val, j_val, o_val;
	int c;

	opterr = 0;

    while( (c = getopt (argc, argv, "s:j:o:cfp")) != -1)
    {
    	switch (c)
    	{
    		case 's':
    			s_val = optarg;
    		    break;
    		case 'j':
    			j_val = optarg;
    		    break;
    		case 'o':
    			o_val = optarg;
    			break;
    		case 'c':
    			cflag=1;
    			break;
    		case 'f':
    			fflag = 1;
    			break;
    		case 'p':
    		    pflag = 1;
    		    break;
    		case '?':
    		    if (optopt == 's' || optopt == 'j' || optopt == 'o')
    			    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
    			else if (isprint (optopt))
    				fprintf(stderr, "Unknown option `-%c`.\n", optopt);
    			else
    				fprintf(stderr,"Unknown option character `\\x%x` .\n",optopt);
    			return 1;
    		default:
    			printHelp();
    		    abort ();
    	}
    }

    if (s_val.empty() && j_val.empty()){
    	printf("Please provide a input file with -s or -j\n\n");
    	printHelp();
    	return 1;
    }
    
    if (o_val.empty() && (cflag==0 && fflag==0 && pflag==0)){
        printf("Please provide a flag or a outputfile\n\n");
        printHelp();
    	return 1;
    }

    CScript l_script;

    if (!s_val.empty()){
        l_script.ParseSU((char *)s_val.c_str());
    }

    if (!j_val.empty()){
    	l_script.ParseJSON((char *)j_val.c_str());
    }

    if (!o_val.empty()){
    	l_script.WriteToFile((char *)o_val.c_str());
    	exit(0);
    }
    
    if (fflag){
        l_script.PrintChecksum();
    	exit(0);
    }

    if (cflag){
    	l_script.PrintChecksum();
    	if (l_script.CheckChecksum()){
    		printf("Valid Checksum!\n");
    	}
    	else {
    		printf("Invalid Checksum!\n");
    	}
    }

    if (pflag){
    	l_script.Print();
    }

    return 0;
}

#endif