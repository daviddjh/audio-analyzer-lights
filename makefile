 
aoanalyser: aoanalyser.cpp
	g++ $(pkg-config --cflags glfw3) -o aa aoanalyser.cpp -I/usr/include/python2.7 -I/home/daviddjh/.local/lib/python2.7/site-packages/numpy/core/include/ -lfftw3_threads -lfftw3 -lfftw3f -lm -lpulse-simple -lpulse -g -std=c++11 -lpython2.7 -lGL -lGLU -lglut -lGL -lGLU -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi

debug: aoanalyser.cpp
	g++ $(pkg-config --cflags glfw3) -o aa aoanalyser.cpp -I/usr/include/python2.7 -I/home/daviddjh/.local/lib/python2.7/site-packages/numpy/core/include/ -lfftw3_threads -lfftw3 -lfftw3f -lm -lpulse-simple -lpulse -std=c++11 -lpython2.7 -lGL -lGLU -lglut -lGL -lGLU -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi
