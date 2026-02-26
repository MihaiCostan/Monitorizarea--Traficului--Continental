# Definim compilatorul
CXX = clang++

# Flag-uri pentru compilare: 
# -Wall (arată toate avertismentele)
# -I (include folderul de headere)
# -std=c++11 (sau c++17 pentru nlohmann/json)
CXXFLAGS = -Wall -Iinclude -std=c++17

# Numele executabilului final
TARGET = traffic_server

# Lista fișierelor sursă
SRCS = src/main.cpp src/TCPServer.cpp src/TrafficManager.cpp

# Generăm lista fișierelor obiect (.o) din cele sursă
OBJS = $(SRCS:.cpp=.o)

# Regula principală: construiește executabilul
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Regula pentru a compila fiecare fișier .cpp în .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regula 'clean' pentru a șterge fișierele temporare
clean:
	rm -f $(OBJS) $(TARGET)

# Regula 'run' pentru a compila și rula direct
run: $(TARGET)
	./$(TARGET)