# define object files
OBJS := \
	main.o \
	BedNetEvent.o \
	Host.o \
	ImportationRateEvent.o \
	MDAEvent.o \
	Model.o \
	Output.o \
	Population.o \
	PrevalenceEvent.o \
	RecordedPrevalence.o \
	Scenario.o \
	ScenariosList.o \
	Statistics.o \
	Vector.o \
	Worm.o \
	tinyxml/tinystr.o \
	tinyxml/tinyxml.o \
	tinyxml/tinyxmlerror.o \
	tinyxml/tinyxmlparser.o

# Provide any options and flags for the compiler here
CFLAGS    = -Wall -O3 -std=c++11

# work out where to find 'gsl-config' to get GSL CFLAGS and library path
GSL_CONFIG := $(shell which gsl-config)

# Set name of final executable file here
EXENAME = transfil_N

# add the Linux C++ 'fs' library if on Linux, -lm only if on Darwin
# (as it's provided by gsl-config on Linux)
UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
	LIBS = -lm
else ifeq ($(UNAME),Linux)
	LIBS += -lstdc++fs
endif

# Get your system's packaged GSL paths
GSL_LIBS := $(shell $(GSL_CONFIG) --libs)

# Add tinyxml and system-packaged GSL include paths
INCLDIR = -I. -I./tinyxml $(shell $(GSL_CONFIG) --cflags)

# default build target
default:$(EXENAME)

# link to generate executable file
$(EXENAME): $(OBJS)
	g++ $(OBJS) $(LIBS) $(GSL_LIBS) -o $(EXENAME)

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.cpp
	g++ -c $(CFLAGS) $(INCLDIR) $*.cpp -o $*.o
	g++ -MM $(CFLAGS) $(INCLDIR) $*.cpp > $*.d

# remove compilation products
clean:
	rm -rf $(EXENAME) $(EXENAME).* *.o *.d tinyxml/*.o tinyxml/*.d
	rm -f ../run/$(EXENAME)
