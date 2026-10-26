CXX = g++ 
CXXFLAGS = -Wall -Wextra -std=c++17

torneo: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o torneo

clean:
	rm -f torneo