

// declarations, defaults
//------------------------------
int arg_read(int argc, char **argv, const char s[], void *ptr, int limitAlloc=80);
// verse 0.2	DEB-->DEB_ARGS
//		lib pocet znaku pred '='
//		matchs the last occurence of par
//		hashes are still not used, although a prepared routine is below 
//------------------------------
// returns value 1:success, 0:arg not found

//------------------------------
void arg_show(int argc, char** argv);
//------------------------------

//------------------------------
int arg_check(int argc, char **argv, const char s[]);
// RETURN VALUE:
//	0 	if string s not found
//	idx>0	if string is found, index of arg
//
// 	does not check filename argv[0]
//------------------------------


//------------------------------
// Jak debugovat:
// 	uncomment next line and insert before #include
// 	#define DEB_ARGS 	if (1)


//----   E X A M P L E   o f   U S A G E  --------------------
//  int debug=0;					// preset default value
//  read_arg(argc, argv,"-debug=%d", &debug);		// if par is not found, value remains unchanged
//  read_arg(argc, argv,"column%d", &debug);		// '=' and initial '-' may be omitted
//  read_arg(argc, argv,"-fname=%s", fname, sizeof(fname));
//  read_arg(argc, argv,"-fl=%lf", &a);
//  // fname preallocated with char fname[limitAlloc]
//------------------------------------------------------------



#ifndef DEBUG_ARGS
#define DEBUG_ARGS 	if (0)
//static int debug = 0;		//scope=file
#endif


// format: .+=N?
// input: zeroterminated string 
// returns number of chars before '='
// if '%' not found, returns -1
// if string begins 0, returns 0
static int find_procento(const char *ps)
{
//  const 
  int counter = 0;
  while(*ps != '\0') {
    if (*ps == '%') return counter;
    counter++;
    ps++;
  }
  return -1;
}

// input: zeroterminated string starting after %
// returns 'd', 'f', 's'
// if type not found, returns \0
// does check, whether float 'f'=>'F' or double 'lf'=>'f'
static char find_typechar(const char *ps)
{
//  const 
  int counter = 0;
  int lflag = 0;
  while(*ps != '\0') {
    if (*ps == 'd' || *ps == 's') return *ps;
    if (*ps == 'l' ) lflag = 1;
    if (*ps == 'f' ) return (lflag==0) ? *ps : 'F';
    ps++;
  }
  return '\0';
}

int arg_read(int argc, char **argv, const char s[], void *ptr, int limitAlloc)
{
  int patternlength, currlen, res, success;
  int arg_int;
  char arg_type;
  double arg_double;

  patternlength = find_procento(s);
  if (patternlength<0) {std::cout << "readpar: Pattern does not contain '='!" << std::endl; return 0; }  
  arg_type = find_typechar(s + patternlength);
  if (arg_type == '\0') {std::cout << "readpar: Pattern does not contain type 'd','f','s'!" << std::endl; return 0; }  
  arg_type = find_typechar(s + patternlength);
  if (arg_type == 'F') {std::cout << "readpar: float 'f' with single float,implemented is only 'lf'!" << std::endl; return 0; }  
  
  while(--argc > 0)
  {
//    cout <<argv[argc]<<" : " << s << "  Parlen="<<parlen<<endl;
    if (strncmp(argv[argc],s,patternlength) == 0)
    {
      switch (arg_type)
      {
	case 'd':
	    res = sscanf(argv[argc], s, &arg_int);
	    if (res == 1) { *(int*)ptr = arg_int; success = 1; }
DEBUG_ARGS  std::cout << s << argv[argc] << " :: " << *(int*)ptr << std::endl;
	    break;
	case 'f':
	    res = sscanf(argv[argc], s, &arg_double);
	    if (res == 1) { *(double*)ptr = arg_double; success = 1; }
DEBUG_ARGS  std::cout << s << argv[argc] << " :: " << *(double*)ptr << std::endl;
	    break;
	case 's':
	    currlen = strlen(argv[argc]);
	    if ( (currlen - patternlength +1) > limitAlloc)	// true length + '\0' > alloc
		std::cout << "ERR: Prilis dlouhy parametr char[]: " << argv[argc] << std::endl;
	    else
	    {
	      res = sscanf(argv[argc], s, ptr);
	      success = 1; 
DEBUG_ARGS    std::cout << s << argv[argc] << " :: " << (char*)ptr << std::endl;
	    }
	    break;
      }
      if (success) return success;
    }
  }
  return 0;
}


// RETURN VALUE:
//	0 	if string s not found
//	idx>0	if string is found, index of arg
// does not check filename argv[0]
int arg_check(int argc, char **argv, const char s[])
{
 
  while(--argc > 0)	//dont check [0]
  {
    if (strcmp(argv[argc], s) == 0)
      return argc;
  }
  return 0;
}

void arg_show(int argc, char** argv)
{
    std::cout << "Argc=" <<argc << std::endl;
    std::cout << "==== A R G S : ================\n";
    for (int k=0; k<argc; k++)
      std::cout << k << ": [" << argv[k] << "]" << std::endl;
    std::cout << "===============================\n";
}

// not used
static int hash(const char* &&s)
{
   int crc = 0;
   const char *ps = s;
   while(*ps != '\0') { crc = 2*crc + (*ps++); }
   return crc;
}
