#include <stdio.h>
#include <cstring> //atoi
#include<iostream>
#include<vector>

#include "read_arg.cpp"
#include "multicol.cpp"

#define require(E,S) do if(!(E)) { printf(S); return -1; } while(0)
#define requirebr(E,S) do if(!(E)) { printf(S); break; } while(0)


#define MAXLINE			(2048)




//=================================================================================================================


int main(int argc, char ** argv)
{

  
  auto __print_args = [&argc, &argv]() {
    printf("FileName: %s,  argc=%d\n", argv[0], argc);
    printf("Command: |%s|\n=====================\n", argv[1]);
    for(auto i=2; i<argc; i++) {printf("%3d: |%8s|\n",i,argv[i]);}};

    
  if (arg_check(argc,argv,"--help"))
  {
	printf("\nMulticolumn output to console:   Reads from stdin, sends to stdout. Terminate with Ctrl-D.\n");
	printf("USAGE:\n\n");
	printf("./mcol	[--help] [par=val]\n");
	printf("cat <file> | ./mcol [par=val]\n\n");
	printf("Parameters:\n");
	printf("	 ncol=5		- num of columns\n");
	printf("	 style=2	- style: RAW,BARE,DENSE,AIR,MXinfo,MAXWIDTH, value 0,...,5\n");
	printf("	 wrap=0		- long lines: 0=wrap, 1=run, 2=trunc\n");
	printf("	 width=20	- set explicit width of columns, set also wrap=2\n");
	printf("	 verbose=0	- additional info: 0=no, 1, 2\n");

	return 1;
  }

  

  
//========= define object, set properties according to parameters


	Multicol prnt;
	char linebuff[MAXLINE];
	char *res_ptr;

// TERMINAL SIZE is now fixed, to improve

	int val_int;
	int ncol = 2;
	arg_read(argc,argv,"ncol=%d", &ncol);
	prnt.ncol = ncol;
	
	prnt.wrap_style = COL_WRAP;
	arg_read(argc,argv,"wrap=%d", &prnt.wrap_style);

	COL_STYLE styl = COL_AIR;
	arg_read(argc,argv,"style=%d", &styl);
	prnt.col_style = styl;

	prnt.verbose = 1;
	arg_read(argc,argv,"verbose=%d", &prnt.verbose);

	arg_read(argc,argv,"width=%d", &prnt.explicitwidth);

//=========  main loop  =================================
	
	if (prnt.verbose > 1)  __print_args();
	
	while( 1 )
	{
	    res_ptr = fgets(linebuff, MAXLINE, stdin);
	    if ( feof(stdin) || !res_ptr ) break;
	    prnt.add(linebuff);
	}
	
	prnt.pr_col_new(ncol);
//	prnt.pr();
	    
  return 0;
}
