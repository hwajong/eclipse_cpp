CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

TARGET =	q9_9

TARGET2 =	q10_4

$(TARGET):	$(TARGET).o
	$(CXX) -o $(TARGET) $(TARGET).o
	
$(TARGET2):	$(TARGET2).o
	$(CXX) -o $(TARGET2) $(TARGET2).o
	

all:	$(TARGET) $(TARGET2)

clean:
	rm -f $(OBJS) $(TARGET)
