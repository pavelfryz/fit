##################################################
CFLAGS_COMMON=-pipe -ggdb -Wall
CC=gcc
CXX=g++

# INTEL OR AMD
#OPENCLROOT = $(AMDAPPSDKROOT)
#OPENCLROOT = $(INTELSDKROOT)

# ATI local installation
#MOPENCL_LIBS = -L"$(OPENCLROOT)/lib/x86" -static -lOpenCL
#MOPENCL_FLAGS = -I"$(OPENCLROOT)/include/"

# CUDA
#MOPENCL_LIBS=-L$(CUDA_LIB_PATH) -static -lOpenCL
#MOPENCL_FLAGS=-I$(CUDA_INC_PATH)

# CVT_linux
#MOPENCL_LIBS=-L/usr/lib/ -lOpenCL
#MOPENCL_FLAGS=-I/usr/local/share/cuda/include/

# Linux standard
MOPENCL_LIBS=-lOpenCL
MOPENCL_FLAGS=

##################################################
ifeq ($(OS), Windows_NT)
	CFLAGS=$(CFLAGS_COMMON) $(MOPENCL_FLAGS) -Wl,--enable-auto-import -std=c++11 -Isrc/ -Iinclude/ -Iinclude/SDL
	DEFINES= -DGLEW_STATIC
	LIBS= -Lbin/ -Llib/ -lSDL2 -lSDL2main -lopengl32 $(MOPENCL_LIBS)
	RM=del
else
	CFLAGS=$(CFLAGS_COMMON) $(MOPENCL_FLAGS) -Isrc/ -Iinclude/ -std=c++1y
	DEFINES= -DGLEW_STATIC
	LIBS=-lGL -lSDL2 $(MOPENCL_LIBS)
	RM=rm -f
endif

.PHONY: all clean

all: bin/BVH.exe

clean:
	$(RM) bin/*.exe

bin/BVH.exe:src/BVH.cpp src/BaseApp.cpp  src/BVH.h src/BaseApp.h src/glew.c
	$(CXX) -o bin/BVH.exe src/BVH.cpp src/BaseApp.cpp src/glew.c $(CFLAGS) $(DEFINES) $(LIBS)




