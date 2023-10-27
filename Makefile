# Compiler
CC = g++
# Flags
FLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
	-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
	-Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
	-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
	-Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Winit-self -Wredundant-decls \
	-Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 \
	-Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override \
	-Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast \
	-Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing \
	-Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation \
	-fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer \
	-Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla \
	-fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero, \
	# integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute, \
	# shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

BUILD_DIR = ./build
SRC_DIRS = ./src
SRC = $(wildcard $(SRC_DIRS)/*.cpp)
OBJS := $(SRC:%.cpp=$(BUILD_DIR)/%.cpp.o)
INCLUDE_DIR = ~/Dev/assembler/include  # to be changed
# LIB = stack.a 					# to be changed
# L_DIR = ~/Dev/Stack/build/src/	# to be changed
VM_INCLUDE = ~/Dev/VM_CPU/include
CODE_CTOR = ~/Dev/VM_CPU/build/src/codector.cpp.o
CODE_DUMP = ~/Dev/VM_CPU/build/src/codedump.cpp.o


EXEC = asm

$(BUILD_DIR)/$(EXEC): $(OBJS)
	@$(CC) $(OBJS) $(CODE_CTOR) $(CODE_DUMP) $(FLAGS) -o $@ -I $(INCLUDE_DIR) -I $(VM_INCLUDE)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/$(SRC_DIRS)
	@$(CC) $(FLAGS) $(FLAGS) -c $< -o $@ -I $(INCLUDE_DIR) -I $(VM_INCLUDE)

.PHONY: clean

clean:
	@rm -rf $(BUILD_DIR)

run:
	@./$(BUILD_DIR)/$(EXEC) tests/square_solver.asm -o tests/square_solver.bin
	../VM_CPU/build/processor tests/square_solver.bin
