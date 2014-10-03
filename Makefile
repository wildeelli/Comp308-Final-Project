CC = g++
CFLAGS = -g -Wall -Wextra -pedantic 
IPATH = -I/usr/X11/include -I/usr/pkg/include
LPATH = -L/usr/X11/lib -L/usr/pkg/lib -L/lib
LDPATH = -Wl,-R/lib/
RM = rm

BUILD = ./build/
SRC = ./src/

all: $(BUILD)fp

$(BUILD)fp: $(BUILD)loadShader.o $(BUILD)main.o
	$(CC) -o $@ $^ -lm -lglfw3 -lopengl32 -lglu32 -lgdi32 -lglew32 $(LPATH) $(LDPATH)
	
$(BUILD)%.o:  $(SRC)%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)
	
linux:$(BUILD)fp-l

$(BUILD)fp-l: $(BUILD)loadShader.o $(BUILD)main.o
	$(CC) -o $@ $^ -lm -lglfw3 -lgl -lglu -lgdi -lglew $(LPATH) $(LDPATH)
	
$(BUILD)%.o:  $(SRC)%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)
	
clean:
	$(RM) -f $(BUILD)*.o $(BUILD)*.gch $(BUILD)Ass2*
