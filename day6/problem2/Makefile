WSROOT:=../..
EXEC_NAME:=d6p2
SRC:=${PWD}/*.cpp
INCDIR=$(WSROOT)/utils
TARGET=$(WSROOT)/bin/$(EXEC_NAME)

INC=-I$(INCDIR)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INC)

debug: CXXFLAGS +=-DCERR_DEBUG
debug: CXXFLAGS +=-g
debug: $(TARGET)

all: $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean
