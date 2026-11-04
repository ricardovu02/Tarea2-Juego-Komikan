CXX = g++ 
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread

torneo: main.cpp juego/juego.cpp juego/juego.h partida/partida.cpp partida/partida.h arbitro/arbitro.cpp arbitro/arbitro.h matchmaking/torneo.cpp matchmaking/torneo.h
	$(CXX) $(CXXFLAGS) main.cpp juego/juego.cpp partida/partida.cpp arbitro/arbitro.cpp matchmaking/torneo.cpp -o torneo

clean:
	rm -f torneo