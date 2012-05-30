@echo off
SET MATLABROOT=C:\Progra~1\MATLAB\R2011a

mbuild -L%MATLABROOT%\extern\lib\win32\lcc matlabShell.c -leng
