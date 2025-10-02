#include "server/ClientHandler.h"
#include <iostream>
#include <cstring>

ClientHandler::ClientHandler(socket_t clientSocket, int clientId)
    : m_clientSocket(clientSocket), m_clientId(clientId), m_active(true) {
}

ClientHandler::~ClientHandler() {
    stop();
}

void ClientHandler::start() {
    if (m_active) {
        m_clientThread = std::thread(&ClientHandler::clientLoop, this);
    }
}

void ClientHandler::stop() {
    m_active = false;
    
    if (m_clientThread.joinable()) {
        m_clientThread.join();
    }
    
    if (m_clientSocket != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(m_clientSocket);
#else
        close(m_clientSocket);
#endif
        m_clientSocket = INVALID_SOCKET;
    }
}

bool ClientHandler::sendMessage(const Message& message) {
    if (!m_active || m_clientSocket == INVALID_SOCKET) {
        return false;
    }
    
    std::string serializedMessage = message.serialize();
    int result = send(m_clientSocket, serializedMessage.c_str(), serializedMessage.length(), 0);
    return result != SOCKET_ERROR;
}

void ClientHandler::setMessageHandler(std::function<void(int, const Message&)> handler) {
    m_messageHandler = handler;
}

void ClientHandler::clientLoop() {
    char buffer[1024];
    
    while (m_active) {
        int bytesReceived = recv(m_clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            if (m_active) {
                std::cout << "Клиент " << m_clientId << " отключился" << std::endl;
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
    
    m_active = false;
}

void ClientHandler::processIncomingMessage(const Message& message) {
    if (m_messageHandler) {
        m_messageHandler(m_clientId, message);
    }
    
    // Обработка различных типов сообщений
    switch (message.getType()) {
        case Message::Type::LOGIN: {
            std::cout << "Клиент " << m_clientId << " пытается войти в систему" << std::endl;
            break;
        }
        case Message::Type::LOGOUT: {
            std::cout << "Клиент " << m_clientId << " выходит из системы" << std::endl;
            break;
        }
        case Message::Type::TEXT: {
            std::cout << "Клиент " << m_clientId << " отправил сообщение: " << message.getContent() << std::endl;
            break;
        }
        default:
            break;
    }
}

void ClientHandler::sendResponse(const Message& message) {
    sendMessage(message);
}

void ClientHandler::handleNetworkError(int error) {
    std::cerr << "Сетевая ошибка для клиента " << m_clientId << ": " << error << std::endl;
    m_active = false;
}
