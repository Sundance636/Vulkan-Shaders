CXX=g++
CXXFLAGS=-Wall -std=c++17 -I./Inc -I./Libs
LDFLAGS=-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

GLSLC=glslc

BIN=VulkanApp


CPP_FILES := $(wildcard ./Src/*.cpp)
CPP_OBJECTS := $(CPP_FILES:./Src/%.cpp=./Src/%.o)

SHADERS := $(wildcard ./shaders/shader.*)
SHADERS_SPV := $(SHADERS:./shaders/shader.%=./shaders/%.spv)


default: $(BIN)

# Compile C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -c $(LDFLAGS) -o $@ 

# Compile shader source files
%.spv: shader.%
	$(GLSLC) $< -o $@

# Link object files
$(BIN): $(CPP_OBJECTS) $(SHADERS_SPV)
	$(CXX) $(CPP_OBJECTS) -o $(BIN) $(LDFLAGS)
#load shaders

run: $(BIN)
	./$(BIN)

memorycheck: $(BIN)
# debugging with valgrind
	valgrind --leak-check=full --log-file="debug.txt" ./$(BIN)

clean:
	rm ./Src/*.o
	rm ./shaders/*.spv
	rm $(BIN)
