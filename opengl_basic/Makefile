CXXFLAGS  = -lGL -lGLU -lm -lobjc -lstdc++
CXXFLAGS += -framework GLUT -framework OpenGL
LDDFLAGS  = -L"/System/Library/Frameworks/OpenGL.framework/Libraries"


all:
	g++ -o draw draw.c ${CXXFLAGS} ${LDDFLAGS}