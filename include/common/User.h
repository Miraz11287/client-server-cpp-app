#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <memory>

/**
 * @brief Класс для представления пользователя в системе
 * 
 * Этот класс инкапсулирует информацию о пользователе,
 * включая его идентификатор, имя, статус и историю сообщений.
 */
class User {
public:
    /**
     * @brief Статусы пользователя
     */
    enum class Status {
        OFFLINE,    ///< Пользователь не в сети
        ONLINE,     ///< Пользователь в сети
        BUSY,       ///< Пользователь занят
        AWAY        ///< Пользователь отошел
    };

    /**
     * @brief Конструктор по умолчанию
     */
    User();

    /**
     * @brief Конструктор с параметрами
     * @param id Уникальный идентификатор пользователя
     * @param username Имя пользователя
     * @param email Email пользователя
     */
    User(int id, const std::string& username, const std::string& email);

    /**
     * @brief Деструктор
     */
    ~User() = default;

    // Геттеры
    int getId() const { return m_id; }
    const std::string& getUsername() const { return m_username; }
    const std::string& getEmail() const { return m_email; }
    Status getStatus() const { return m_status; }
    const std::vector<int>& getContacts() const { return m_contacts; }

    // Сеттеры
    void setId(int id) { m_id = id; }
    void setUsername(const std::string& username) { m_username = username; }
    void setEmail(const std::string& email) { m_email = email; }
    void setStatus(Status status) { m_status = status; }

    /**
     * @brief Добавление контакта
     * @param contactId ID контакта
     */
    void addContact(int contactId);

    /**
     * @brief Удаление контакта
     * @param contactId ID контакта
     * @return true если контакт был удален
     */
    bool removeContact(int contactId);

    /**
     * @brief Проверка наличия контакта
     * @param contactId ID контакта
     * @return true если контакт существует
     */
    bool hasContact(int contactId) const;

    /**
     * @brief Получение строкового представления статуса
     * @param status Статус пользователя
     * @return Строковое представление
     */
    static std::string statusToString(Status status);

    /**
     * @brief Получение статуса из строки
     * @param statusStr Строковое представление статуса
     * @return Статус пользователя
     */
    static Status stringToStatus(const std::string& statusStr);

    /**
     * @brief Валидация email адреса
     * @param email Email для проверки
     * @return true если email валиден
     */
    static bool isValidEmail(const std::string& email);

    /**
     * @brief Валидация имени пользователя
     * @param username Имя пользователя для проверки
     * @return true если имя валидно
     */
    static bool isValidUsername(const std::string& username);

private:
    int m_id;                           ///< Уникальный идентификатор
    std::string m_username;             ///< Имя пользователя
    std::string m_email;                ///< Email адрес
    Status m_status;                    ///< Текущий статус
    std::vector<int> m_contacts;        ///< Список контактов
};

#endif // USER_H
