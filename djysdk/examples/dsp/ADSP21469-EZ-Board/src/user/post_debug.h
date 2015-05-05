#ifndef __POST_DEBUG_H__
#define __POST_DEBUG_H__


/**************************************************************
 * defining one of these methods will enabling debug printing *
 **************************************************************/
/* #define __DEBUG_FILE__ */ 	/* prints are directed to file __DEBUG_FILE_NAME__ */
#define __DEBUG_UART__			/* prints are directed to the UART */
/* #define __DEBUG_VDSP__ */	/* prints are directed to the VDSP console window (MUCH SLOWER!!!) */
/**************************************************************/

#define DO_CYCLE_COUNTS		/* used in cycles.h, do not rename this define - calculate cycle counts for tests */

#define HEX32   0x%04X

/* if debug printing is enabled */
#if defined(__DEBUG_FILE__) || defined(__DEBUG_UART__) || defined(__DEBUG_VDSP__)

	#include <stdio.h>

	/* if printing to a file */
    #if defined(__DEBUG_FILE__)
        #undef _PRINT_CYCLES
        #define __DEBUG_FILE_NAME__ "post_debug.txt"	/* output file name */
        #define DEBUG_STREAM pDebugFile
        #define  _PRINT_CYCLES(_STRG, _DAT)   fprintf(DEBUG_STREAM,"%s%llu\n", (_STRG), (_DAT)) __TRAILING_SC__
        #define DEBUG_CLOSE()	fclose(DEBUG_STREAM)
        #define DEBUG_PRINT(fmt,...) fprintf(DEBUG_STREAM,fmt, __VA_ARGS__);
    	#define DEBUG_STATEMENT(statement) fprintf(DEBUG_STREAM, statement);

	/* else if printing to the UART */
	#elif defined(__DEBUG_UART__)
		#define UART_DEBUG_BUFFER_LINE_SIZE 256
		extern char UART_DEBUG_BUFFER[];
		int UART_DEBUG_PRINT(void);
		#define DEBUG_PRINT(fmt,...)		snprintf(UART_DEBUG_BUFFER, UART_DEBUG_BUFFER_LINE_SIZE, fmt, __VA_ARGS__); \
											UART_DEBUG_PRINT();
    	#define DEBUG_STATEMENT(statement)	snprintf(UART_DEBUG_BUFFER, UART_DEBUG_BUFFER_LINE_SIZE, statement); \
    										UART_DEBUG_PRINT();

	/* else printing to VDSP console window */
    #elif defined(__DEBUG_VDSP__)
        #define DEBUG_STREAM stdout
        #define DEBUG_CLOSE
        #define DEBUG_PRINT(fmt,...) fprintf(DEBUG_STREAM,fmt, __VA_ARGS__);
    	#define DEBUG_STATEMENT(statement) fprintf(DEBUG_STREAM, statement);
    #endif

/* else debug printing not defined */
#else

	#ifdef DO_CYCLE_COUNTS
        #include <stdio.h>
    #endif

    #undef  __DEBUG_FILE__
    #define DEBUG_PRINT(fmt,...)
    #define DEBUG_STATEMENT(statement)
#endif

/* every method can use these defines */

#define DEBUG_HEADER(header)  		DEBUG_STATEMENT("\n\n"); \
   									DEBUG_STATEMENT("\n********************************************"); \
                               		DEBUG_STATEMENT("\n*** "); \
                               		DEBUG_STATEMENT(header); \
                               		DEBUG_STATEMENT("\n********************************************");
#define DEBUG_SUBHEADER(subheader)	DEBUG_STATEMENT("\n\n"); \
									DEBUG_STATEMENT("\n----------------------------------------"); \
                               		DEBUG_STATEMENT("\n--- "); \
                               		DEBUG_STATEMENT(subheader); \
                               		DEBUG_STATEMENT("\n----------------------------------------");
#define DEBUG_SUBHEADER2(subheader2)DEBUG_STATEMENT("\n\n"); \
									DEBUG_STATEMENT("\n----------------------------"); \
                               		DEBUG_STATEMENT("\n- "); \
                               		DEBUG_STATEMENT(subheader2); \
                               		DEBUG_STATEMENT("\n----------------------------");

#define DEBUG_RESULT(result,pass,fail)		if(result)\
											{DEBUG_STATEMENT(pass);} \
											else{DEBUG_STATEMENT(fail);}

#endif /* __POST_DEBUG_H__ */
