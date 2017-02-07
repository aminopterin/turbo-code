# filename: makefile
# author: Tzu-Yu Jeng
# date: early Sep. 2016
# description: linker for the whole project

# to set the C++ compiler to use
CXX = g++

# to set the C++ compile-time flags
# for compatibility, use 2003 standard
CXXFLAGS = -std=c++03

# to remove the generated files: `rm` for remove, `-f` for force
RM =rm -f

# to set the executable's filename
MAIN = ./bin/turbo

# to set the C source files
SRCS = $(wildcard src/*.cpp)

# to replace all `.cpp` with `.o`
OBJS = $(SRCS:.cpp=.o)

# to set the directory being included:
# for binary libraries, use -L/...;
# for included files, use -I/...;
INCLUDES =

# to set the headers
DEPS = $(wildcard src/*.hpp)

# to hold message displayed in the beginning (defined later)
MSG =

# for release
all : all_flag message $(MAIN)

all_flag :
	$(eval MSG := To compile the executable $(MAIN):)
	$(eval CXXFLAGS += -O3)

# for sake of debugging
debug : debug_flag message $(MAIN)

# `-Wall` turns on all warnings (warn all)
# `-g` is the debug option: saving symbol table, line number
debug_flag :
	$(eval MSG := To compile the debug version of executable $(MAIN):)
	$(eval CXXFLAGS += -g -Wall)

# to output pre-build message
message :
	@echo $(MSG)

# to compile the main executable
$(MAIN) : $(OBJS)
	@echo Compiling $(MAIN)...
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo Compilation successful!

# that all object files depends on all headers
$(OBJS) : $(DEPS)

# this is a suffix replacement rule for building `.o`'s from `.cpp`'s:
# value `$^` is list of all prerequisites of rules of this target;
# value `$<` is the first prerequisite of first rule (rhs. of `:`);
# value `$@` is the name of this target (lhs. of `:`).
# option `-c` for g++ claims not to run the linker
./src/%.o : ./src/%.cpp
	@echo Now $@ is being created...
	$(CXX) $(CXXFLAGS) -o $@ $(INCLUDES) -c $<

# to claim a fake (phony) object that is always out of date,
# in order it is always being re-made (`make clean`).
.PHONY : all all_flag debug debug_flag message clean 

clean :
	$(RM) $(OBJS) $(MAIN)

