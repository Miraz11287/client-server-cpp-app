#include "common/User.h"
#include <regex>
#include <algorithm>

User::User() 
    : m_id(-1), m_status(Status::OFFLINE) {
}

User::User(int id, const std::string& username, const std::string& email)
    : m_id(id), m_username(username), m_email(email), m_status(Status::OFFLINE) {
}

void User::addContact(int contactId) {
    if (!hasContact(contactId)) {
        m_contacts.push_back(contactId);
    }
}

bool User::removeContact(int contactId) {
    auto it = std::find(m_contacts.begin(), m_contacts.end(), contactId);
    if (it != m_contacts.end()) {
        m_contacts.erase(it);
        return true;
    }
    return false;
}

bool User::hasContact(int contactId) const {
    return std::find(m_contacts.begin(), m_contacts.end(), contactId) != m_contacts.end();
}

std::string User::statusToString(Status status) {
    switch (status) {
        case Status::OFFLINE: return "OFFLINE";
        case Status::ONLINE: return "ONLINE";
        case Status::BUSY: return "BUSY";
        case Status::AWAY: return "AWAY";
        default: return "UNKNOWN";
    }
}

User::Status User::stringToStatus(const std::string& statusStr) {
    if (statusStr == "OFFLINE") return Status::OFFLINE;
    if (statusStr == "ONLINE") return Status::ONLINE;
    if (statusStr == "BUSY") return Status::BUSY;
    if (statusStr == "AWAY") return Status::AWAY;
    return Status::OFFLINE; // По умолчанию
}

bool User::isValidEmail(const std::string& email) {
    // Простая проверка email с помощью регулярного выражения
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailRegex);
}

bool User::isValidUsername(const std::string& username) {
    // Имя пользователя должно быть от 3 до 20 символов
    // и содержать только буквы, цифры и подчеркивания
    if (username.length() < 3 || username.length() > 20) {
        return false;
    }
    
    std::regex usernameRegex(R"([a-zA-Z0-9_]+)");
    return std::regex_match(username, usernameRegex);
}
