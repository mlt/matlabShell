== About

These are the usage notes for matlabShell.exe, version 1.1, a matlab
shell engine suitable for use with Windows XP+.

matlabShell.exe can be used standalone in a command window, and many
installation mysteries can be cleared up by doing so, thereby helping
to determine whether any problems are with the shell installation or
with the matlab.el installation.

== dot emacs

Follow matlab-mode instructions for details.
Your emacs initialization should have something like this in it:

```elisp
;; Load CEDET.
;; See cedet/common/cedet.info for configuration details.
;; IMPORTANT: For Emacs >= 23.2, you must place this *before* any
;; CEDET component (including EIEIO) gets activated by another 
;; package (Gnus, auth-source, ...).
(load-file "d:/dev/emacs/cedet/common/cedet.el") ;; cedet-devel-load.el")

;; Replace path below to be where your matlab.el file is.
(add-to-list 'load-path "d:/dev/emacs/matlab-emacs")
(load-library "matlab-load")

;; Change path
(setq matlab-shell-command "c:/Programs/matlabshell/matlabshell.exe")
(setq matlab-shell-command-switches '("10000" "20000"))

;; Have libeng.dll on your PATH or use the following
(setenv "PATH" (concat "C:/PROGRA~1/MATLAB/R2011a/bin/win32;" (getenv "PATH")))

(org-babel-do-load-languages
 'org-babel-load-languages
 '((matlab . t) (octave . t)
   ))

(setq org-babel-default-header-args:matlab
  '((:results . "output") (:session . "*MATLAB*")))
```

== Compilation

If you want to recompile and link this code do it with make provided you have gcc installed.
Otherwise use linkit.bat if you have MS VC++ (Express) installed.
lcc shipped with Matlab has issues with stdout buffering.
