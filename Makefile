CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall

SRCS = main.cpp instance.cpp solution.cpp decode.cpp results.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = hgtsa

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET) results_f*.csv
