PROJECT := $(shell pwd)
SRCDIR  := $(PROJECT)/../../
TEST := $(wildcard $(PROJECT)/*.cpp)
SRC := $(wildcard $(SRCDIR)/src/*.cpp)
override SRC := $(SRC) $(TEST) 
TARGET := mutualDeadLock_test
CXX := g++
INCLUDE := -I $(SRCDIR)/src/
CFLAGS := -std=c++17 -g -pg -Wall -O3 -D_PTHREADS
LIBS := -lpthread


$(TARGET): $(SRC)
	$(CXX) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

clean :
	find . -name '*.o' | xargs rm -f
	find . -name $(TARGET) | xargs rm -f