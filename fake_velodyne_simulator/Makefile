CC					= gcc
CXX         = g++
SRCROOT     = $(CURDIR)
OBJDIR      = $(CURDIR)/obj
INCLUDES    = -I$(CURDIR)
COMMONFLAGS = -Wall -g -o
CFLAGS      = $(INCLUDES) $(COMMONFLAGS) -Os
CXXFLAGS    = $(INCLUDES) $(COMMONFLAGS) -Os
TARGET      = $(CURDIR)/fake_velodyne_simulator
OBJS        = $(OBJDIR)/fake_velodyne_simulator.o $(OBJDIR)/interface.o $(OBJDIR)/main.o
PACKAGE     = `pkg-config --cflags --libs gtk+-3.0`
LDFLAGS     = $(PACKAGE) -export-dynamic
LDFLAGS     += -lstdc++ -lboost_system -lboost_filesystem -lboost_thread -lcpprest -lcrypto -lm

.PHONY : target
target: $(TARGET)

$(CURDIR)/fake_velodyne_simulator: $(OBJS)
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Build completed: $(notdir $@)"
	
.PHONY : clean
clean:
	@-rm -rf $(CURDIR)/obj

$(OBJS): | $(CURDIR)/obj

$(CURDIR)/obj:
	@mkdir -p $@

# Pattern rules
$(OBJDIR)/%.o: %.c
	@$(CC) -c $(CFLAGS) $(PACKAGE) $< -o $@

$(OBJDIR)/%.o: %.cpp
	@$(CXX) -c $(CXXFLAGS) $< -o $@
