#!MAKE

SOURCE=main.cpp fileworks.cpp solver.cpp
TARGET=main
ifneq '' '$(findstring clang++,$(COMPILER_VERSION))'
  CXX=clang++
else ifneq '' '$(findstring g++,$(COMPILER_VERSION))'
  CXX=g++
endif

CFLAGS = -I. -Wall -Wextra -std=gnu++17 -march=native -flto -O3

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CFLAGS) -o $@ -I. $^

$(TARGET)-dbg: $(SOURCE)
	$(CXX) $(CFLAGS) -DDEBUG=1 -o $@ -I. $^

run: $(TARGET)
	./$(TARGET)

debug: $(TARGET)-dbg
	./$(TARGET)-dbg


clean:
	rm -rf $(TARGET) $(TARGET)-dbg

