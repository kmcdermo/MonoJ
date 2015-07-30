CPPFLAGS := $(shell root-config --cflags)
CXXFLAGS := -std=c++0x -g
LDFLAGS  := $(shell root-config --libs)

TGTS := main

EXES := ${TGTS}

SRCDIR := src
OBJDIR := bin
DEPDIR := dep

SRCS := $(wildcard ${SRCDIR}/*.cc)
OBJS := $(addprefix ${OBJDIR}/,$(notdir $(SRCS:.cc=.o)))
DEPS := $(addprefix ${DEPDIR}/,$(notdir $(SRCS:.cc=.d)))

-include ${DEPS}

main: ${OBJS} 
	${CXX} ${CXXFLAGS} -o $@ ${OBJS} ${LDFLAGS}

${OBJS}: ${OBJDIR}/%.o: ${SRCDIR}/%.cc
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -c -o $@ $<

# ROOT6
HEADDIR := interface
HEADERS := $(wildcard ${HEADDIR}/*.h)

${SRCDIR}/dict.cc: ${HEADERS} 
	rootcling -f $@ -c -p $^

clean:
	-rm -f ${EXES} ${DEPDIR}/*.d ${SRCDIR}/*.o ${SRCDIR}/*.pcm ./*/*~

