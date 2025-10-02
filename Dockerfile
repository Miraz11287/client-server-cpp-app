# Dockerfile для клиент-серверного приложения на C++
FROM ubuntu:20.04

# Установка необходимых пакетов
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    doxygen \
    graphviz \
    && rm -rf /var/lib/apt/lists/*

# Создание рабочей директории
WORKDIR /app

# Копирование исходного кода
COPY . .

# Создание директории для сборки
RUN mkdir -p build

# Сборка проекта
WORKDIR /app/build
RUN cmake .. && make

# Создание скрипта запуска
RUN echo '#!/bin/bash\n\
if [ "$1" = "server" ]; then\n\
    echo "Запуск сервера..."\n\
    ./server\n\
elif [ "$1" = "client" ]; then\n\
    echo "Запуск клиента..."\n\
    ./client\n\
else\n\
    echo "Использование: docker run <image> [server|client]"\n\
fi' > /app/run.sh && chmod +x /app/run.sh

# Установка рабочей директории
WORKDIR /app

# Открытие порта для сервера
EXPOSE 8080

# Команда по умолчанию
CMD ["./run.sh", "server"]
