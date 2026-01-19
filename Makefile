# Makefile dla RetroSLM

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./include
LDFLAGS = -lftxui-component -lftxui-dom -lftxui-screen
TARGET = RetroSLM

SOURCES = main.cpp src/Magazyn.cpp src/Klienci.cpp src/Faktury.cpp src/Historia.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	@echo "Kompilacja zakonczona!"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -rf build/
	@echo "Wyczyszczono pliki obiektowe i program"

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
