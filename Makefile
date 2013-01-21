
BIN = test_texture_psd
OBJS = cross_correlation.o phase_correlation.o

CFLAGS = -I/home/aaron/usr/include -Wall  -ggdb
LDFLAGS = -L/home/aaron/usr/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lfftw3 -lc
VPATH = lib

# If OpenCV is installed in a non-standard location
# OPENCV_ROOT = /my/home/directory/usr
# CFLAGS += -I$(OPENCV_ROOT)/include
# LDFLAGS += -L$(OPENCV_ROOT)/lib

CC = gcc

all: $(BIN)

run: $(BIN)
	./test_texture_psd -v IMG_7088.JPG

$(BIN) : test_texture_psd.o $(OBJS)
	$(CXX) $^ -o $@ $(CFLAGS) $(LDFLAGS) 

%.o : %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(BIN) *.o
