all=matlabShell.exe

MATLABROOT=C:/Progra~1/MATLAB/R2011a

CFLAGS:=-O3 -I$(MATLABROOT)/extern/include

LIB=$(MATLABROOT)/extern/lib/win32/microsoft/libeng.lib

matlabShell.exe: matlabShell.o $(LIB)
	$(CC) -o $@ $^
	strip $@

clean:
	$(RM) matlabShell.exe matlabShell.o
