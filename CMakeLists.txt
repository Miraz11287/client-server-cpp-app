cmake_minimum_required(VERSION 3.10)
project(ClientServerApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем директорию с заголовочными файлами
include_directories(include)

# Настройка для Windows
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lws2_32")
endif()

# Исходные файлы сервера
set(SERVER_SOURCES
    src/server/main.cpp
    src/server/Server.cpp
    src/server/ClientHandler.cpp
    src/common/Message.cpp
    src/common/User.cpp
)

# Исходные файлы клиента
set(CLIENT_SOURCES
    src/client/main.cpp
    src/client/Client.cpp
    src/common/Message.cpp
    src/common/User.cpp
)

# Создание исполняемого файла сервера
add_executable(server ${SERVER_SOURCES})

# Создание исполняемого файла клиента
add_executable(client ${CLIENT_SOURCES})

# Подключение библиотек для Windows
if(WIN32)
    target_link_libraries(server ws2_32)
    target_link_libraries(client ws2_32)
endif()

# Установка заголовочных файлов
install(DIRECTORY include/ DESTINATION include)
