# Диаграмма классов клиент-серверного приложения

## Общая архитектура

```mermaid
classDiagram
    class Message {
        -Type m_type
        -string m_content
        -int m_senderId
        -int m_receiverId
        -time_point m_timestamp
        +getType() Type
        +getContent() string
        +getSenderId() int
        +getReceiverId() int
        +getTimestamp() time_point
        +setType(Type) void
        +setContent(string) void
        +setSenderId(int) void
        +setReceiverId(int) void
        +serialize() string
        +deserialize(string) bool
        +typeToString(Type) string
        +stringToType(string) Type
    }

    class User {
        -int m_id
        -string m_username
        -string m_email
        -Status m_status
        -vector~int~ m_contacts
        +getId() int
        +getUsername() string
        +getEmail() string
        +getStatus() Status
        +getContacts() vector~int~
        +setId(int) void
        +setUsername(string) void
        +setEmail(string) void
        +setStatus(Status) void
        +addContact(int) void
        +removeContact(int) bool
        +hasContact(int) bool
        +statusToString(Status) string
        +stringToStatus(string) Status
        +isValidEmail(string) bool
        +isValidUsername(string) bool
    }

    class Server {
        -int m_port
        -socket_t m_serverSocket
        -bool m_running
        -thread m_serverThread
        -vector~thread~ m_clientThreads
        -mutex m_clientsMutex
        -map~int,socket_t~ m_clients
        -map~int,shared_ptr~User~~ m_users
        -function~void(int,Message)~ m_messageHandler
        -int m_nextClientId
        -int m_nextUserId
        +start() bool
        +stop() void
        +isRunning() bool
        +getClientCount() size_t
        +sendMessage(int, Message) bool
        +broadcastMessage(Message) void
        +registerUser(User) int
        +authenticateUser(string, string) int
        +getUser(int) shared_ptr~User~
        +setMessageHandler(function) void
        -serverLoop() void
        -handleClient(socket_t) void
        -processMessage(int, Message) void
        -initializeNetwork() bool
        -cleanupNetwork() void
    }

    class ClientHandler {
        -socket_t m_clientSocket
        -int m_clientId
        -atomic~bool~ m_active
        -thread m_clientThread
        -shared_ptr~User~ m_user
        -function~void(int,Message)~ m_messageHandler
        +start() void
        +stop() void
        +isActive() bool
        +getClientId() int
        +getUser() shared_ptr~User~
        +setUser(shared_ptr~User~) void
        +sendMessage(Message) bool
        +setMessageHandler(function) void
        -clientLoop() void
        -processIncomingMessage(Message) void
        -sendResponse(Message) void
        -handleNetworkError(int) void
    }

    class Client {
        -socket_t m_socket
        -atomic~bool~ m_connected
        -thread m_receiveThread
        -shared_ptr~User~ m_currentUser
        -function~void(Message)~ m_messageHandler
        -function~void(string)~ m_errorHandler
        -int m_clientId
        -string m_serverAddress
        -int m_serverPort
        +connect(string, int) bool
        +disconnect() void
        +isConnected() bool
        +sendMessage(Message) bool
        +sendTextMessage(string, int) bool
        +login(string, string) bool
        +logout() bool
        +registerUser(string, string, string) bool
        +getCurrentUser() shared_ptr~User~
        +setMessageHandler(function) void
        +setErrorHandler(function) void
        +getClientId() int
        -receiveLoop() void
        -processIncomingMessage(Message) void
        -initializeNetwork() bool
        -cleanupNetwork() void
    }

    %% Связи между классами
    Server --> ClientHandler : "создает и управляет"
    Server --> User : "хранит пользователей"
    ClientHandler --> Message : "обрабатывает"
    ClientHandler --> User : "связан с пользователем"
    Client --> Message : "отправляет/получает"
    Client --> User : "имеет текущего пользователя"
    Message --> User : "ссылается на отправителя/получателя"
```

## Диаграмма взаимодействия (Sequence Diagram)

```mermaid
sequenceDiagram
    participant C as Client
    participant S as Server
    participant CH as ClientHandler
    participant U as User

    C->>S: connect(serverAddress, port)
    S->>CH: create ClientHandler
    S->>C: connection established

    C->>S: login(username, password)
    S->>U: authenticateUser(username, password)
    U-->>S: userId
    S->>CH: setUser(user)
    S-->>C: login success

    C->>S: sendTextMessage(content)
    S->>CH: processMessage(message)
    CH->>S: broadcastMessage(message)
    S->>CH: sendMessage(message)
    CH-->>C: message delivered

    C->>S: logout()
    S->>U: setStatus(OFFLINE)
    S-->>C: logout success

    C->>S: disconnect()
    S->>CH: stop()
    CH-->>S: handler stopped
    S-->>C: disconnected
```

## Диаграмма компонентов

```mermaid
graph TB
    subgraph "Client Application"
        CM[Client Main]
        CC[Client Class]
        CU[User Class]
        CMsg[Message Class]
    end

    subgraph "Server Application"
        SM[Server Main]
        SC[Server Class]
        SCH[ClientHandler Class]
        SU[User Class]
        SMsg[Message Class]
    end

    subgraph "Network Layer"
        TCP[TCP Socket]
        IP[IP Protocol]
    end

    subgraph "Docker Container"
        DC[Docker Container]
        DCS[Docker Compose]
    end

    CM --> CC
    CC --> CU
    CC --> CMsg
    CC --> TCP

    SM --> SC
    SC --> SCH
    SC --> SU
    SCH --> SMsg
    SC --> TCP

    TCP --> IP
    DC --> DCS
```

## Диаграмма развертывания

```mermaid
graph TB
    subgraph "Development Environment"
        DEV[Developer Machine]
        GIT[Git Repository]
    end

    subgraph "Docker Environment"
        DOCKER[Docker Engine]
        COMPOSE[Docker Compose]
        SERVER_CONTAINER[Server Container]
        CLIENT_CONTAINER[Client Container]
    end

    subgraph "Network"
        NET[Network Interface]
        PORT[Port 8080]
    end

    DEV --> GIT
    GIT --> DOCKER
    DOCKER --> COMPOSE
    COMPOSE --> SERVER_CONTAINER
    COMPOSE --> CLIENT_CONTAINER
    SERVER_CONTAINER --> NET
    NET --> PORT
    CLIENT_CONTAINER --> NET
```

## Описание классов

### Message
Класс для представления сообщений в системе. Поддерживает различные типы сообщений (LOGIN, LOGOUT, TEXT, FILE, STATUS, ERROR) и обеспечивает сериализацию/десериализацию для передачи по сети.

### User
Класс для представления пользователей системы. Содержит информацию о пользователе, его статусе и списке контактов. Включает методы валидации данных.

### Server
Основной класс сервера, реализующий многопоточную обработку клиентских подключений. Управляет пользователями, обрабатывает сообщения и обеспечивает связь между клиентами.

### ClientHandler
Класс для обработки отдельного клиентского подключения. Работает в отдельном потоке и обрабатывает входящие сообщения от конкретного клиента.

### Client
Класс клиентского приложения, обеспечивающий подключение к серверу, аутентификацию и обмен сообщениями. Работает в многопоточном режиме для приема сообщений.
