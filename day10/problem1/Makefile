WSROOT:=../..
EXEC_NAME:=d10p1
SRC:=${PWD}/*.cpp
INCDIR=$(WSROOT)/utils
TARGET=$(WSROOT)/bin/$(EXEC_NAME)

INC=-I$(INCDIR)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INC)

debug: CXXFLAGS +=-g
debug: $(TARGET)

all: $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean
