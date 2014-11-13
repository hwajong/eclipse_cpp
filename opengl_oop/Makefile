CXXFLAGS =	-O2 -g -Wall -fmessage-length=0
CXXFLAGS += -framework GLUT -framework OpenGL -Wno-deprecated

LIBS = -lGL -lGLU -L"/System/Library/Frameworks/OpenGL.framework/Libraries"

all:
	$(CXX) opengl_oop.cpp $(CXXFLAGS) $(LIBS)

clean:
	rm -f a.out

	
