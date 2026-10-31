CXX = g++ 
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread

torneo: main.cpp juego/juego.cpp juego/juego.h partida/partida.cpp partida/partida.h
	$(CXX) $(CXXFLAGS) main.cpp juego/juego.cpp partida/partida.cpp -o torneo

clean:
	rm -f torneo