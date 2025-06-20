MAINSRC = main.cpp
ASSEMBLERSRC = ${wildcard assembler/*.cpp}
BASM = basm

all: ${BASM}

${BASM}: ${MAINSRC} ${ASSEMBLERSRC} ${PREPROCESSORSRC}
	g++ $^ -o $@

clean:
	rm ${BASM}