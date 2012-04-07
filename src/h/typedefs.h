#ifndef DEC_TYPEDEFS_H
#define DEC_TYPEDEFS_H

#if __WORDSIZE == 64
 typedef signed long int sint_t;
 #define sintmax_t numeric_limits<signed long int>::max()
 #define sintmin_t numeric_limits<signed long int>::min()

 typedef unsigned long int uint_t;
 #define uintmax_t numeric_limits<unsigned long int>::max()
 #define uintmin_t numeric_limits<unsigned long int>::min()
#else
 typedef signed int sint_t;
 #define sintmax_t numeric_limits<signed int>::max()
 #define sintmin_t numeric_limits<signed int>::min()

 typedef unsigned int uint_t;
 #define uintmax_t numeric_limits<unsigned int>::max()
 #define uintmin_t numeric_limits<unsigned int>::min()
#endif

class Command;
class Directory;
class Server;
class Socket;

#endif
