These are the usage notes for matlabShell.exe, version 1.0, a matlab
shell engine suitable for use with Windows NT and probably for Windows
95. Install this executable in your execution path, and use it with
matlab.el version 2.2, which can be obtained from ftp.mathworks.com.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * If you did not receive a copy of the GPL you can get it at
 *     //http://www.fsf.org


matlabShell.exe can be used standalone in a command window, and many
installation mysteries can be cleared up by doing so, thereby helping
to determine whether any problems are with the shell installation or
with the matlab.el installation.

I don't promise to answer questions, which should therefore be mailed
to the matlab-emacs interest list matlab-emacs@mathworks.com or posted
to comp.soft-sys.matlab.

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

