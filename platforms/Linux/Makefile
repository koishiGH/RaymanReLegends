
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g -DLINUX -D__LINUX__
INCLUDES = -I../../src -I../../external -I../../external/SDL2
LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -ldl -lpthread -lstdc++fs

SRCDIR = ../../src
OBJDIR = ../../obj
BINDIR = bin

SOURCES = \
	$(SRCDIR)/main.cpp \
	$(SRCDIR)/engine/graphics/GraphicsInitializer.cpp \
	$(SRCDIR)/engine/graphics/SDL2Renderer.cpp \
	$(SRCDIR)/engine/video/Video.cpp \
	$(SRCDIR)/engine/system/config.cpp \
	$(SRCDIR)/engine/system/regconfig.cpp \
	$(SRCDIR)/engine/resource/Bundle.cpp \
	$(SRCDIR)/engine/resource/FileProperties.cpp \
	$(SRCDIR)/engine/platform/DXInit.cpp \
	$(SRCDIR)/engine/platform/window.cpp \
	../../external/tinyxml2.cpp

OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
OBJECTS := $(OBJECTS:../../external/%.cpp=$(OBJDIR)/external/%.o)

TARGET = $(BINDIR)/RaymanLegends

all: $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)/engine/graphics
	mkdir -p $(OBJDIR)/engine/video
	mkdir -p $(OBJDIR)/engine/system
	mkdir -p $(OBJDIR)/engine/resource
	mkdir -p $(OBJDIR)/engine/platform
	mkdir -p $(OBJDIR)/external

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/external/%.o: ../../external/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

install-deps:
	sudo apt-get update
	sudo apt-get install -y \
		build-essential \
		libsdl2-dev \
		libsdl2-image-dev \
		libsdl2-mixer-dev \
		libsdl2-ttf-dev \
		cmake

install-deps-arch:
	sudo pacman -S --needed \
		base-devel \
		sdl2 \
		sdl2_image \
		sdl2_mixer \
		sdl2_ttf \
		cmake

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run install-deps install-deps-arch