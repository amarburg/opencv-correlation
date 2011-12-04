
BIN = test_correlation 
OBJS = cross_correlation.o phase_correlation.o

CFLAGS = -Wall  -ggdb
LDFLAGS = -lopencv_core -lopencv_highgui -lfftw3
VPATH = lib

# If OpenCV is installed in a non-standard location
# OPENCV_ROOT = /my/home/directory/usr
# CFLAGS += -I$(OPENCV_ROOT)/include
# LDFLAGS += -L$(OPENCV_ROOT)/lib

all: $(BIN)

run: $(BIN)
	./test_correlation -vg 10 IMG_7088.JPG

$(BIN) : test_correlation.o $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.o : %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(BIN) *.o
