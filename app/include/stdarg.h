/*
 *This file is important for functions as fun(para, ...), printf() forexample.
 *mostly copied from linux-0.12/include/stdarg.h
*/
#define va_rounded_size(TYPE)  \
  ((sizeof(TYPE)+sizeof(int)-1)/sizeof(int)*sizeof(int))
/*
 *in:
 *AP is a whole new pointer
 *LASTARG is current parameter
 *out:
 *AP is next parameter's pointer
*/
#define va_start(AP,LASTARG)  \
  (AP=(char *)&LASTARG+va_rounded_size(LASTARG))
/*
 *in:
 *AP is current parameter's pointer
 *TYPE is current parameter's type
 *out:
 *AP is next parameter's pointer
 *current parameter's value 
*/
#define va_arg(AP,TYPE)  \
  (AP=AP+va_rounded_size(TYPE),  \
   * ((TYPE *) (AP-va_rounded_size(TYPE))))
