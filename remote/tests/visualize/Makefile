CC = g++
CFLAGS = -I../../include -I../../../common/include
DEBUGFLAGS = $(CFLAGS) -g -D_DEBUG
RELEASEFLAGS = $(CFLAGS)
LDFLAGS = -L../../lib -lSDL2 -lGL -lGLU -lcommon_d -lremote_d

.PHONY: all release debug clean

all: debug

release: visualize.x
debug: visualize_d.x

visualize.x: test_visualize.cpp
	$(CC) $(DEBUGFLAGS) test_visualize.cpp -o $@

visualize_d.x: test_visualize.cpp
	$(CC) $(RELEASEFLAGS) test_visualize.cpp $(LDFLAGS) -o $@


clean:
	rm *.x

