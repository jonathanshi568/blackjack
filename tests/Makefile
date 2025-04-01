# Makefile

CXX = g++            # Compiler
CXXFLAGS = -std=c++20 # Compiler flags
SRCS = random_latency.cpp # Source files
OBJS = $(SRCS:.cpp=.o) # Object files (replace .cpp with .o)

RANDOM_LATENCY_SRCS = random_latency.cpp
RANDOM_LATENCY_OBJS = $(RANDOM_LATENCY_SRCS:.cpp=.o)

# Default target: compile and link
random_latency: $(RANDOM_LATENCY_OBJS)
	$(CXX) $(RANDOM_LATENCY_OBJS) -o random_latency

# Rule to compile each .cpp file to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJS) random_latency