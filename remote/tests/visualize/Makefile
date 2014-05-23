CC = g++
CFLAGS = -I. -I../../include -I../../../common/include \
		-I../../../quadcopter/include
DEBUGFLAGS = $(CFLAGS) -g -D_DEBUG
RELEASEFLAGS = $(CFLAGS)
LDFLAGS_DEBUG = -L../../lib -lSDL2 -lGL -lGLU -lfreetype -lcommon_d \
		-lremote_d
LDFLAGS_RELEASE = -L../../lib -lSDL2 -lGL -lGLU -lfreetype -lcommon -lremote

.PHONY: all dirs release debug clean

all: dirs debug

release: visualize.x
debug: visualize_d.x

dirs: graphics/obj lib/
graphics/obj/:
	mkdir -p graphics/obj
lib/:
	mkdir -p lib

visualize.x: test_visualize.cpp lib/libgraphics.a
	$(CC) $(RELEASEFLAGS) test_visualize.cpp lib/libgraphics.a \
		$(LDFLAGS_RELEASE) -o $@

visualize_d.x: test_visualize.cpp lib/libgraphics_d.a
	$(CC) $(DEBUGFLAGS) test_visualize.cpp lib/libgraphics_d.a \
		$(LDFLAGS_DEBUG) -o $@


# Graphics

lib/libgraphics.a: graphics/obj/font_d.o graphics/obj/image_d.o
#		graphics/obj/loadimage_d.o
	ar rcs $@ $^

graphics/obj/font.o: graphics/font.cpp graphics/font.h
	$(CC) -c $(RELEASEFLAGS) graphics/font.cpp -o $@

graphics/obj/image.o: graphics/image.cpp graphics/image.h
	$(CC) -c $(RELEASEFLAGS) graphics/image.cpp -o $@

#graphics/obj/loadimage.o: graphics/loadimage.cpp graphics/loadimage.h
#	$(CC) -c $(RELEASEFLAGS) graphics/loadimage.cpp -o $@


lib/libgraphics_d.a: graphics/obj/font_d.o graphics/obj/image_d.o
#		graphics/obj/loadimage_d.o
	ar rcs $@ $^

graphics/obj/font_d.o: graphics/font.cpp graphics/font.h
	$(CC) -c $(DEBUGFLAGS) graphics/font.cpp -o $@

graphics/obj/image_d.o: graphics/image.cpp graphics/image.h
	$(CC) -c $(DEBUGFLAGS) graphics/image.cpp -o $@

#graphics/obj/loadimage_d.o: graphics/loadimage.cpp graphics/loadimage.h
#	$(CC) -c $(DEBUGFLAGS) graphics/loadimage.cpp -o $@


clean:
	rm -rf *.x graphics/obj lib

