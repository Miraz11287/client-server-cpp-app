#include "client/Client.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== Клиент клиент-серверного приложения ===" << std::endl;
    std::cout << "Автор: Эдуард" << std::endl;
    std::cout << "Версия: 1.0" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    Client client;
    
    // Подключение к серверу
    std::string serverAddress;
    std::cout << "Введите адрес сервера (по умолчанию 127.0.0.1): ";
    std::getline(std::cin, serverAddress);
    if (serverAddress.empty()) {
        serverAddress = "127.0.0.1";
    }
    
    if (!client.connect(serverAddress, 8080)) {
        std::cerr << "Не удалось подключиться к серверу" << std::endl;
        return 1;
    }
    
    // Установка обработчиков
    client.setMessageHandler([](const Message& message) {
        std::cout << "Получено сообщение типа " << Message::typeToString(message.getType()) 
                  << ": " << message.getContent() << std::endl;
    });
    
    client.setErrorHandler([](const std::string& error) {
        std::cerr << "Ошибка: " << error << std::endl;
    });
    
    // Меню пользователя
    std::string choice;
    while (true) {
        std::cout << "\n=== Меню ===" << std::endl;
        std::cout << "1. Войти в систему" << std::endl;
        std::cout << "2. Зарегистрироваться" << std::endl;
        std::cout << "3. Отправить сообщение" << std::endl;
        std::cout << "4. Выйти из системы" << std::endl;
        std::cout << "5. Отключиться от сервера" << std::endl;
        std::cout << "Выберите действие: ";
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            std::string username, password;
            std::cout << "Введите имя пользователя: ";
            std::getline(std::cin, username);
            std::cout << "Введите пароль: ";
            std::getline(std::cin, password);
            
            if (client.login(username, password)) {
                std::cout << "Успешный вход в систему" << std::endl;
            } else {
                std::cout << "Ошибка входа в систему" << std::endl;
            }
        }
        else if (choice == "2") {
            std::string username, email, password;
            std::cout << "Введите имя пользователя: ";
            std::getline(std::cin, username);
            std::cout << "Введите email: ";
            std::getline(std::cin, email);
            std::cout << "Введите пароль: ";
            std::getline(std::cin, password);
            
            if (client.registerUser(username, email, password)) {
                std::cout << "Успешная регистрация" << std::endl;
            } else {
                std::cout << "Ошибка регистрации" << std::endl;
            }
        }
        else if (choice == "3") {
            if (!client.getCurrentUser()) {
                std::cout << "Сначала войдите в систему" << std::endl;
                continue;
            }
            
            std::string message;
            std::cout << "Введите сообщение: ";
            std::getline(std::cin, message);
            
            if (client.sendTextMessage(message)) {
                std::cout << "Сообщение отправлено" << std::endl;
            } else {
                std::cout << "Ошибка отправки сообщения" << std::endl;
            }
        }
        else if (choice == "4") {
            if (client.logout()) {
                std::cout << "Успешный выход из системы" << std::endl;
            } else {
                std::cout << "Ошибка выхода из системы" << std::endl;
            }
        }
        else if (choice == "5") {
            break;
        }
        else {
            std::cout << "Неверный выбор" << std::endl;
        }
    }
    
    client.disconnect();
    std::cout << "Клиент завершен" << std::endl;
    return 0;
}
