ifdef DJGPP
 include src/djgpp.mk
else
ifeq ($(OS),Windows_NT)
 include src/mingw.mk
else
ifeq ($(shell uname), Linux)
 include src/linux.mk
endif
endif
endif

