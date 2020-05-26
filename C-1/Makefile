SrcFiles=$(wildcard src/*.cpp)
HeaderFiles=$(wildcard include/*.h)
ObjFiles=$(patsubst %.cpp, %.o, $(SrcFiles))

CXX = g++
INCLUDE = -I./include
CXXFLAGS = $(INCLUDE)

C-1: $(ObjFiles) $(HeaderFiles)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	@rm $(ObjFiles)
	@rm C-1