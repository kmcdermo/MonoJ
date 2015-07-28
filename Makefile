CPPFLAGS := $(shell root-config --cflags)
CXXFLAGS := -std=c++11
LDFLAGS  := $(shell root-config --libs)

TGTS := main

EXES := ${TGTS}

SRCS := $(wildcard *.cc)
DEPS := $(SRCS:.cc=.d)
OBJS := $(SRCS:.cc=.o)

-include ${DEPS}

main: ${OBJS} 
	${CXX} ${CXXFLAGS} -o $@ ${OBJS}  ${LDFLAGS}

${OBJS}: %.o: %.cc
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -c -o $@ $<

HEADERS := $(wildcard *.h)

dict.cc: ${HEADERS} 
	rootcling -f $@ -c -p $^

clean:
	-rm -f ${EXES} *.d *.o *.pcm *~

