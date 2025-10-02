#!/bin/bash

# Скрипт запуска тестов

set -e

echo "=== Запуск тестов ==="

# Проверка наличия директории сборки
if [ ! -d "build" ]; then
    echo "Ошибка: Директория сборки не найдена. Запустите build.sh сначала."
    exit 1
fi

cd build

# Запуск unit тестов (если есть)
if [ -f "test_client_server" ]; then
    echo "Запуск unit тестов..."
    ./test_client_server
else
    echo "Unit тесты не найдены. Пропускаем."
fi

# Запуск интеграционных тестов
echo "Запуск интеграционных тестов..."

# Тест 1: Запуск сервера
echo "Тест 1: Запуск сервера"
timeout 5s ./server &
SERVER_PID=$!
sleep 2

# Проверка, что сервер запущен
if ps -p $SERVER_PID > /dev/null; then
    echo "✓ Сервер запущен успешно"
else
    echo "✗ Ошибка запуска сервера"
    exit 1
fi

# Тест 2: Подключение клиента
echo "Тест 2: Подключение клиента"
echo "connect 127.0.0.1 8080" | timeout 5s ./client &
CLIENT_PID=$!
sleep 2

# Проверка, что клиент подключился
if ps -p $CLIENT_PID > /dev/null; then
    echo "✓ Клиент подключился успешно"
else
    echo "✗ Ошибка подключения клиента"
fi

# Остановка процессов
echo "Остановка тестовых процессов..."
kill $SERVER_PID 2>/dev/null || true
kill $CLIENT_PID 2>/dev/null || true

echo "=== Тесты завершены ==="
