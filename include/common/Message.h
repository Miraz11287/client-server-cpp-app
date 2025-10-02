#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>
#include <chrono>

/**
 * @brief Класс для представления сообщения в системе
 * 
 * Этот класс инкапсулирует сообщение между клиентом и сервером,
 * включая тип сообщения, содержимое и метаданные.
 */
class Message {
public:
    /**
     * @brief Типы сообщений в системе
     */
    enum class Type {
        LOGIN,          ///< Сообщение для входа в систему
        LOGOUT,         ///< Сообщение для выхода из системы
        TEXT,           ///< Текстовое сообщение
        FILE,           ///< Файловое сообщение
        STATUS,         ///< Статусное сообщение
        ERROR           ///< Сообщение об ошибке
    };

    /**
     * @brief Конструктор по умолчанию
     */
    Message();

    /**
     * @brief Конструктор с параметрами
     * @param type Тип сообщения
     * @param content Содержимое сообщения
     * @param senderId ID отправителя
     * @param receiverId ID получателя
     */
    Message(Type type, const std::string& content, int senderId, int receiverId = -1);

    /**
     * @brief Деструктор
     */
    ~Message() = default;

    // Геттеры
    Type getType() const { return m_type; }
    const std::string& getContent() const { return m_content; }
    int getSenderId() const { return m_senderId; }
    int getReceiverId() const { return m_receiverId; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return m_timestamp; }

    // Сеттеры
    void setType(Type type) { m_type = type; }
    void setContent(const std::string& content) { m_content = content; }
    void setSenderId(int senderId) { m_senderId = senderId; }
    void setReceiverId(int receiverId) { m_receiverId = receiverId; }

    /**
     * @brief Сериализация сообщения в строку
     * @return Строковое представление сообщения
     */
    std::string serialize() const;

    /**
     * @brief Десериализация сообщения из строки
     * @param data Строковое представление сообщения
     * @return true если десериализация успешна
     */
    bool deserialize(const std::string& data);

    /**
     * @brief Получение строкового представления типа сообщения
     * @param type Тип сообщения
     * @return Строковое представление
     */
    static std::string typeToString(Type type);

    /**
     * @brief Получение типа сообщения из строки
     * @param typeStr Строковое представление типа
     * @return Тип сообщения
     */
    static Type stringToType(const std::string& typeStr);

private:
    Type m_type;                                    ///< Тип сообщения
    std::string m_content;                          ///< Содержимое сообщения
    int m_senderId;                                 ///< ID отправителя
    int m_receiverId;                               ///< ID получателя
    std::chrono::system_clock::time_point m_timestamp; ///< Временная метка
};

#endif // MESSAGE_H
