#ifndef LIO_H_   /* Include guard */
#define LIO_H_
/*#define lio_buffer_size__ 5000
#define lio_field_size__    20

static char lio_buffer__[lio_buffer_size__*lio_field_size__]={'\0'}, lio_bsplit__[lio_buffer_size__*lio_field_size__], lio_delim__[256];
static char *lio_fields__[lio_buffer_size__];
static int lio_fields_len__=0, lio_debug__=1;// lio_delim_len__=0;
static FILE *lio_file__=0;*/

/*==> Function Declarations  <==========================================*/
int LioLogging(int level);

int LioSetDelim(const char* delim);

int LioCloseFile();

int LioOpenFile(const char* fname, const char* delim);

char* LioGetRecord();

int LioSplitRecord();

int LioFindTag(const char *tag);

char *LioGetChar(int n);

char **LioGetChars(int start);

double LioGetDouble(int n);

int LioGetDoubles(int start, int dsize, double *data);

int LioGetNFields();

#endif // LIO_H_
