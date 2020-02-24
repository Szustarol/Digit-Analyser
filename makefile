CXX=g++
CXXFLAGS=-Wall -std=c++11 -g -pthread
LIBS=-lGL -lglfw -lGLEW
FILES= analyser.cpp \
			shader.cpp \
			UI/uiObject.cpp \
			UI/button.cpp \
			UI/drawArea.cpp \
			data/texture.cpp \
			data/network.cpp \
			PerlinNoise.cpp

OBJS= $(FILES:%.cpp=%.o)

compile: $(FILES)
	$(CXX) $(FILES) $(CXXFLAGS) $(LIBS)
