#!/bin/bash

# Скрипт генерации документации

set -e

echo "=== Генерация документации ==="

# Проверка наличия Doxygen
if ! command -v doxygen &> /dev/null; then
    echo "Ошибка: Doxygen не найден. Установите Doxygen для генерации документации."
    exit 1
fi

# Создание директории для документации
mkdir -p docs/doxygen

# Генерация документации
echo "Генерация документации с помощью Doxygen..."
doxygen Doxyfile

# Проверка результата
if [ -d "docs/doxygen/html" ]; then
    echo "✓ Документация успешно сгенерирована"
    echo "Откройте docs/doxygen/html/index.html для просмотра"
else
    echo "✗ Ошибка генерации документации"
    exit 1
fi

echo "=== Генерация документации завершена ==="
