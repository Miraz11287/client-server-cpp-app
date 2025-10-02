#!/bin/bash

# Скрипт сборки клиент-серверного приложения

set -e  # Выход при ошибке

echo "=== Сборка клиент-серверного приложения ==="

# Проверка наличия CMake
if ! command -v cmake &> /dev/null; then
    echo "Ошибка: CMake не найден. Установите CMake для продолжения."
    exit 1
fi

# Создание директории сборки
echo "Создание директории сборки..."
mkdir -p build
cd build

# Конфигурация проекта
echo "Конфигурация проекта..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Сборка проекта
echo "Сборка проекта..."
make -j$(nproc)

echo "=== Сборка завершена успешно ==="
echo "Исполняемые файлы:"
echo "  - Сервер: build/server"
echo "  - Клиент: build/client"
