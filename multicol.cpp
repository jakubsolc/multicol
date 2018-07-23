#ifndef MULTICOL
#define MULTICOL

// g++ multicol_runner.cpp -g -std=c++11 -o mcol

#include<iostream>
#include<fstream>
#include<vector>
#include<cstring>

#define TERMINALWIDTH		(140)
#define X
//#define CONCAT(A)		A ## A
#define DEBMC			if(0) 
#define DEBMCAPP		if(0)

#define OFFS			(4)				// v tuhle chvili je OFFS zbytecna hracka
//#define LN(A)			*(int*)(& A[0] )		// tuhle cestu jsem opustil,  lze smazat
//#define OFFS			(1)
//#define LN(A)			*(unsigned char*)(& A[0] )
//#define LN(A)			(*(unsigned char*)(A))		// terse verse

typedef enum {COL_RAW, COL_DENSE, COL_AIR, COL_WIDTH, COL_BARE, COL_WIDTH_VERBOSE}	COL_STYLE;
typedef enum {COL_WRAP, COL_NOWRAP, COL_TRUNC}						WRAP_STYLE;

class Multicol
{

public:
      int ncol;
      FILE *f;
      COL_STYLE col_style;
      WRAP_STYLE wrap_style;
      int explicitwidth;
      int verbose;
private:
      std::vector<char*> li;	// lines
      std::vector<int>   wid;	// lengths of lines
      int maxwidth;
      int n;			// count of true lines without metalines #DELETED etc
      char *runningbuff;
      int runninglen;

public:      
      Multicol();
      ~Multicol();
      
      void pr();
      void pr_col(int nc);
      void pr_col_new(int nc);
      int add( const char* &&s);
      int append( const char* &&s);
      int add_preallocated(char *ps);
      void prf(char fmt[], char s[]);
  
      void info();
private:
      int add_to_idx(int idx, const char* &&s);
      int remapline(int targetwidth, std::vector<int> &remap);
      char* fetch_segment(int idx, int targetwidth, std::vector<int> &remap);
};

Multicol::Multicol() :
	n(0), ncol(1), maxwidth(0), col_style(COL_AIR), wrap_style(COL_WRAP), verbose(1), explicitwidth(0)
{
DEBMC	printf("Konstruktor Multicol\n");
DEBMC	printf("===================\n");
	
	runningbuff = new char[TERMINALWIDTH+1+OFFS];
	runninglen = 0;

	f = NULL;
	char fnn[] = "TESTFILENAME.XXX";
	if (0) {
	  printf("Oteviram soubor '%s'.\n",fnn);
	  f = fopen(fnn, "w");
	}
	if (f == NULL) f = stdout;
DEBMC	fprintf(f, "DEBUG:  n=%d  vecsize=%ld  ncol=%d  maxwidth=%d\n", n, li.size(), ncol, maxwidth);
DEBMC	fprintf(f, "=======================================\n");
      
}

void Multicol::pr()
{
	fprintf(f, "=======================================\n");
	fprintf(f, "DEBUG:  n=%d  vecsize=%ld widsize=%ld ncol=%d  maxwidth=%d\n", n, li.size(), wid.size(), ncol, maxwidth);
	
	printf("Rows: %ld\n", li.size());
	printf("==================================\n");
	if (li.size()==0) printf("  *  *  *\n");
	for (int r=0; r < li.size(); r++)
	{
		printf("%4d:%4d | %s", r, wid[r], li[r]+OFFS);
		printf("\n");
	}
	printf("==================================\n");

}

void Multicol::prf( char fmt[], char s[])
{
//	fprintf(f, "DEBUG:  n=%d  vecsize=%ld  ncol=%d  maxwidth=%d\n", n, li.size(), ncol, maxwidth);
	fprintf(f, "N/A!!!!!!!!!!!!!!!!!!!!!\n");

	printf("Rows: %ld\n", li.size());
	printf("==================================\n");
	if (li.size()==0) printf("  *  *  *\n");
	for (int r=0; r < li.size(); r++)
	{
		printf("%4d:%4d | %s", r, wid[r], li[r]+OFFS);
		printf("\n");
	}
	printf("==================================\n");

}

// aktualne: ignoruje this->nc, bere argument nc
void Multicol::pr_col(int nc)
{
  
	if (nc > 40) printf("MAX NUM of COLS is set to 40.\n");
	if (nc <= 0 || nc > 40) return;
//	if (nc <= 0 || nc > 10) return;
DEBMC	fprintf(f, "DEBUG: PRCOL n=%d  vecsize=%ld  ncol=%d  maxwidth=%d\n", n, li.size(), ncol, maxwidth);

	
	// TODO : udelat si schema v poli, pak tisk
	// zatim privizorne aritmeika s indexy
	// TODO test sirky

//	int vyska = n / nc + 1;
	int vyska = li.size() / nc + 1;		// docasne
	int sirka;
	static const char delimtable[][4] = {"","|"," | "," | "};	// pozor sirka <=3
	static const char delimtablx[][3] = {"","|"," |"," |"};
	const char *delim, *delim_end;
	switch (col_style) {
	  case COL_RAW:
	    sirka = (TERMINALWIDTH) / nc;		// pocitam nadoraz
	    delim = delimtable[0];
	    delim_end = delimtablx[0];			// TODO lze indexovat najednou
	    break;
	  case COL_DENSE:
	    sirka = (TERMINALWIDTH-nc-1) / nc;		// pocitam vcetne '|'
	    delim = delimtable[1];
	    delim_end = delimtablx[1];
	    break;
	  case COL_AIR:
	    sirka = (TERMINALWIDTH-3*nc-1) / nc;	// pocitam vcetne ' | '
	    delim = delimtable[2];
	    delim_end = delimtablx[2];
	    break;
	  default:
	  case COL_WIDTH:
	    sirka = maxwidth;				// pocitam podle akt sirky '
   	    delim = delimtable[3];
	    delim_end = delimtablx[3];
	    break;
	  case COL_WIDTH_VERBOSE:
	    sirka = maxwidth;				// pocitam podle akt sirky '
   	    delim = delimtable[3];
	    delim_end = delimtablx[3];
	    verbose = 1;
	    break;
	  case COL_BARE:				// zatim identicke jako RAW
	    sirka = (TERMINALWIDTH) / nc;		// pocitam nadoraz
	    delim = delimtable[0];
	    delim_end = delimtablx[0];			// TODO lze indexovat najednou
	    break;
	}
	
	if (explicitwidth > 0) sirka = explicitwidth;
	
	char *ss, ssvoid[] = "  . . . .";
	int sswid=7;
	int k, r, id;
	
	if (verbose>1) {
	  printf("Rows: %ld\n", li.size());
	  printf("Rows true: %d\n", n);
	  printf("HEIGHT: %d\n", vyska);
	  printf("WIDTH:  %d\n", sirka);
	}
	if (verbose > 0) printf("==================================\n");
	if (li.size()==0) printf("  *  *  *\n");
	
	for (r = 0; r < vyska; r++)
	{
	    for (k = 0; k < nc; k++) {
	       id = k * vyska + r;
	       if (id>=li.size() || li[id] == NULL) 
	         {ss = ssvoid; sswid=7; }			//assume that col has at least 7 chars // TODO : is sswid used?
	       else
	         {ss = li[id] + OFFS; sswid=wid[id]; }
//	       printf("%s%*s%*s", delim, sirka, ss, sirka-sswid, "");
	       switch (wrap_style)
	       {
		 case COL_TRUNC:
		    printf("%s%*.*s", delim, -sirka, sirka, ss);
		    break;
		 default:
		 case COL_NOWRAP:
		    printf("%s%*s", delim, -sirka, ss);
		    break;
	       }
	    }
	    printf("%s\n", delim_end);
	}
	if (verbose > 0) printf("==================================\n");

}


Multicol::~Multicol()
{
DEBMC	fprintf(f, "Destruktor\n=======================================\n");
	if (f != stdout) 
	{
		fclose(f);
		printf("Data saved.\n");
	}
}




//	return:	if success, no of rows
//		if fail, -1
// 	removes crlf at end of the string
//	multiple line is added as one object containing \n
int Multicol::add(const char* &&s)
{
	char *ps;
	unsigned w = strlen(s);
	if (w>254) {printf("Too long string\n"); return -1;}
	ps = new char[w + 1 + OFFS];
	strcpy(ps + OFFS, s);
	if (ps[w - 1 + OFFS] == '\n') {ps[--w + OFFS] = '\0';}		// remove \n at end
DEBMC 	printf("Add: w=%d, s='%s'\n", w, ps+OFFS);
	li.push_back(ps);
	wid.push_back(w);
	if (w > maxwidth) maxwidth = w;
	n++;
	return n;
}

// clone of add
// appends to last line (similar to printf)
// removes crlf
// provisory: reallocates everytime.
int Multicol::append(const char* &&a)
{
    const char *p_start = a;
    const char *p_end;
//DEBMCAPP	    printf("APP-0 Enter Line %d: \n", __LINE__);
//DEBMCAPP	    printf("F Enter Line %d: \n", __LINE__);
    
    while (*p_start != '\0') 
    {
DEBMCAPP	printf("APP: runlen=%d, runn=[%s], s=[%s]\n",  runninglen, (runningbuff)? runningbuff+OFFS : "NULL", p_start);
	// determine length of next segment
	p_end = p_start;
	while( *p_end != '\0' && *p_end != '\n') p_end++; 
	int len = p_end - p_start;	// p_end points at terminal symbol, len=length w/o terminal
	
	// check output buffer
	if (runningbuff == NULL) 
	{
	  runningbuff = new char[TERMINALWIDTH+1+OFFS];			// BACHA na ty zavorky !!!
	  runninglen = 0;
	}
	
	int num_of_chars_to_write;
	int wrap;
	if (TERMINALWIDTH - runninglen -1 < len)     // reserve [0], at the end '$' '\0'
	{
	  num_of_chars_to_write = TERMINALWIDTH - runninglen -1;	//-1 because of wrappingchar 
	  wrap = 1;
	  printf("Line is too long: len=%d,  wrapping, writing:%d.\n", len, num_of_chars_to_write);
	} else {
	  num_of_chars_to_write = len;
	  wrap = 0;
	}
	int newlength = runninglen + num_of_chars_to_write;

DEBMCAPP	printf("AP2: towrite=%d, newlen=%d, wrap=%d\n", num_of_chars_to_write, newlength, wrap);
	// choose destination:  \n or wrapping means that line is finished
	if (*p_end == '\n' || wrap == 1)
	{
	  // if the line is long, use buff as new line
	  if (newlength > TERMINALWIDTH/2)
	  {
DEBMCAPP	    printf("Enter Case #1: \n");
	    strncpy( runningbuff + OFFS + runninglen, p_start, num_of_chars_to_write );
	    runningbuff[ OFFS + runninglen + num_of_chars_to_write + wrap] = '\0';
	    if (wrap == 1) runningbuff[OFFS + runninglen + num_of_chars_to_write + wrap -1] = '$';		//TODO pro jistotu o znak driv, pozdeji zkusit +1
	    //install into li
	    li.push_back(runningbuff);
	    wid.push_back(newlength + wrap);							// new: zkratil jsem delku num_of_chars_to_write o 1
	    if (newlength+wrap > maxwidth) maxwidth = newlength+wrap;
	    n++;
	    runningbuff = NULL;
	    runninglen = 0;
	  } else {
DEBMCAPP	    printf("Enter Case #2: -- add line to li, realloc\n");
	  // allocate
	  char *newline = new char[newlength+1+OFFS];	// [0], endchar
	  // zkopirujem zacatek
	    strncpy(newline + OFFS, runningbuff + OFFS, runninglen);
	  //zkopirujem novy segment do newline
	    strncpy( newline + OFFS + runninglen, p_start, num_of_chars_to_write );  
	  // pridame konec
	    newline[OFFS + newlength] = '\0';
	    li.push_back(newline);
	    wid.push_back(newlength);
	    if (newlength > maxwidth) maxwidth = newlength;
	    n++;
	    runninglen = 0;		//reset runningbuff
	  }
	} else {
DEBMCAPP	    printf("Enter Case #3 -- append->buff: \n");
	  //zkopirujem novy segment do buff
	    strncpy( runningbuff + OFFS + runninglen, p_start, num_of_chars_to_write );  
	  // pridame konec
	    runninglen += num_of_chars_to_write;
	    runningbuff[OFFS + runninglen] = '\0';	  
	}
DEBMCAPP	if (runningbuff) printf("ENDLOOP: runlen=%d, runn=[%s], s=[%s]\n",  runninglen, runningbuff+OFFS, p_start); else printf("ENDLOOP: %d NULL [%s]\n",runninglen,p_start);
	
	// update pointers for new cycle
	if ( wrap==0 ) 	p_start = p_end;
	else p_start += num_of_chars_to_write;
DEBMCAPP	    printf("ELOOP1 cond char=%d: \n", (int)*p_start);
	if (*p_start == '\n') p_start++;			// kdyz \n\0, neprojde dalsi cyklus
DEBMCAPP	    printf("ELOOP2 cond char=%d: \n", (int)*p_start);
    }
    return n;
}

//private
// deallocate, allocate
// idx < size !!!
// do not update n
int Multicol::add_to_idx(int idx, const char* &&s)
{

  	if (idx >= li.size()) return -1;
	if (li[idx] != NULL) delete[] li[idx];		//condition redundant
	unsigned w = strlen(s);
	if (w>254) {printf("Too long string\n"); return -1;}
	char *ps = new char[w+1+OFFS];	// 1 for \0, 1 for size in ps[0]
	strcpy(ps+OFFS,s);
DEBMC	printf("Addtoidx: idx=%d, w=%d, s='%s'\n", idx, w, ps+OFFS);
	li[idx] = ps;
	wid[idx] = w;
	if (w > maxwidth) maxwidth = w;
	return 0;
}

//	return:	if success, no of rows
//		if fail, -1
//	first char is size<=254, length is computed and stored(overwrite) to ps[0].
int Multicol::add_preallocated(char *ps)
{
	unsigned w = strlen(ps);
	if (w>254) {printf("Too long string\n"); return -1;}
DEBMC	printf("Add: w=%d, s='%s'\n", w, ps+OFFS);
	li.push_back(ps);
	wid.push_back(w);
	n++;
	return n;
}

/*
void Multicolsolu::deleterow(unsigned int idx)
{

	
  	if (idx >= li.size()) return;
	add_to_idx(idx, "#DELETED");
	// check maxwidth
	for (int r=0; r < li.size(); r++)
	{
	    if ( (w=strlen(li[r]+OFFS)) > maxwidth) maxwidth = w;
	}
	return;
}
*/


/*
int main()
{
  
  Multicol p;
  
  p.pr();
  
  char ss[] = "ABCDE";
  p.add(ss);
  p.add("ERTYU5e");
  p.add("QWEERT");
  p.add("POIUYTYT");
  p.add("QWEERT");
  p.add("ERTYU5e");
  p.add("POIUYTYT");
  p.add("QWEERT");
  p.add("ERTYU5e");
  p.add("POIUYTYT");
  p.add("QWEERT");
  p.add("ERTYU5e");
  p.add("POIUYTYT");
  p.add("QWEERT");
  p.add("QWEERT");
  p.add("ERTYU5e");
  p.add("POIUYTYT");
  p.add("QWEERT");
  p.add("ERTYU5e");
  p.add("POIUYTYT");
  p.add("QWEERT");
  
  p.pr();

  p.pr_col(4);
  
  return 0;

}

*/

#endif

int trueline(const char s[])
{ 
	if(s[0] == '#')	return false;
	return true;
}

//	cmd==0:		add \0 at the end of seg-th segment
//	cmd==1:		repair string

void repairstring(int cmd, char *ps, int totalwid, int segment, int targetwidth)
{
      static char savedchar;
      static int action = false;
      if (cmd == 0) 		// set
      {
	// is current line terminated \0 ?
	if ( totalwid < (segment+1) * targetwidth) 
	{
	  action = false;
	  return;
	}
	else 
	{
	  action = true;
	  ps += (segment+1) * targetwidth;	//char after the end of segment (space for \0 should be allocated)
	  savedchar = *ps;
	  *ps = '\0';
	  return;
	}
      }
      else
      {
			//repair
	if (action)
	{
		ps += (segment+1) * targetwidth;	//char after the end of segment (space for \0 should be allocated)
		*ps = savedchar;
		action = false;
	}
      }
}

// 	return NULL if idx out of range
// deactivated: routine MUST be followed by repair_string, we use %*.*s truncation
char* Multicol::fetch_segment(int idx, int targetwidth, std::vector<int> &remap)
{
      if (idx >= remap.size() || idx < 0) return NULL;
      int seg = 0;
      while( idx-seg >= 0 && remap[idx-seg-1] == remap[idx] ) seg++;
      
//      repair_string(0, li[remap[idx]], wid[remap[idx]], seg, targetwidth);
      return li[ remap[idx] ] + OFFS + targetwidth * seg ;
}

int Multicol::remapline(int targetwidth, std::vector<int> &remap)
{
	int k;
	
	for (int idxr = 0; idxr < li.size(); idxr++)
	{

		if ( trueline( li[idxr] + OFFS) )
		{
			remap.push_back(idxr); 
			if( wrap_style == COL_WRAP && wid[idxr] > targetwidth)
			{
				k = wid[idxr] / targetwidth;
				while (k--) remap.push_back(idxr); 
			}
		}
	}

	return remap.size();
}

// aktualne: ignoruje this->nc, bere argument nc
void Multicol::pr_col_new(int nc)
{
  
	if (nc > 40) printf("MAX NUM of COLS is set to 40.\n");
	if (nc <= 0 || nc > 40) return;
//	if (nc <= 0 || nc > 10) return;

DEBMC	fprintf(f, "DEBUG: PRCOL n=%d  vecsize=%ld  ncol=%d  maxwidth=%d\n", n, li.size(), ncol, maxwidth);

	if (verbose > 0) printf("==================================\n");
	if (li.size() == 0) printf("  *  *  *\n");

	int vyska;
	int sirka;
	int k, r, id;

	static const char delimtable[][4] = {"","|"," | "," | "};	// pozor sirka <=3
	static const char delimtablx[][3] = {"","|"," |"," |"};
	char  ssvoid[] = "  . . . .";
	const char *delim, *delim_end;

	switch (col_style) {
	  case COL_RAW:
	    sirka = (TERMINALWIDTH) / nc;		// pocitam nadoraz
	    delim = delimtable[0];
	    delim_end = delimtablx[0];			// TODO lze indexovat najednou
	    break;
	  case COL_DENSE:
	    sirka = (TERMINALWIDTH-nc-1) / nc;		// pocitam vcetne '|'
	    delim = delimtable[1];
	    delim_end = delimtablx[1];
	    break;
	  case COL_AIR:
	    sirka = (TERMINALWIDTH-3*nc-1) / nc;	// pocitam vcetne ' | '
	    delim = delimtable[2];
	    delim_end = delimtablx[2];
	    break;
	  default:
	  case COL_WIDTH:
	    sirka = maxwidth;				// pocitam podle akt sirky '
   	    delim = delimtable[3];
	    delim_end = delimtablx[3];
	    break;
	  case COL_WIDTH_VERBOSE:
	    sirka = maxwidth;				// pocitam podle akt sirky '
   	    delim = delimtable[3];
	    delim_end = delimtablx[3];
	    verbose = 1;
	    break;
	  case COL_BARE:				// zatim identicke jako RAW
	    sirka = (TERMINALWIDTH) / nc;		// pocitam nadoraz
	    delim = delimtable[0];
	    delim_end = delimtablx[0];			// TODO lze indexovat najednou
	    break;
	}
	
	
	if (explicitwidth > 0) sirka = explicitwidth;
	
	

	//new-----------------------------------------

	std::vector<int>remap;
	char 		*ps;
	int 		truerowcount = remapline(sirka, remap);

	vyska = truerowcount / nc + 1;
	if (verbose > 1) 
	{
	    printf("Rows: %ld\n", li.size());
	    printf("Rows n: %d\n", n);
	    printf("Rows true: %d\n", truerowcount);
	    printf("HEIGHT: %d\n", vyska);
	    printf("WIDTH:  %d\n", sirka);
	}
	
	for (r = 0; r < vyska; r++)
	{
	      for (k = 0; k < nc; k++)
	      {
		  id = k * vyska + r;
		  ps = fetch_segment(id, sirka, remap);
		  if (ps == NULL ) ps = ssvoid;
	   
	
		  switch (wrap_style)
		  {
		      case COL_WRAP:
		      case COL_TRUNC:
			printf("%s%*.*s", delim, -sirka, sirka, ps);
//			repair_string(0, li[remap[idx]], wid[remap[idx]], seg, targetwidth);
			break;
		      default:
		      case COL_NOWRAP:
			printf("%s%*s", delim, -sirka, ps);
			break;
		  }
	      } // for k
	      printf("%s\n", delim_end);
	} //for r
//-----------------------------------------
	if (verbose > 0) printf("==================================\n");
}

