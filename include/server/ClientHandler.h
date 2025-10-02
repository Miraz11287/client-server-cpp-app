#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <memory>
#include <thread>
#include <atomic>
#include "common/User.h"
#include "common/Message.h"

#ifdef _WIN32
    #include <winsock2.h>
    typedef SOCKET socket_t;
#else
    #include <sys/socket.h>
    typedef int socket_t;
#endif

/**
 * @brief Класс для обработки отдельного клиентского подключения
 * 
 * Этот класс инкапсулирует логику работы с конкретным клиентом,
 * включая прием и отправку сообщений.
 */
class ClientHandler {
public:
    /**
     * @brief Конструктор обработчика клиента
     * @param clientSocket Сокет клиента
     * @param clientId Уникальный ID клиента
     */
    ClientHandler(socket_t clientSocket, int clientId);

    /**
     * @brief Деструктор
     */
    ~ClientHandler();

    /**
     * @brief Запуск обработки клиента
     */
    void start();

    /**
     * @brief Остановка обработки клиента
     */
    void stop();

    /**
     * @brief Проверка активности клиента
     * @return true если клиент активен
     */
    bool isActive() const { return m_active; }

    /**
     * @brief Получение ID клиента
     * @return ID клиента
     */
    int getClientId() const { return m_clientId; }

    /**
     * @brief Получение пользователя
     * @return Указатель на пользователя
     */
    std::shared_ptr<User> getUser() const { return m_user; }

    /**
     * @brief Установка пользователя
     * @param user Указатель на пользователя
     */
    void setUser(std::shared_ptr<User> user) { m_user = user; }

    /**
     * @brief Отправка сообщения клиенту
     * @param message Сообщение для отправки
     * @return true если сообщение отправлено успешно
     */
    bool sendMessage(const Message& message);

    /**
     * @brief Установка обработчика входящих сообщений
     * @param handler Функция-обработчик
     */
    void setMessageHandler(std::function<void(int, const Message&)> handler);

private:
    /**
     * @brief Основной цикл обработки клиента
     */
    void clientLoop();

    /**
     * @brief Обработка входящего сообщения
     * @param message Сообщение
     */
    void processIncomingMessage(const Message& message);

    /**
     * @brief Отправка ответа клиенту
     * @param message Сообщение-ответ
     */
    void sendResponse(const Message& message);

    /**
     * @brief Обработка ошибок сети
     * @param error Код ошибки
     */
    void handleNetworkError(int error);

    socket_t m_clientSocket;                        ///< Сокет клиента
    int m_clientId;                                 ///< ID клиента
    std::atomic<bool> m_active;                     ///< Флаг активности
    std::thread m_clientThread;                     ///< Поток обработки клиента
    std::shared_ptr<User> m_user;                   ///< Пользователь
    std::function<void(int, const Message&)> m_messageHandler; ///< Обработчик сообщений
};

#endif // CLIENTHANDLER_H
