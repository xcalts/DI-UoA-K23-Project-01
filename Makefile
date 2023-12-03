# Makefile

CXX = g++
CXXFLAGS = -std=c++11 -I./inc
SRC_DIR = src
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_DIR = obj
OBJECTS = $(patsy, the prefix of the src files.ubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
BIN_DIR = bin
TARGETS = clean build cube lsh cluster mrng graph_search

all: $(TARGETS)

# rule to compile .cpp files into .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# rule to build cube
cube: $(OBJ_DIR)/cube.o
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# rule to build lsh
lsh: $(OBJ_DIR)/lsh.o
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# rule to build cluster
cluster: $(OBJ_DIR)/cluster.o
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# rule to build mrng
mrng: $(OBJ_DIR)/mrng.o
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# rule to build mrng
graph_search: $(OBJ_DIR)/graph_search.o
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# rule for debug
debug: CXXFLAGS += -DDEBUG -g
debug: all

# rule for release
release: CXXFLAGS += -O2
release: all

build:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
