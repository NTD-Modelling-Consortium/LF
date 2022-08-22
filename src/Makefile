OBJS := main.o ScenariosList.o Scenario.o Population.o Host.o Model.o Output.o Vector.o Worm.o Statistics.o MDAEvent.o BedNetEvent.o PrevalenceEvent.o ImportationRateEvent.o tinyxml/tinystr.o tinyxml/tinyxmlparser.o tinyxml/tinyxmlerror.o tinyxml/tinyxml.o 

# Provide any options and flags for the compiler here
CFLAGS    = -Wall -O3 -std=c++11

# Set name of final executable file here
EXENAME = transfil

# If you are linking to external libraries, provide them here
LIBS  = -lm -lgsl -lgslcblas

# If there are libraries with non-standard paths, provide the paths here
LIBDIR = -L$(HOME)/lib

# If there are header files with non-standard paths, provide the paths here
INCLDIR = -I. -I./tinyxml -I$(HOME)/include

default:$(EXENAME)

# link to generate executable file
$(EXENAME): $(OBJS)
	g++ $(LIBDIR) $(OBJS) $(LIBS) -o $(EXENAME)

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.cpp
	gcc -c $(CFLAGS) $(INCLDIR) $*.cpp -o $*.o
	gcc -MM $(CFLAGS) $(INCLDIR) $*.cpp > $*.d

# remove compilation products
clean:
	rm -f $(EXENAME) *.o *.d
