CC					= gcc
CXX         = g++
SRCROOT     = $(CURDIR)
OBJDIR      = $(CURDIR)/obj
INCLUDES    = -I$(CURDIR)
COMMONFLAGS = -Wall -g -o
CFLAGS      = $(INCLUDES) $(COMMONFLAGS) -Os
CXXFLAGS    = $(INCLUDES) $(COMMONFLAGS) -Os
TARGET      = $(CURDIR)/fake_imu_simulator
OBJS        = $(OBJDIR)/fake_imu_simulator.o $(OBJDIR)/main.o
PACKAGE     = `pkg-config --cflags --libs gtk+-3.0`
LDFLAGS     = $(PACKAGE) -export-dynamic
LDFLAGS     += -lstdc++ -lboost_system -lboost_filesystem

.PHONY : target
target: $(TARGET)

$(CURDIR)/fake_imu_simulator: $(OBJS)
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
