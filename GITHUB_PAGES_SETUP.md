# Настройка GitHub Pages для проекта

## 📋 Пошаговая инструкция

### 1. Включение GitHub Pages

1. Перейдите в **Settings** вашего репозитория
2. Найдите раздел **Pages** в левом меню
3. В разделе **Source** выберите **"GitHub Actions"**
4. Нажмите **Save**

### 2. Проверка разрешений

Убедитесь, что в настройках репозитория включены:
- **Actions**: General → Actions permissions → "Allow all actions and reusable workflows"
- **Pages**: Pages → Source → "GitHub Actions"

### 3. Запуск workflow

После настройки GitHub Pages:

1. Перейдите в раздел **Actions**
2. Найдите workflow **"Deploy Documentation"**
3. Нажмите **"Run workflow"** (если есть кнопка)
4. Или сделайте новый commit для автоматического запуска

### 4. Проверка результата

После успешного выполнения workflow:
- Документация будет доступна по адресу: `https://miraz11287.github.io/client-server-cpp-app/`
- В разделе **Pages** в настройках репозитория появится ссылка на сайт

## 🔧 Устранение проблем

### Проблема: "Process /usr/bin/git exited with code 128"

**Причина**: GitHub Pages не настроен или нет необходимых разрешений.

**Решение**:
1. Убедитесь, что GitHub Pages включен в настройках
2. Проверьте, что выбран источник "GitHub Actions"
3. Убедитесь, что у workflow есть необходимые разрешения

### Проблема: Workflow не запускается

**Решение**:
1. Проверьте, что Actions включены в настройках репозитория
2. Убедитесь, что файлы workflow находятся в `.github/workflows/`
3. Проверьте синтаксис YAML файлов

### Проблема: Документация не генерируется

**Решение**:
1. Проверьте, что Doxygen установлен в workflow
2. Убедитесь, что скрипт `scripts/generate_docs.sh` исполняемый
3. Проверьте логи в разделе Actions

## 📚 Альтернативные варианты

### Вариант 1: Ручная генерация документации

```bash
# Локально
make docs

# Затем загрузите docs/doxygen/html/ в ветку gh-pages
```

### Вариант 2: Использование GitHub Pages с веткой

1. Создайте ветку `gh-pages`
2. Сгенерируйте документацию локально
3. Загрузите в ветку `gh-pages`
4. В настройках Pages выберите источник "Deploy from a branch"

## 🎯 Ожидаемый результат

После успешной настройки:
- ✅ Документация автоматически генерируется при каждом push
- ✅ Документация доступна по публичному URL
- ✅ Обновления документации происходят автоматически
- ✅ Документация доступна для всех пользователей

## 🔗 Полезные ссылки

- [GitHub Pages Documentation](https://docs.github.com/en/pages)
- [GitHub Actions for Pages](https://github.com/actions/deploy-pages)
- [Doxygen Documentation](https://www.doxygen.nl/manual/index.html)
