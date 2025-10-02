#include "server/Server.h"
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>

// Глобальная переменная для сервера
std::unique_ptr<Server> g_server;

// Обработчик сигналов для корректного завершения
void signalHandler(int signal) {
    std::cout << "\nПолучен сигнал " << signal << ". Завершение работы сервера..." << std::endl;
    if (g_server) {
        g_server->stop();
    }
    exit(0);
}

int main() {
    // Установка обработчиков сигналов
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    std::cout << "=== Сервер клиент-серверного приложения ===" << std::endl;
    std::cout << "Автор: Эдуард" << std::endl;
    std::cout << "Версия: 1.0" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // Создание и запуск сервера
    g_server = std::make_unique<Server>(8080);
    
    if (!g_server->start()) {
        std::cerr << "Не удалось запустить сервер" << std::endl;
        return 1;
    }
    
    // Установка обработчика сообщений
    g_server->setMessageHandler([](int clientId, const Message& message) {
        std::cout << "Получено сообщение от клиента " << clientId 
                  << ": " << message.getContent() << std::endl;
    });
    
    std::cout << "Сервер работает. Нажмите Ctrl+C для завершения." << std::endl;
    
    // Основной цикл сервера
    while (g_server->isRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Вывод статистики каждые 10 секунд
        static int counter = 0;
        if (++counter >= 10) {
            std::cout << "Активных подключений: " << g_server->getClientCount() << std::endl;
            counter = 0;
        }
    }
    
    return 0;
}
