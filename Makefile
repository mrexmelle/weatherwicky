#CROSS_COMPILE=ppc_6xx-
CROSS_COMPILE=

#compiler
CXX = g++
CC = gcc
LDXX = g++
LD = gcc
AR = ar

#Qt paths
QT_PATH = /usr/local/Trolltech/Qt-4.7.1

OUTPUT = weatherwicky
MAIN_C_SOURCES=$(shell find src -name "*.c" | grep -v main_goocurl\.c$)
MAIN_C_OBJECTS=$(MAIN_C_SOURCES:.c=.o)
MAIN_CPP_SOURCES=$(shell find src -name "*.cpp")
MAIN_CPP_OBJECTS=$(MAIN_CPP_SOURCES:.cpp=.o)
MAIN_MOC_SOURCES=src/widget/moc_weatherwidget.cpp
MAIN_MOC_OBJECTS=src/widget/moc_weatherwidget.o

MAIN_APP_OBJECTS=${MAIN_C_OBJECTS} ${MAIN_CPP_OBJECTS}

INCLUDE= -Isrc -Isrc/curler -Isrc/widget	\
	-I$(QT_PATH)/include -I$(QT_PATH)/include/QtCore -I$(QT_PATH)/include/QtGui	\
	-I/usr/include/libxml2

LINK= 	-L$(QT_PATH)/lib -lQtCore -lQtGui -lcurl -lxml2

DEFINES= -DQT_CORE_LIB -DQT_GUI_LIB

CFLAGS=

CPPFLAGS=

all : $(OUTPUT)

${OUTPUT}: ${MAIN_MOC_OBJECTS} ${MAIN_APP_OBJECTS} ${MAIN_MOC_SOURCES}
	$(CROSS_COMPILE)$(CXX) $(LINK) ${MAIN_APP_OBJECTS} $(DEFINES) -o ${OUTPUT}

%.o : %.c 
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(INCLUDE) $(DEFINES) -c $< -o $@
 
%.o : %.cpp
	$(CROSS_COMPILE)$(CXX) $(CPPFLAGS) $(INCLUDE) $(DEFINES) -c $< -o $@

src/widget/moc_%.cpp : src/widget/%.h
	$(QT_PATH)/bin/moc $(INCLUDE) $(DEFINES) $< -o $@

clean:
	rm -f ${MAIN_APP_OBJECTS} ${MAIN_MOC_OBJECTS} ${MAIN_MOC_SOURCES} $(OUTPUT)
