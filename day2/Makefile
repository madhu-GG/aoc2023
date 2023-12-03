WSROOT:=../..
EXEC_NAME=d2p1
SRC:=${PWD}/*.cpp
INCDIR=$(WSROOT)/utils
TARGET=$(WSROOT)/bin/$(EXEC_NAME)

INC=-I$(INCDIR)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INC)
