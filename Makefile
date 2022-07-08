CXX			= g++ -std=c++2a
CXX11		= g++-11 -std=c++2a
INCLUDES	= -I ../fastflow/

LDFLAGS 	= -pthread
OPTFLAGS	= -O3


LIB			= 	./utils/utility.cpp

TARGETS 	=	sequential.out \
				parallel_chunks_barrier.out \
				parallel_chunks_barrier_threadpinned.out \
				parallel_row_cyclic_barrier.out \
				ff_parallel.out

.PHONY: all clean

all: $(TARGETS)

sequential.out: sequential.cpp
	$(CXX) $(LIB) $(OPTFLAGS) $^ -o $@

parallel_chunks_barrier.out: parallel_chunks_barrier.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(LIB) $^ $(LDFLAGS) -o $@

parallel_chunks_barrier_threadpinned.out: parallel_chunks_barrier_threadpinned.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(LIB) $^ $(LDFLAGS) -o $@

parallel_row_cyclic_barrier.out: parallel_row_cyclic_barrier.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(LIB) $^ $(LDFLAGS) -o $@

ff_parallel.out: ff_parallel.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) $(LIB) $^ $(LDFLAGS) -o $@

clean:
	rm -rf *.out