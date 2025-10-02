#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>
#include "common/User.h"
#include "common/Message.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET socket_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    typedef int socket_t;
#endif

/**
 * @brief Класс клиента для подключения к серверу
 * 
 * Этот класс реализует клиентскую часть приложения,
 * обеспечивая подключение к серверу и обмен сообщениями.
 */
class Client {
public:
    /**
     * @brief Конструктор клиента
     */
    Client();

    /**
     * @brief Деструктор клиента
     */
    ~Client();

    /**
     * @brief Подключение к серверу
     * @param serverAddress Адрес сервера
     * @param port Порт сервера
     * @return true если подключение успешно
     */
    bool connect(const std::string& serverAddress, int port = 8080);

    /**
     * @brief Отключение от сервера
     */
    void disconnect();

    /**
     * @brief Проверка статуса подключения
     * @return true если подключен
     */
    bool isConnected() const { return m_connected; }

    /**
     * @brief Отправка сообщения на сервер
     * @param message Сообщение для отправки
     * @return true если сообщение отправлено успешно
     */
    bool sendMessage(const Message& message);

    /**
     * @brief Отправка текстового сообщения
     * @param content Содержимое сообщения
     * @param receiverId ID получателя (опционально)
     * @return true если сообщение отправлено успешно
     */
    bool sendTextMessage(const std::string& content, int receiverId = -1);

    /**
     * @brief Вход в систему
     * @param username Имя пользователя
     * @param password Пароль
     * @return true если вход успешен
     */
    bool login(const std::string& username, const std::string& password);

    /**
     * @brief Выход из системы
     * @return true если выход успешен
     */
    bool logout();

    /**
     * @brief Регистрация нового пользователя
     * @param username Имя пользователя
     * @param email Email пользователя
     * @param password Пароль
     * @return true если регистрация успешна
     */
    bool registerUser(const std::string& username, const std::string& email, const std::string& password);

    /**
     * @brief Получение текущего пользователя
     * @return Указатель на пользователя
     */
    std::shared_ptr<User> getCurrentUser() const { return m_currentUser; }

    /**
     * @brief Установка обработчика входящих сообщений
     * @param handler Функция-обработчик
     */
    void setMessageHandler(std::function<void(const Message&)> handler);

    /**
     * @brief Установка обработчика ошибок
     * @param handler Функция-обработчик
     */
    void setErrorHandler(std::function<void(const std::string&)> handler);

    /**
     * @brief Получение ID клиента
     * @return ID клиента
     */
    int getClientId() const { return m_clientId; }

private:
    /**
     * @brief Основной цикл приема сообщений
     */
    void receiveLoop();

    /**
     * @brief Обработка входящего сообщения
     * @param message Сообщение
     */
    void processIncomingMessage(const Message& message);

    /**
     * @brief Инициализация сетевой библиотеки
     * @return true если инициализация успешна
     */
    bool initializeNetwork();

    /**
     * @brief Очистка сетевых ресурсов
     */
    void cleanupNetwork();

    socket_t m_socket;                               ///< Сокет клиента
    std::atomic<bool> m_connected;                   ///< Флаг подключения
    std::thread m_receiveThread;                     ///< Поток приема сообщений
    std::shared_ptr<User> m_currentUser;             ///< Текущий пользователь
    std::function<void(const Message&)> m_messageHandler; ///< Обработчик сообщений
    std::function<void(const std::string&)> m_errorHandler; ///< Обработчик ошибок
    int m_clientId;                                  ///< ID клиента
    std::string m_serverAddress;                     ///< Адрес сервера
    int m_serverPort;                                ///< Порт сервера
};

#endif // CLIENT_H
