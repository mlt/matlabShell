/* */
/*
 *	matlabShell.c
 *
 *	This is a simple program call MATLAB matlab.el
 *
 *    Copyright (c) 1998 by Robert A. Morris
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * If you did not receive a copy of the GPL you can get it at
 *     //http://www.fsf.org
 * 
 * This program is a matlab shell which will run under WindowsNT, and possibly
 * any OS, and can be invoked either from from matlab.el or a native
 * command shell.
 *
 * See the usage notes at the end of this file, or invoke it with
 * -h argument for brief usage message

 *      02dec98 version 1.0 ram@cs.umb.edu
 *         -remove echo; instead require 
 *              matlab-shell-process-echoes nil
 *              in matlab-shell-mode-hook
 *         -works with matlab.el ver 2.2
 *      01nov98 version 0.91 ram@cs.umb.edu
 *      Bugs fixed:
 *        "exit" command processing should be case sensitive
 *        input not echoed properly
 *        line spacing different from Matlab standard window
 *
 *        - Matlab "exit" command must be all lower case, so
 *          replace isExitCommand() with simple strncmp()
 *        - echo input because something is erasing it on NT. Maybe comint.el?
 *        - make line spacing look like Matlab native shell
 *      Known deficiencies in 0.91:
 *      1. Matlab standard command window pops up when starting
 *      2. Matlab window doesn't exit if *matlab* buffer is killed without
 *       sending exit command to matlab from matlabShell
 *
 *	01nov98 version 0.9 ram@cs.umb.edu
 *	Known deficencies in 0.9
 *	1. should be quiet production mode and verbose C debugging modes
 *      2. Matlab Debug is untested, probably doesn't work
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include "engine.h"

#define MAXLEN 1024;		/* default */
int main(int argc, char **argv)
{
  char version[]="MatlabShell 1.0. 02Dec1998.\nCopyright 1998 Robert A. Morris. \nThis is Free Software licensed under the GNU Public License.";
	Engine *ep;
	int inputMax; /* buffer size */
	char *inbuf;

	int outputMax; /*buffer size */
	char *fromEngine; 

	int noArgs;
	int len, pos;

	int debug = 0;
	int retval; /* for debug */
	
	int isExitCommand(char* str);

	/* matlab.el always invokes the shell command with two
	   arguments, the second of which is NULL unless the lisp
	   variable matlab-shell-command-switches is set, in which
	   case the string value of that variable is passed as the
	   argv[1] to the shell program. In that case we have to parse
	   this string. In the standalone case, we may see 1, 2, or 3
	   args, the first of which is always the program path
	*/

	printf("%s\n", version);
	noArgs = (argc==1) || (argv[1]==0 || argv[1][0] == 0);

	if ( (!noArgs) && (argv[1][0] == '?' || !strcmp(argv[1], "-h"))) {
	  printf("usage: %s <inbufSize> <outbufSize>", argv[0]);
	  exit(0);
	}

	/* Start the MATLAB engine */
	if (!(ep = engOpen(NULL))) {
	  printf("Can not start engine\n");
	  exit(-1);
	}

	inputMax = MAXLEN;
	outputMax = 0;
	
	/* if there are args they might be:
	   1. one string from matlab.el with either 1 or 2 numbers
	   2. one or two strings from a command shell invocation
	*/
	if ( !noArgs ){ 
	  inputMax = atoi(argv[1]);
	  if (argc>2 && argv[2]) /* doesn't happen under matlab.el */
	    outputMax = atoi(argv[2]);
	  else { /*matlab.el passes args as a single string */
	    len = strlen(argv[1]);
	    pos = strcspn(argv[1], " \t\n"); /* scan to white space */
	    if (debug) printf("argv[1]=%s len=%d pos=%d\n", argv[1], len, pos);
	    argv[1][pos]=0; /* split */
	    inputMax = atoi(argv[1]);
	    if (pos < len) /* there was stuff left */
	      outputMax = atoi(1+pos+argv[1]);
	  }
	}
	if (!outputMax)		/* nobody set it */
	  outputMax = 8*inputMax;

	inbuf = malloc(inputMax+2); /* room for newline and \0 */
	outputMax = inputMax*8;
	fromEngine = malloc(outputMax +2);
	engOutputBuffer(ep, fromEngine, outputMax);

	
	while (1) {
	  printf(">> "); fflush(stdout);
	  fgets(inbuf, inputMax, stdin);
	    
	    /* On NT, something erases input and I don't know what. It
	     might be the way comint.el is passing input to this
	     process. If this is platform dependent then other platforms
	     may see doubled input  */

	  //	  printf("%s",inbuf);   fflush(stdout);	/* re-echo input */
	  
	  /* it would be good to test retval, but on NT it seems
	     to return non-zero whether the engine is
	     running or not, contrary to Matlab doc.
	     In fact, I can't figure out how to know whether the
	     engine is running, so special case "exit"
	  */
	  
	  retval = engEvalString(ep, inbuf);
	  if (!strncmp(inbuf,"exit",4)) {
	    printf("exiting\n"); fflush(stdout);
	    exit(0);
	  }
	  if (fromEngine[0] == 0 ){ /*the command didn't return anything */
	    if(debug) 
	      printf("\ncmd returned nothing");
	  }
	  else {
	    printf("%s", fromEngine); /* show matlab reply */
	    fromEngine[0] = 0; /* clear buffer, else confusing */  
	    if (debug) {
	      printf("retval=%x\n");
	      fflush(stdout);
	    }
	  }
	}
	exit(0);
}


char* downcase(char* str)
{
  /* downcase string in place. just touch upper case alpha chars */
  char c; char* cp=str;
  while ( (c = *cp)) {
    if (c >= 'A' && c <= 'Z') *cp |= 0x20;
    cp++;
  }
  return str;
}

int strncasecmp(char* a, char* b, int n) {
  /* why isn't this in string library??? */
  char* copya; char* copyb; int retval;
  copya = malloc(1+strlen(a));
  strcpy(copya, a);
  downcase(copya);
  copyb = malloc(1+strlen(b));
  strcpy(copyb,b);
  downcase(copyb);
  retval=strncmp(copya, copyb, n);
  free(copya); free(copyb);
  return(retval);
}

int isExitCommand(char* str)
{
  return !strncasecmp(str, "exit", 4);
}
 
/* usage notes
1.0 02dec98 ram@cs.umb.edu
Your emacs initialization should have something like this in it:

(autoload 'matlab-shell "matlab" "Interactive Matlab mode." t)
( setq matlab-shell-command "D:/users/ram/matlab/engine/matlabShell.exe"
       matlab-shell-command-switches "500 10000"
	 shell-command-echoes nil))

matlab-shell-command should evaluate to a string with the full path name 
of the executable of this shell.

With matlab.el 2.2 you may need to have
(load "font-lock")
in your emacs initialization if nothing else loads it.
The symptom of needing this will be complaints about some emacs font stuff 
not found while loading matlab.el

matlab-shell-command-switches is optional. It should evaluate to a string
with either one or two integers. The first is the size in bytes
of the buffer the program uses for input, and the second, if present,
is the size of the buffer to which Matlab returns its output. If only
the first is present, the second is set to 8 times the first. If the string
is not set it defaults to "", and the shelll treats it as "1024 8192".
Previous releases had "500 5000" as the suggested value, but 5000 characters
may be rather small for output.

Beginning with release 0.93, the lisp variable matlab-shell-echoes must have 
value nil. matlab.el sets this to t, but on NT, the symptom of having this t 
is that the command you give to matlab will not be echoed in the
matlabShell buffer, because the matlab.el thinks that matlabShell will echo it.
matlabShell doesn't echo in order that it can also work reasonably from a DOS 
command window.

If you want to recompile and link this code do it with the file
linkit.bat which should look something like this:

mex -f %MATLAB%\bin\msvc50engmatopts.bat matlabShell.c

You can only do this with MS VC++ 5.0.

Known issues using matlabShell.exe 1.0 with matlab.el version2.2

     1. the function matlab-shell-run-region (C-c C-r) does not leave
     the cursor positioned in the right place.

     2. The function matlab-shell-save-and-go (C-c C-s) passes only
     the file name to the shell, not the full pathname. Consequently,
     if the file you are editing is not the one found on your Matlab
     path, it is not the one executed. If the current directory is not
     on your path at all and there is no file of the same name, Matlab
     will complain that there is no such file. If there is one on your
     path with the same name, you will come to believe that your edits
     are having no effect.

*/

