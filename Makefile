# Компилятор
CXX = g++
# Флаги компиляции
CXXFLAGS = -std=c++17 -Wall -Wextra

# Линковка с необходимыми библиотеками
LDFLAGS = -lmysqlclient

# Объектные файлы
OBJS = main.o server.o db_manager.o network.o user.o message.o chat.o logger.o

# Название целевого файла
TARGET = chat_app

# Правила сборки
all: $(TARGET)

$(TARGET): $(OBJS)
    $(CXX) -o $@ $^ $(LDFLAGS)

# Правила для каждого файла
main.o: main.cpp
    $(CXX) $(CXXFLAGS) -c main.cpp

server.o: src/server.cpp include/server.h
    $(CXX) $(CXXFLAGS) -c src/server.cpp

db_manager.o: src/db_manager.cpp include/db_manager.h
    $(CXX) $(CXXFLAGS) -c src/db_manager.cpp

network.o: src/network.cpp include/network.h
    $(CXX) $(CXXFLAGS) -c src/network.cpp

user.o: src/user.cpp include/user.h
    $(CXX) $(CXXFLAGS) -c src/user.cpp

message.o: src/message.cpp include/message.h
    $(CXX) $(CXXFLAGS) -c src/message.cpp

chat.o: src/chat.cpp include/chat.h
    $(CXX) $(CXXFLAGS) -c src/chat.cpp

logger.o: src/logger.cpp include/logger.h
    $(CXX) $(CXXFLAGS) -c src/logger.cpp

client.o: src/client.cpp include/client.h
    $(CXX) $(CXXFLAGS) -c src/client.cpp

# Очистка
clean:
    rm -f *.o $(TARGET)