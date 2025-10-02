#include "common/Message.h"
#include <sstream>
#include <iomanip>

Message::Message() 
    : m_type(Type::TEXT), m_senderId(-1), m_receiverId(-1), m_timestamp(std::chrono::system_clock::now()) {
}

Message::Message(Type type, const std::string& content, int senderId, int receiverId)
    : m_type(type), m_content(content), m_senderId(senderId), m_receiverId(receiverId), 
      m_timestamp(std::chrono::system_clock::now()) {
}

std::string Message::serialize() const {
    std::ostringstream oss;
    
    // Конвертируем timestamp в строку
    auto time_t = std::chrono::system_clock::to_time_t(m_timestamp);
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    
    // Формат: TYPE|SENDER_ID|RECEIVER_ID|TIMESTAMP|CONTENT
    oss << typeToString(m_type) << "|" 
        << m_senderId << "|" 
        << m_receiverId << "|" 
        << oss.str() << "|" 
        << m_content;
    
    return oss.str();
}

bool Message::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    std::vector<std::string> tokens;
    
    // Разделяем строку по символу '|'
    while (std::getline(iss, token, '|')) {
        tokens.push_back(token);
    }
    
    // Проверяем количество токенов
    if (tokens.size() < 5) {
        return false;
    }
    
    try {
        m_type = stringToType(tokens[0]);
        m_senderId = std::stoi(tokens[1]);
        m_receiverId = std::stoi(tokens[2]);
        
        // Парсим timestamp
        std::tm tm = {};
        std::istringstream timeStream(tokens[3]);
        timeStream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        m_timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        
        m_content = tokens[4];
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string Message::typeToString(Type type) {
    switch (type) {
        case Type::LOGIN: return "LOGIN";
        case Type::LOGOUT: return "LOGOUT";
        case Type::TEXT: return "TEXT";
        case Type::FILE: return "FILE";
        case Type::STATUS: return "STATUS";
        case Type::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

Message::Type Message::stringToType(const std::string& typeStr) {
    if (typeStr == "LOGIN") return Type::LOGIN;
    if (typeStr == "LOGOUT") return Type::LOGOUT;
    if (typeStr == "TEXT") return Type::TEXT;
    if (typeStr == "FILE") return Type::FILE;
    if (typeStr == "STATUS") return Type::STATUS;
    if (typeStr == "ERROR") return Type::ERROR;
    return Type::TEXT; // По умолчанию
}
