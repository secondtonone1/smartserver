CXXFLAGS = -I../Include -I../Msg -I../MsgHandler -I../NetWorkSystem -I../ProtoBuf -I../ -lpthread -lprotobuf -levent
TARGET = smartserver
DIRS = ../MsgHandler ../NetWorkSystem ../ ./
FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
OBJS = $(patsubst %.cpp, %.o ,$(FILES))

PROTOBUFOBJS = $(patsubst %.cc, %.o, $(wildcard ../ProtoBuf/*.cc))

$(TARGET):$(OBJS) $(PROTOBUFOBJS) 
	$(CXX) -g -o  $(TARGET) $^ $(CXXFLAGS)

#$(OBJS):$(FILES)
#	$(CXX) -g -c $< -o $@ 

$(PROTOBUFOBJS):$(wildcard ../ProtoBuf/*.cc)
	$(CXX) -g -c $< -o $@ 
	
clean:
	-$(RM) $(TARGET)
	-$(RM) $(OBJS)
