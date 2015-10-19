TARGET       = bin/tp2
SOURCE       = $(wildcard source/*.cpp)
CXX          = mpicxx
CXX_FLAGS    = -std=c++11

SOURCES = $(wildcard source/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	rm -rf $(SOURCES:.cpp=.o) $(TARGET)

run: all
	mpiexec -prepend-rank -np $(np) ./$(TARGET) $(size) $(local)
