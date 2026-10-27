CXX = g++ 
CXXFLAGS = -Wall -Wextra -std=c++17

torneo: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp juego/juego.cpp -o torneo

clean:
	rm -f torneo