TARGET       = bin/tp3
SOURCE       = $(wildcard source/*.cpp)
CXX          = mpicxx
CXX_FLAGS    = -fopenmp

SOURCES = $(wildcard source/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	rm -rf $(SOURCES:.cpp=.o) $(TARGET)

run: all
	mpiexec -np $(np) ./$(TARGET) $(threads) $(jobs) $(size) $(sort_method)
