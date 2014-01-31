CC = g++
CFLAGS = -I../../include -I../../../common/include
DEBUGFLAGS = $(CFLAGS) -g -D_DEBUG
RELEASEFLAGS = $(CFLAGS)
LDFLAGS_D = -L../../lib -lcommon_d -lquadcopter_d
LDFLAGS_R = -L../../lib -lcommon -lquadcopter

.PHONY: all libs release debug clean

all: libs debug

release: visualize.x visualizedrive.x
debug: visualize_d.x visualizedrive_d.x

libs:
	make -C ../..


visualize.x: test_visualize.cpp ../../lib/libcommon.a \
		../../lib/libquadcopter.a
	$(CC) $(RELEASEFLAGS) test_visualize.cpp $(LDFLAGS_R) -o $@

visualize_d.x: test_visualize.cpp ../../lib/libcommon_d.a \
		../../lib/libquadcopter_d.a
	$(CC) $(DEBUGFLAGS) test_visualize.cpp $(LDFLAGS_D) -o $@


visualizedrive.x: test_visualizedrive.cpp ../../lib//libcommon.a \
		../../lib/libquadcopter.a
	$(CC) $(RELEASEFLAGS) test_visualizedrive.cpp $(LDFLAGS_R) -o $@

visualizedrive_d.x: test_visualizedrive.cpp ../../lib/libcommon_d.a \
		../../lib/libquadcopter_d.a
	$(CC) $(DEBUGFLAGS) test_visualizedrive.cpp $(LDFLAGS_D) -o $@


clean:
	rm *.x

