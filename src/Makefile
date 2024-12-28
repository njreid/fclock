CXXFLAGS=-Wall -O3 -g -Wextra -Wno-unused-parameter

# Where our library resides
RGB_LIB_DISTRIBUTION=../../c-src/rpi-rgb-led-matrix
RGB_INCDIR=$(RGB_LIB_DISTRIBUTION)/include
RGB_LIBDIR=$(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a

# Add libconfuse and other required libraries
LDFLAGS=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread -lconfuse

SOURCES=fclock.cc text_writer.cc clock_config.cc
OBJECTS=$(SOURCES:.cc=.o)

# Output directory
DIST_DIR=../dist
TARGET=$(DIST_DIR)/fclock

all : $(TARGET)

$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR)

$(TARGET): $(OBJECTS) $(RGB_LIBRARY)
	@mkdir -p $(DIST_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

%.o : %.cc
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

install:
	@echo "Installing fclock..."
	@mkdir -p $(HOME)/bin
	@cp $(TARGET) $(HOME)/bin/
	@mkdir -p $(HOME)/bin/fonts
	@cp $(RGB_LIB_DISTRIBUTION)/fonts/*.bdf $(HOME)/bin/fonts/
	@if [ ! -f $(HOME)/.fclock.conf ]; then \
		echo "Installing default config file..."; \
		sed 's|font-path = "fonts"|font-path = "$(HOME)/bin/fonts"|' fclock.conf > $(HOME)/.fclock.conf; \
	fi
	@echo "Installation complete."

.PHONY: clean install
