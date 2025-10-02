#include "server/Server.h"
#include <iostream>
#include <cstring>

Server::Server(int port) 
    : m_port(port), m_serverSocket(INVALID_SOCKET), m_running(false), 
      m_nextClientId(1), m_nextUserId(1) {
}

Server::~Server() {
    stop();
    cleanupNetwork();
}

bool Server::start() {
    if (m_running) {
        return true;
    }
    
    if (!initializeNetwork()) {
        std::cerr << "Ошибка инициализации сети" << std::endl;
        return false;
    }
    
    // Создание сокета
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        return false;
    }
    
    // Настройка адреса сервера
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(m_port);
    
    // Привязка сокета к адресу
    if (bind(m_serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка привязки сокета к адресу" << std::endl;
        cleanupNetwork();
        return false;
    }
    
    // Начало прослушивания
    if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Ошибка начала прослушивания" << std::endl;
        cleanupNetwork();
        return false;
    }
    
    m_running = true;
    m_serverThread = std::thread(&Server::serverLoop, this);
    
    std::cout << "Сервер запущен на порту " << m_port << std::endl;
    return true;
}

void Server::stop() {
    if (!m_running) {
        return;
    }
    
    m_running = false;
    
    // Закрытие сокета сервера
    if (m_serverSocket != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(m_serverSocket);
#else
        close(m_serverSocket);
#endif
        m_serverSocket = INVALID_SOCKET;
    }
    
    // Ожидание завершения потока сервера
    if (m_serverThread.joinable()) {
        m_serverThread.join();
    }
    
    // Закрытие всех клиентских сокетов
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    for (auto& client : m_clients) {
#ifdef _WIN32
        closesocket(client.second);
#else
        close(client.second);
#endif
    }
    m_clients.clear();
    
    // Ожидание завершения всех клиентских потоков
    for (auto& thread : m_clientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    m_clientThreads.clear();
    
    std::cout << "Сервер остановлен" << std::endl;
}

size_t Server::getClientCount() const {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    return m_clients.size();
}

bool Server::sendMessage(int clientId, const Message& message) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    auto it = m_clients.find(clientId);
    if (it == m_clients.end()) {
        return false;
    }
    
    std::string serializedMessage = message.serialize();
    int result = send(it->second, serializedMessage.c_str(), serializedMessage.length(), 0);
    return result != SOCKET_ERROR;
}

void Server::broadcastMessage(const Message& message) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    std::string serializedMessage = message.serialize();
    
    for (auto& client : m_clients) {
        send(client.second, serializedMessage.c_str(), serializedMessage.length(), 0);
    }
}

int Server::registerUser(const User& user) {
    int userId = m_nextUserId++;
    auto newUser = std::make_shared<User>(user);
    newUser->setId(userId);
    m_users[userId] = newUser;
    return userId;
}

int Server::authenticateUser(const std::string& username, const std::string& password) {
    // Простая аутентификация (в реальном приложении нужно использовать хеширование)
    for (auto& user : m_users) {
        if (user.second->getUsername() == username) {
            // В реальном приложении здесь должна быть проверка хеша пароля
            return user.first;
        }
    }
    return -1;
}

std::shared_ptr<User> Server::getUser(int userId) {
    auto it = m_users.find(userId);
    if (it != m_users.end()) {
        return it->second;
    }
    return nullptr;
}

void Server::setMessageHandler(std::function<void(int, const Message&)> handler) {
    m_messageHandler = handler;
}

void Server::serverLoop() {
    while (m_running) {
        sockaddr_in clientAddr{};
        int clientAddrLen = sizeof(clientAddr);
        
        socket_t clientSocket = accept(m_serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            if (m_running) {
                std::cerr << "Ошибка принятия подключения" << std::endl;
            }
            continue;
        }
        
        std::cout << "Новое подключение от " << inet_ntoa(clientAddr.sin_addr) << std::endl;
        
        // Создание нового потока для обработки клиента
        int clientId = m_nextClientId++;
        {
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            m_clients[clientId] = clientSocket;
        }
        
        m_clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
    }
}

void Server::handleClient(socket_t clientSocket) {
    char buffer[1024];
    int clientId = -1;
    
    // Находим ID клиента по сокету
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        for (auto& client : m_clients) {
            if (client.second == clientSocket) {
                clientId = client.first;
                break;
            }
        }
    }
    
    if (clientId == -1) {
        return;
    }
    
    while (m_running) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            break;
        }
        
        buffer[bytesReceived] = '\0';
        std::string messageData(buffer);
        
        Message message;
        if (message.deserialize(messageData)) {
            processMessage(clientId, message);
        }
    }
    
    // Удаление клиента при отключении
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        m_clients.erase(clientId);
    }
    
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}

void Server::processMessage(int clientId, const Message& message) {
    if (m_messageHandler) {
        m_messageHandler(clientId, message);
    }
    
    // Обработка различных типов сообщений
    switch (message.getType()) {
        case Message::Type::LOGIN: {
            // Обработка входа в систему
            break;
        }
        case Message::Type::LOGOUT: {
            // Обработка выхода из системы
            break;
        }
        case Message::Type::TEXT: {
            // Пересылка текстового сообщения
            if (message.getReceiverId() != -1) {
                sendMessage(message.getReceiverId(), message);
            } else {
                broadcastMessage(message);
            }
            break;
        }
        default:
            break;
    }
}

bool Server::initializeNetwork() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return result == 0;
#else
    return true;
#endif
}

void Server::cleanupNetwork() {
#ifdef _WIN32
    WSACleanup();
#endif
}
