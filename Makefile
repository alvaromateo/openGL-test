## Add to this line all the files to compile
SRCS := 
BINS := $(patsubst %.cpp,%,$(SRCS))
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
## The line above is equivalent to --> OBJS := $(SRC:.cpp=.o)
SUFFIX = .exe
PROGS := $(patsubst %.cpp,%$(SUFFIX),$(SRCS))

## This line is for executing the bin specified when "make run" is called
ENTREGA = 

## Add to this line the files to compile which need model.h
SRCS_M := 
BINS_M := $(patsubst %.cpp,%,$(SRCS_M))
OBJS_M := $(patsubst %.cpp,%.o,$(SRCS_M))

LIBS 	= -lglut -lGL -lGLU
## LIBS    = -framework OpenGL -framework GLUT
CFLAGS  = -g -pedantic -Wall -Wvariadic-macros -Os -Wno-deprecated
LDFLAGS = -g ${LIBS}

all: $(PROGS) $(BINS_M)

.cpp.o:
	g++ -c $(CFLAGS) $< -o $@

.SECONDEXPANSION:
OBJ = $(patsubst %$(SUFFIX),%.o,$@)
BIN = $(patsubst %$(SUFFIX),%,$@)
OBJ_M = $(patsubst %,%.o,$@)
BIN_M = $@

%$(SUFFIX): $(OBJS)
	g++ $(OBJ) $(LDFLAGS) -o $(BIN) 

$(BINS_M): $(OBJS_M) model.o
	g++ $(OBJ_M) model.o $(LDFLAGS) -o $(BIN_M)

model.o: model.cpp
	g++ -c $(CFLAGS) model.cpp

.PRECIOUS: %.o


run: $(ENTREGA)
	./$(ENTREGA)

clean:
	rm -f -r *.o *.dSYM $(BINS) $(BINS_M)

.PHONY: all run clean
