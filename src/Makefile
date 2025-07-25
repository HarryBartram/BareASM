INC=${wildcard inc/*.hpp}
SRC=${wildcard src/*.cpp}
MAIN=main.cpp
EXE=basm

all: ${EXE}

${EXE}: ${INC} ${SRC} ${MAIN}
	g++ ${MAIN} ${SRC} -o $@

clean:
	rm ${EXE}