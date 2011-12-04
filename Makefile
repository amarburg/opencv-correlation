
BIN = test_correlation 
OBJS = 

CFLAGS = -Wall 
LDFLAGS = -lopencv_core -lopencv_highgui
VPATH = lib

# If OpenCV is installed in a non-standard location
# OPENCV_ROOT = /my/home/directory/usr
# CFLAGS += -I$(OPENCV_ROOT)/include
# LDFLAGS += -L$(OPENCV_ROOT)/lib

all: $(BIN)

run:
	./test_correlation -v IMG_7088.JPG

$(BIN) : test_correlation.o $(OBJS)
	$(CC) $(LDFLAGS) $< -o $@

%.o : %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(BIN) *.o
