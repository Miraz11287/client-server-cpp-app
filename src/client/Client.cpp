#include "client/Client.h"
#include <iostream>
#include <cstring>

Client::Client() 
    : m_socket(INVALID_SOCKET), m_connected(false), m_clientId(-1) {
}

Client::~Client() {
    disconnect();
    cleanupNetwork();
}

bool Client::connect(const std::string& serverAddress, int port) {
    if (m_connected) {
        return true;
    }
    
    if (!initializeNetwork()) {
        std::cerr << "Ошибка инициализации сети" << std::endl;
        return false;
    }
    
    // Создание сокета
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        return false;
    }
    
    // Настройка адреса сервера
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Неверный адрес сервера" << std::endl;
        return false;
    }
    
    // Подключение к серверу
    if (::connect(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка подключения к серверу" << std::endl;
        return false;
    }
    
    m_connected = true;
    m_serverAddress = serverAddress;
    m_serverPort = port;
    
    // Запуск потока приема сообщений
    m_receiveThread = std::thread(&Client::receiveLoop, this);
    
    std::cout << "Подключение к серверу " << serverAddress << ":" << port << " установлено" << std::endl;
    return true;
}

void Client::disconnect() {
    if (!m_connected) {
        return;
    }
    
    m_connected = false;
    
    // Закрытие сокета
    if (m_socket != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(m_socket);
#else
        close(m_socket);
#endif
        m_socket = INVALID_SOCKET;
    }
    
    // Ожидание завершения потока приема сообщений
    if (m_receiveThread.joinable()) {
        m_receiveThread.join();
    }
    
    std::cout << "Отключение от сервера" << std::endl;
}

bool Client::sendMessage(const Message& message) {
    if (!m_connected) {
        return false;
    }
    
    std::string serializedMessage = message.serialize();
    int result = send(m_socket, serializedMessage.c_str(), serializedMessage.length(), 0);
    return result != SOCKET_ERROR;
}

bool Client::sendTextMessage(const std::string& content, int receiverId) {
    if (!m_currentUser) {
        return false;
    }
    
    Message message(Message::Type::TEXT, content, m_currentUser->getId(), receiverId);
    return sendMessage(message);
}

bool Client::login(const std::string& username, const std::string& password) {
    if (!m_connected) {
        return false;
    }
    
    // Создание сообщения для входа
    std::string loginData = username + ":" + password;
    Message loginMessage(Message::Type::LOGIN, loginData, -1);
    
    if (!sendMessage(loginMessage)) {
        return false;
    }
    
    // В реальном приложении здесь должно быть ожидание ответа от сервера
    // Для простоты создаем пользователя локально
    m_currentUser = std::make_shared<User>(1, username, username + "@example.com");
    m_currentUser->setStatus(User::Status::ONLINE);
    
    return true;
}

bool Client::logout() {
    if (!m_connected || !m_currentUser) {
        return false;
    }
    
    Message logoutMessage(Message::Type::LOGOUT, "", m_currentUser->getId());
    bool result = sendMessage(logoutMessage);
    
    if (result) {
        m_currentUser->setStatus(User::Status::OFFLINE);
        m_currentUser.reset();
    }
    
    return result;
}

bool Client::registerUser(const std::string& username, const std::string& email, const std::string& password) {
    if (!m_connected) {
        return false;
    }
    
    // В реальном приложении здесь должна быть отправка данных регистрации на сервер
    // Для простоты создаем пользователя локально
    m_currentUser = std::make_shared<User>(1, username, email);
    m_currentUser->setStatus(User::Status::ONLINE);
    
    return true;
}

void Client::setMessageHandler(std::function<void(const Message&)> handler) {
    m_messageHandler = handler;
}

void Client::setErrorHandler(std::function<void(const std::string&)> handler) {
    m_errorHandler = handler;
}

void Client::receiveLoop() {
    char buffer[1024];
    
    while (m_connected) {
        int bytesReceived = recv(m_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            if (m_connected) {
                std::cout << "Соединение с сервером потеряно" << std::endl;
                if (m_errorHandler) {
                    m_errorHandler("Соединение с сервером потеряно");
                }
            }
            break;
        }
        
        buffer[bytesReceived] = '\0';
        std::string messageData(buffer);
        
        Message message;
        if (message.deserialize(messageData)) {
            processIncomingMessage(message);
        }
    }
}

void Client::processIncomingMessage(const Message& message) {
    if (m_messageHandler) {
        m_messageHandler(message);
    }
    
    // Обработка различных типов сообщений
    switch (message.getType()) {
        case Message::Type::TEXT: {
            std::cout << "Получено сообщение: " << message.getContent() << std::endl;
            break;
        }
        case Message::Type::ERROR: {
            std::cout << "Ошибка от сервера: " << message.getContent() << std::endl;
            if (m_errorHandler) {
                m_errorHandler(message.getContent());
            }
            break;
        }
        default:
            break;
    }
}

bool Client::initializeNetwork() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return result == 0;
#else
    return true;
#endif
}

void Client::cleanupNetwork() {
#ifdef _WIN32
    WSACleanup();
#endif
}
