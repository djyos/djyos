/*
 * main.c -- Main program for the GoAhead WebServer (eCos version)
 *
 * Copyright (c) Go Ahead Software Inc., 1995-2010. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 */

/******************************** Description *********************************/

/*
 *  Main program for for the GoAhead WebServer. This is a demonstration
 *  main program to initialize and configure the web server.
 */

/********************************* Includes ***********************************/

#include  "../uemf.h"
#include  "../wsIntrn.h"

#include  <sys/socket.h>

/*********************************** Locals ***********************************/
/*
 *  Change configuration here
 */

static char_t   *password = T("");  /* Security password */
static int      port = 80;          /* Server port */
static int      retries = 5;        /* Server port retries */
static int      finished;           /* Finished flag */

/****************************** Forward Declarations **************************/

static int  initWebs();
static int  aspTest(int eid, webs_t wp, int argc, char_t **argv);
static void formTest(webs_t wp, char_t *path, char_t *query);
static int  websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
                int arg, char_t* url, char_t* path, char_t* query);

#ifdef B_STATS
#error WARNING:  B_STATS directive is not supported in this OS!
#endif

/*********************************** Code *************************************/
/*
 *  Main -- entry point from eCos
 */

int webservermain(int argc, char** argv)
{
/*
 *  Initialize the memory allocator. Allow use of malloc and start 
 *  with a 60K heap.  For each page request approx 8KB is allocated.
 *  60KB allows for several concurrent page requests.  If more space
 *  is required, malloc will be used for the overflow.
 */
    bopen(NULL, (60 * 1024), B_USE_MALLOC);

/*
 *  Initialize the web server
 */
    if (initWebs() < 0) {
        return -1;
    }

/*
 *  Basic event loop. SocketReady returns true when a socket is ready for
 *  service. SocketSelect will block until an event occurs. SocketProcess
 *  will actually do the servicing.
 */
    while (!finished) {
        if (socketReady(-1) || socketSelect(-1, 2000)) {
            socketProcess(-1);
        }
        emfSchedProcess();
    }

/*
 *  Close the socket module, report memory leaks and close the memory allocator
 */
    websCloseServer();
    socketClose();
    bclose();
    return 0;
}

/******************************************************************************/
/*
 *  Initialize the web server.
 */

int initWebs()
{
    char        host[128]= "DJYOS_STACK";
    char        *cp;
    char_t      wbuf[128];

    struct in_addr  addr;
/*
 *  Initialize the socket subsystem
 */
    socketOpen();

/*
 *  Configure the web server options before opening the web server
 */
    websSetDefaultDir("/www");
    addr.s_addr = INADDR_ANY;
    cp = inet_ntoa(addr);
    ascToUni(wbuf, cp, min(strlen(cp) + 1, sizeof(wbuf)));
    websSetIpaddr(wbuf);
    ascToUni(wbuf, host, min(strlen(host) + 1, sizeof(wbuf)));
    websSetHost(wbuf);

/*
 *  Configure the web server options before opening the web server
 */
    websSetDefaultPage(T("home.htm"));
    websSetPassword(password);

/* 
 *  Open the web server on the given port. If that port is taken, try
 *  the next sequential port for up to "retries" attempts.
 */
    websOpenServer(port, retries);

/*
 *  First create the URL handlers. Note: handlers are called in sorted order
 *  with the longest path handler examined first. Here we define the security 
 *  handler, forms handler and the default web page handler.
 */
    websUrlHandlerDefine(T(""), NULL, 0, websSecurityHandler, 
        WEBS_HANDLER_FIRST);
    websUrlHandlerDefine(T("/goform"), NULL, 0, websFormHandler, 0);
    websUrlHandlerDefine(T(""), NULL, 0, websDefaultHandler, 
        WEBS_HANDLER_LAST); 

/*
 *  Now define two test procedures. Replace these with your application
 *  relevant ASP script procedures and form functions.
 */
    websAspDefine(T("aspTest"), aspTest);
    websFormDefine(T("formTest"), formTest);

/*
 *  Create a handler for the default home page
 */
    websUrlHandlerDefine(T("/"), NULL, 0, websHomePageHandler, 0); 
    return 0;
}

/******************************************************************************/
/*
 *  Test Javascript binding for ASP. This will be invoked when "aspTest" is
 *  embedded in an ASP page. See web/asp.asp for usage. Set browser to 
 *  "localhost/asp.asp" to test.
 */

static int aspTest(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t  *name, *address;

    if (ejArgs(argc, argv, T("%s %s"), &name, &address) < 2) {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    return websWrite(wp, T("Name: %s, Address %s"), name, address);
}

/******************************************************************************/
/*
 *  Test form for posted data (in-memory CGI). This will be called when the
 *  form in web/forms.asp is invoked. Set browser to "localhost/forms.asp" to test.
 */

static void formTest(webs_t wp, char_t *path, char_t *query)
{
    char_t  *name, *address;

    name = websGetVar(wp, T("name"), T("Joe Smith")); 
    address = websGetVar(wp, T("address"), T("1212 Milky Way Ave.")); 

    websHeader(wp);
    websWrite(wp, T("<body><h2>Name: %s, Address: %s</h2>\n"), name, address);
//    websWrite(wp, T("<body><h2>WEBSERVER FOR DJYOS</h2>\n"));

    websFooter(wp);
    websDone(wp, 200);
}

/******************************************************************************/
/*
 *  Home page handler
 */

static int websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
    int arg, char_t* url, char_t* path, char_t* query)
{
/*
 *  If the empty or "/" URL is invoked, redirect default URLs to the home page
 */
    if (*url == '\0' || gstrcmp(url, T("/")) == 0) {
        websRedirect(wp, T("home.asp"));
        return 1;
    }
    return 0;
}


ptu32_t GoAheadMain(void)
{
    webservermain(0, NULL);
    return true;
}

bool_t GoAheadStart(char *para)
{
    pthread_create(NULL,NULL,GoAheadMain,NULL);
    return true;
}

#include "shell.h"

static struct ShellCmdTab  gGoAheadCmd[] =
{
    {
        "goaheadstart",
        GoAheadStart,
        "usage:goaheadstart",
        "usage:this is an webserver, and the version now is 2.5.0,specified for DJYOS"
    }
};

#define CN_GOAHEAD_CMDNUM  ((sizeof(gGoAheadCmd))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gGoAheadCmdRsc[CN_GOAHEAD_CMDNUM];
bool_t ModuleInstall_GoAhead(void)
{
    bool_t result;

    result = Sh_InstallCmd(gGoAheadCmd,gGoAheadCmdRsc,CN_GOAHEAD_CMDNUM);

    return result;
}


