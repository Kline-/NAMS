#ifndef DEC_CONFIG_H
#define DEC_CONFIG_H

/************************************************************
 *                      GAME OPTIONS                        *
 ***********************************************************/
// CFG_GAM_PULSE_RATE -- How many cycles per second should be processed.
//  Default: 100
#define CFG_GAM_PULSE_RATE  100

/************************************************************
 *                      MEMORY OPTIONS                      *
 ************************************************************/
// CFG_MEM_MAX_BITSET -- Maximum size of all bitset elements within the server.
//  Default: 128
#define CFG_MEM_MAX_BITSET  128

/************************************************************
 *                      SOCKET OPTIONS                      *
 ************************************************************/
// CFG_SOC_BIND_ADDR -- IP address to bind the listening server socket to.
//  Default: 0.0.0.0
#define CFG_SOC_BIND_ADDR   "0.0.0.0"

// CFG_SOC_MIN_PORTNUM -- The listening server socket must be above this port.
//  Default: 1024
#define CFG_SOC_MIN_PORTNUM 1024

// CFG_SOC_MAX_PENDING -- The maximum number of pending connections to allow in the listen backlog.
//  Default: 5
#define CFG_SOC_MAX_PENDING 5

// CFG_SOC_MAX_PORTNUM -- The listening server socket must be below this port.
//  Default: 65536
#define CFG_SOC_MAX_PORTNUM 65536

// CFG_SOC_PORTNUM -- Port number to listen on if not specified on the command line.
// Default: 4321
#define CFG_SOC_PORTNUM     4321

/************************************************************
 *                      STRING OPTIONS                      *
 ************************************************************/
// CFG_STR_LOGIN -- String sent on initial socket connection.
#define CFG_STR_LOGIN "\r\nFor a list of commands type 'help'.\r\nPlease enter your name: "

// CFG_STR_MAX_BUFLEN -- Maximum length of all char type buffers.
//  Default: 16384
#define CFG_STR_MAX_BUFLEN  16384

// CFG_STR_VERSION -- The current server version.
//  Default: NAMS 1.0.0
#define CFG_STR_VERSION     "NAMS 1.0.0"

#endif
