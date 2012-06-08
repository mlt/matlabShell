About
=====

These are the usage notes for matlabShell.exe, version 1.1, a matlab
shell engine suitable for use with Windows XP+.

matlabShell.exe can be used standalone in a command window, and many
installation mysteries can be cleared up by doing so, thereby helping
to determine whether any problems are with the shell installation or
with the matlab.el installation.

dot emacs
=========

Follow matlab-mode instructions for details.
Your emacs initialization should have something like this in it:

```lisp
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
;; the following is for non-session based evaluations
(setq org-babel-matlab-shell-command "c:/Programs/matlabshell/matlabshell.cmd")

;; Have libeng.dll on your PATH or use the following
(setenv "PATH" (concat "C:/PROGRA~1/MATLAB/R2011a/bin/win32;" (getenv "PATH")))

(org-babel-do-load-languages
 'org-babel-load-languages
 '((matlab . t) (octave . t)
   ))

(setq org-babel-default-header-args:matlab
  '((:results . "output") (:session . "*MATLAB*")))
```

Compilation
===========

If you want to recompile and link this code do it with make provided you have gcc installed.
Otherwise use linkit.bat if you have MS VC++ (Express) installed.
lcc shipped with Matlab has issues with stdout buffering.

# org-mode

Matlab code blocks are served by ob-octave.el .
Unfortunately Octave and Matlab deviated quite a bit in their development.
Also ob-octave has not been updated for quite a while and does not support some org-babel properties.
The followin patch can help.

```patch
diff --git a/lisp/ob-octave.el b/lisp/ob-octave.el
index 9e85757..4beddb6 100644
--- a/lisp/ob-octave.el
+++ b/lisp/ob-octave.el
@@ -67,6 +67,12 @@ end")
 (defvar org-babel-octave-eoe-indicator "\'org_babel_eoe\'")
 
 (defvar org-babel-octave-eoe-output "ans = org_babel_eoe")
+(defvar org-babel-matlab-eoe-output "
+ans =
+
+org_babel_eoe
+
+")
 
 (defun org-babel-execute:matlab (body params)
   "Execute a block of matlab code with Babel."
@@ -81,23 +87,48 @@ end")
          (vars (mapcar #'cdr (org-babel-get-header params :var)))
          (result-params (cdr (assoc :result-params params)))
          (result-type (cdr (assoc :result-type params)))
-  (out-file (cdr (assoc :file params)))
+	 (out-file (org-babel-octave-graphical-output-file params))
+	 ;; some magic from org-babel-R-construct-graphics-device-call
+	 ;; somehow we should handle nil out-file
+	 (device (and (string-match ".+\\.\\([^.]+\\)" (or out-file ""))
+		      (match-string 1 out-file)))
+	 (devices
+	  '((:bmp . "bmp")
+	    (:jpg . "jpeg")
+	    (:jpeg . "jpeg")
+	    (:tiff . "tiff")
+	    (:png . "png")
+	    (:svg . "svg") ; does not work for me in R2011a
+	    (:pdf . "pdf")
+	    (:ps . "psc")
+	    (:eps . "epsc")))
+	 (device (or (and device
+			  (cdr (assq (intern (concat ":" device))
+				     devices)))
+		     "png"))
 	 (full-body
 	  (org-babel-expand-body:generic
 	   body params (org-babel-variable-assignments:octave params)))
 	 (result (org-babel-octave-evaluate
 		  session
-		  (if (org-babel-octave-graphical-output-file params)
+		  (if out-file
 		      (mapconcat 'identity
 				 (list
-				  "set (0, \"defaultfigurevisible\", \"off\");"
+				  "set (0, 'defaultfigurevisible', 'off');"
 				  full-body
-				  (format "print -dpng %s" (org-babel-octave-graphical-output-file params)))
+				  (if (and (cdr (assq :width params))
+					   (cdr (assq :height params)) )
+				      ;; http://www.mathworks.com/support/solutions/en/data/1-16WME/?solution=1-16WME
+				      (format "set(gcf,'PaperUnits','inches','PaperPosition',[0 0 %s %s]);"
+					      (cdr (assq :width params))
+					      (cdr (assq :height params)) )
+				    )
+				  ;; we need to close figure so its size is not "inherited"
+				  (format "print('-d%s','%s'); close;" device out-file))
 				 "\n")
 		    full-body)
 		  result-type matlabp)))
-    (if (org-babel-octave-graphical-output-file params)
-	nil
+    (if out-file nil
       (org-babel-reassemble-table
        result
        (org-babel-pick-name
@@ -231,7 +262,7 @@ value of the last statement in BODY, as elisp."
 		(org-babel-comint-with-output
 		    (session
 		     (if matlabp
-			 org-babel-octave-eoe-indicator
+			 org-babel-matlab-eoe-output
 		       org-babel-octave-eoe-output)
 		     t full-body)
 		  (insert full-body) (comint-send-input nil t)))) results)
@@ -265,13 +296,20 @@ This removes initial blank and comment lines and then calls
     (org-babel-import-elisp-from-file temp-file '(16))))
 
 (defun org-babel-octave-read-string (string)
-  "Strip \\\"s from around octave string"
-  (if (string-match "^\"\\([^\000]+\\)\"$" string)
-      (match-string 1 string)
-    string))
+  "Clean up result"
+  (let*( ;; Strip \\\"s from around octave string
+	( line (if (string-match "^\"\\([^\000]+\\)\"$" string)
+		   (match-string 1 string)
+		 string))
+	;; cleanup extra prompts left in output
+	( line (if (string-match "^\\(>> \\)+" line)
+		   (substring line (match-end 1))
+		 line))
+	)
+    line))
 
 (defun org-babel-octave-graphical-output-file (params)
-  "Name of file to which maxima should send graphical output."
+  "Name of file to which octave should send graphical output."
   (and (member "graphics" (cdr (assq :result-params params)))
        (cdr (assq :file params))))
 ```
 