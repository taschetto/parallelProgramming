TARGET       = bin/tp1
SOURCE       = $(wildcard source/*.cpp)
CXX          = mpicxx
CXX_FLAGS    =

SOURCES = $(wildcard source/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	rm -rf $(SOURCES:.cpp=.o) $(TARGET)

run: all
	mpiexec -np $(np) ./$(TARGET) $(size)