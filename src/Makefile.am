AM_LDFLAGS = $(OCL_LDFLAGS)
CPPFLAGS += $(OCL_CPPFLAGS)
L1LIBS = $(OPENCL_LIBS) 
#$(LIBS)

# what is the destination for programs built from this directory?
serialopencldir = $(bindir)/

# what programs should be installed in the destination?
serialopencl_PROGRAMS = cacrypt

# how to build those programs
cacrypt_SOURCES	= caCrypt.cpp \
				Graph.cpp \
				main.cpp
cacrypt_LDADD	= $(L1LIBS)