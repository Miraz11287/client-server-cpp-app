#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <map>
#include <string>
#include <functional>
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
 * @brief Класс сервера для обработки клиентских подключений
 * 
 * Этот класс реализует многопоточный сервер, который может обрабатывать
 * множественные клиентские подключения одновременно.
 */
class Server {
public:
    /**
     * @brief Конструктор сервера
     * @param port Порт для прослушивания
     */
    explicit Server(int port = 8080);

    /**
     * @brief Деструктор сервера
     */
    ~Server();

    /**
     * @brief Запуск сервера
     * @return true если сервер успешно запущен
     */
    bool start();

    /**
     * @brief Остановка сервера
     */
    void stop();

    /**
     * @brief Проверка статуса сервера
     * @return true если сервер запущен
     */
    bool isRunning() const { return m_running; }

    /**
     * @brief Получение количества подключенных клиентов
     * @return Количество активных подключений
     */
    size_t getClientCount() const;

    /**
     * @brief Отправка сообщения конкретному клиенту
     * @param clientId ID клиента
     * @param message Сообщение для отправки
     * @return true если сообщение отправлено успешно
     */
    bool sendMessage(int clientId, const Message& message);

    /**
     * @brief Отправка сообщения всем подключенным клиентам
     * @param message Сообщение для отправки
     */
    void broadcastMessage(const Message& message);

    /**
     * @brief Регистрация нового пользователя
     * @param user Данные пользователя
     * @return ID зарегистрированного пользователя
     */
    int registerUser(const User& user);

    /**
     * @brief Аутентификация пользователя
     * @param username Имя пользователя
     * @param password Пароль
     * @return ID пользователя или -1 при ошибке
     */
    int authenticateUser(const std::string& username, const std::string& password);

    /**
     * @brief Получение пользователя по ID
     * @param userId ID пользователя
     * @return Указатель на пользователя или nullptr
     */
    std::shared_ptr<User> getUser(int userId);

    /**
     * @brief Установка обработчика сообщений
     * @param handler Функция-обработчик
     */
    void setMessageHandler(std::function<void(int, const Message&)> handler);

private:
    /**
     * @brief Основной цикл сервера
     */
    void serverLoop();

    /**
     * @brief Обработка нового клиентского подключения
     * @param clientSocket Сокет клиента
     */
    void handleClient(socket_t clientSocket);

    /**
     * @brief Обработка входящего сообщения
     * @param clientId ID клиента
     * @param message Сообщение
     */
    void processMessage(int clientId, const Message& message);

    /**
     * @brief Инициализация сетевой библиотеки
     * @return true если инициализация успешна
     */
    bool initializeNetwork();

    /**
     * @brief Очистка сетевых ресурсов
     */
    void cleanupNetwork();

    int m_port;                                     ///< Порт сервера
    socket_t m_serverSocket;                        ///< Сокет сервера
    bool m_running;                                 ///< Флаг работы сервера
    std::thread m_serverThread;                     ///< Поток сервера
    std::vector<std::thread> m_clientThreads;       ///< Потоки клиентов
    mutable std::mutex m_clientsMutex;              ///< Мьютекс для защиты клиентов
    std::map<int, socket_t> m_clients;              ///< Карта клиентских сокетов
    std::map<int, std::shared_ptr<User>> m_users;   ///< Карта пользователей
    std::function<void(int, const Message&)> m_messageHandler; ///< Обработчик сообщений
    int m_nextClientId;                             ///< Счетчик ID клиентов
    int m_nextUserId;                               ///< Счетчик ID пользователей
};

#endif // SERVER_H
