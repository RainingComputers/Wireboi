.PHONY : help clean all build dirs install profile
help :
	@echo "clean"
	@echo "      Remove auto-generated files."
	@echo "build"
	@echo "      Build release executable."
	@echo "build CXX=x86_64-w64-mingw32-g++"
	@echo "      Cross-compile to build 64bit windows executable."
	@echo "build DEBUG=1"
	@echo "      Build executable for debugging."
	@echo "install"
	@echo "      Installs the executable to PATH. Execute 'make build' first."
	@echo "profile"
	@echo "      Create dot graph from gmon.out"
	@echo "appdir"
	@echo "      Create appdir directory for AppImage packaging"

now=$(shell date +%d-%m_%H-%M-%S)

# Name of the executable
EXEC_NAME = wireboi

# C++ compiler
CXX ?= g++

# Get platform
ifeq ($(OS), Windows_NT)
	PLATFORM = $(OS)_$(PROCESSOR_ARCHITECTURE)
	EXEC_NAME := $(EXEC_NAME).exe
else ifeq ($(CXX), x86_64-w64-mingw32-g++)
	PLATFORM = Windows_NT_AMD64
	EXEC_NAME := $(EXEC_NAME).exe
	OS = Windows_NT
	PROCESSOR_ARCHITECTURE = AMD64
else
	OS = $(shell uname -s)
	PLATFORM = $(shell uname -s)_$(shell uname -p)
endif

# Linked libs
LIB_PATH = lib/$(PLATFORM)

ifeq ($(OS), Windows_NT)
	LIBS =  -L$(LIB_PATH)/SFML -lsfml-graphics-s -lsfml-window-s -lsfml-system-s \
	-lfreetype -lopengl32 -lwinmm -lgdi32 -mwindows -static-libgcc -static-libstdc++
else
	LIBS = -L$(LIB_PATH)/SFML -lsfml-graphics -lsfml-window -lsfml-system `pkg-config --libs gtk+-3.0`
endif

# Include path
ifeq ($(OS), Windows_NT)
	INCLUDE = -Iinclude/ -D_WIN32 -MMD -MP -DSFML_STATIC
else
	INCLUDE = -Iinclude/ `pkg-config --cflags gtk+-3.0`
endif

# Set C++ compiler flags, build type and build path
DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CXXFLAGS = -g -pg
	BUILD_TYPE = debug
else
    CXXFLAGS = -O -no-pie
	BUILD_TYPE = release_$(PLATFORM)
endif

# Find all .cpp files in src/
SOURCES = $(shell find src/ -name '*.cpp')
# Set object file names, all object files are in obj/
OBJECTS = $(SOURCES:src/%.cpp=obj/$(BUILD_TYPE)/%.o)

# Remove object files and executable
clean:
	rm -f -r bin/
	rm -f -r obj/
	rm -f gmon.out
	rm -f profile.output
	rm -f profile_*.png
	rm -f *.AppImage
	rm -f -r *.AppDir

# For compiling .cpp files in src/ to .o object files in obj/
obj/$(BUILD_TYPE)/%.o: src/%.cpp src/%.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

obj/$(BUILD_TYPE)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

# For creting directories required for linking and building executable
dirs:
	mkdir -p obj/$(BUILD_TYPE)/
	mkdir -p bin/$(BUILD_TYPE)/

# Linking all object files to executable 
bin/$(BUILD_TYPE)/$(EXEC_NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o bin/$(BUILD_TYPE)/$(EXEC_NAME) $(OBJECTS) $(LIBS)

all: dirs bin/$(BUILD_TYPE)/$(EXEC_NAME) 

# Build executable
build: all

profile:
	gprof bin/debug/$(EXEC_NAME) > profile.output 
	gprof2dot profile.output | dot -Tpng -o profile_$(now).png
	xdg-open profile_$(now).png

appdir:
	rm -f -r $(EXEC_NAME).AppDir/

	mkdir $(EXEC_NAME).AppDir/
	mkdir -p $(EXEC_NAME).AppDir/usr/bin/
	mkdir -p $(EXEC_NAME).AppDir/usr/lib/
	
	cp AppRun $(EXEC_NAME).AppDir/AppRun
	chmod +x $(EXEC_NAME).AppDir/AppRun

	cp Wireboi.desktop $(EXEC_NAME).AppDir/Wireboi.desktop
	cp wireboi.svg $(EXEC_NAME).AppDir/Wireboi.svg

	cp bin/$(BUILD_TYPE)/$(EXEC_NAME) $(EXEC_NAME).AppDir/usr/bin/$(EXEC_NAME)
	
	cp lib/$(PLATFORM)/SFML/* $(EXEC_NAME).AppDir/usr/lib/

install:
	cp bin/$(BUILD_TYPE)/$(EXEC_NAME) /usr/local/bin

