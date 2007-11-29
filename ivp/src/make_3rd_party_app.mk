# Generic makefile structure for all application modules
.SILENT:

MOOS_LFLAGS = \
	-L ../../../../MOOS/MOOSBin

MOOS_IFLAGS = \
	-I ../../../../MOOS/Essentials \
	-I ../../../../MOOS/Core \
	-I ../../../../MOOS/NavigationAndControl \
	-I ../../../../MOOS/Thirdparty/FLTKVW \
	-I ../../../../MOOS/Essentials/MOOSUtilityLib \
	-I ../../../../MOOS/Core/MOOSLIB \
	-I ../../../../MOOS/Core/MOOSGenLib \
	-I ../../../../MOOS/NavigationAndControl\MOOSNavLib \
	-I ../../../../MOOS/NavigationAndControl\MOOSTaskLib \
	-I ../../../../MOOS/Thirdparty/FLTKVW


BIN_DIR  = ../../bin
TARGET   = $(BIN_DIR)/$(LTARGET)
IFLAGS   = -I. -I.. -I../.. $(patsubst %,-I%,$(PLDIRS)) $(MOOS_IFLAGS) $(EXTRA_IFLAGS)


LFLAGS   = -L/usr/X11R6/lib -L/usr/X11R6/lib64     \
	   -L/sw/lib $(patsubst %,-L%,$(PLDIRS))   \
	   $(MOOS_LFLAGS) $(EXTRA_LFLAGS)
           

SOURCES  = $(wildcard *.cpp)
OBJECTS  = $(SOURCES:.cpp=.o)
CFLAGS   = $(DEBUG_LEVEL) -Wno-deprecated  $(FP) $(LCFLAGS)

vpath %.a  $(PLDIRS) 

all:  	$(TARGET)
	makedepend > /dev/null 2>&1 ; rm -f *akefile.bak

depend:
	makedepend $(IFLAGS) $(SOURCES) > /dev/null 2>&1

clean:
	@rm -f $(TARGET) $(LTARGET)
	makedepend > /dev/null 2>&1
	@rm -f *.bak *~ $(OBJECTS)
	@echo Finished cleaning $(LTARGET)

$(TARGET): $(OBJECTS) $(PLIBS)
	@echo "Linking now....." $(LTARGET) 
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(IFLAGS) $(LFLAGS) $(LIBS) 
	@chmod 777 $(TARGET)

%.o: %.cpp
	@echo $(CXX) "compiling" $(LTARGET) "source code" $<
	$(CXX) $(CFLAGS) $(IFLAGS) -c $<

# DO NOT DELETE THIS LINE -- make depend depends on it.
