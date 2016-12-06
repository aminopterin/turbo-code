# filename: makefile
# author: Tzu-Yu Jeng
# date: early Sep. 2016
# description: linker for the whole project

# the C++ compiler to use
CXX = g++

# the C++ compile-time flags
# for compatibility, use 2003 standard
CXXFLAGS = -std=c++03

# removal of generated files: "rm" for remove, "-f" for force
RM =rm -f

# to define the executable's filename
MAIN = ./bin/turbo

# to define the C source files
SRCS = $(wildcard src/*.cpp)

# to replace all .cpp with .o
OBJS = $(SRCS:.cpp=.o)

# the directory being included:
# for binary libraries, use -L/...;
# for included files, use -I/...;
INCLUDES =

# the headers
DEPS = $(wildcard src/*.h)

# message displayed in the beginning (defined later)
MSG =

# the normal object for release
all : all_flag message $(MAIN)

all_flag :
	$(eval MSG := To compile the executable $(MAIN):)
	$(eval CXXFLAGS += -O3)

# the object for sake of debugging
debug : debug_flag message $(MAIN)

# "-Wall" turns on all warnings (warn all)
# "-g" is the debug option: saving symbol table, line number
debug_flag :
	$(eval MSG := To compile the debug version of executable $(MAIN):)
	$(eval CXXFLAGS += -g -Wall)

# output pre-build message
message :
	@echo $(MSG)

# to compile the main executable
$(MAIN) : $(OBJS)
	@echo Compiling $(MAIN)...
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo Compilation successful!

# all object files depends on all headers
$(OBJS) : $(DEPS)

# This is a suffix replacement rule for building .o's from .cpp's:
# Value $^ is list of all prerequisites of rules of this target;
# Value $< is the first prerequisite of first rule (rhs. of `:');
# Value $@ is the name of this target (lhs. of `:').
# Option -c for g++ claims not to run the linker.
./src/%.o : ./src/%.cpp
	@echo Now $@ is being created...
	$(CXX) $(CXXFLAGS) -o $@ $(INCLUDES) -c $<

# Claim a fake (phony) object that is always out of date,
# in order it is always being re-made (`make clean').
.PHONY : all all_flag debug debug_flag message clean 

clean :
	$(RM) $(OBJS) $(MAIN)

