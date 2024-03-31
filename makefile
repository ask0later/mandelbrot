CFLAGS = -O3
СC = g++
TARGET = main
IFLAGS = -I./include/

SRC_FOLDER = ./src/
OBJ_FOLDER = ./obj/

SRC = $(wildcard $(SRC_FOLDER)*.cpp)
OBJ = $(patsubst $(SRC_FOLDER)%.cpp, $(OBJ_FOLDER)%.o, $(SRC))

$(TARGET) : $(OBJ)
	@$(CXX) $(IFLAGS) $(CFLAGS) $(OBJ) -o $(TARGET) -lSDL2 -lSDL2_ttf

$(OBJ_FOLDER)%.o : $(SRC_FOLDER)%.cpp
	@mkdir -p $(@D)
	@$(CXX) -mavx2 -mavx $(IFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm $(TARGET) $(OBJ)
