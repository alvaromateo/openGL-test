BIN = main.exe
SRC = main.c
OBJ = ${SRC:.c=.o}

LIBS    = -framework OpenGL -framework GLUT
CFLAGS  = -g -pedantic -Wall -Wvariadic-macros -Os -Wno-deprecated
LDFLAGS = -g ${LIBS}

all: options ${BIN}

options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"

.c.o:
	g++ -c ${CFLAGS} $<

${BIN}: ${OBJ}
	g++ -o $@ ${OBJ} ${LDFLAGS}

run: ${BIN}
	./${BIN}

clean:
	rm -f *.o ${BIN}

.PHONY: all options run clean
