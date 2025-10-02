# Инструкции по настройке GitHub репозитория

## Создание репозитория на GitHub

### Шаг 1: Создание нового репозитория

1. Войдите в GitHub с учетными данными:
   - Email: mabdelsater11@hotmail.com
   - Пароль: M.m.112345ru@@

2. Перейдите на https://github.com/new

3. Заполните форму:
   - **Repository name**: `client-server-cpp-app`
   - **Description**: `Многопоточное клиент-серверное приложение на C++ с Docker поддержкой`
   - **Visibility**: Public (или Private по желанию)
   - **Initialize repository**: НЕ отмечайте (у нас уже есть файлы)

4. Нажмите "Create repository"

### Шаг 2: Инициализация локального Git репозитория

```bash
# Инициализация Git репозитория
git init

# Добавление всех файлов
git add .

# Первый коммит
git commit -m "Initial commit: Client-server application with Docker support"

# Добавление удаленного репозитория
git remote add origin https://github.com/mabdelsater11/client-server-cpp-app.git

# Отправка кода на GitHub
git push -u origin main
```

### Шаг 3: Настройка веток

```bash
# Создание ветки для разработки
git checkout -b develop

# Отправка ветки на GitHub
git push -u origin develop
```

## Настройка GitHub Actions (CI/CD)

### Создание файла .github/workflows/ci.yml

```yaml
name: CI/CD Pipeline

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake doxygen graphviz
    
    - name: Build project
      run: |
        mkdir build
        cd build
        cmake ..
        make
    
    - name: Run tests
      run: |
        cd build
        chmod +x ../scripts/run_tests.sh
        ../scripts/run_tests.sh
    
    - name: Generate documentation
      run: |
        chmod +x scripts/generate_docs.sh
        ./scripts/generate_docs.sh
    
    - name: Upload documentation
      uses: actions/upload-artifact@v3
      with:
        name: documentation
        path: docs/doxygen/html/
```

## Настройка GitHub Pages

### Шаг 1: Включение GitHub Pages

1. Перейдите в Settings репозитория
2. Найдите раздел "Pages"
3. В "Source" выберите "GitHub Actions"

### Шаг 2: Создание workflow для GitHub Pages

Создайте файл `.github/workflows/pages.yml`:

```yaml
name: Deploy Documentation

on:
  push:
    branches: [ main ]

jobs:
  deploy:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake doxygen graphviz
    
    - name: Generate documentation
      run: |
        chmod +x scripts/generate_docs.sh
        ./scripts/generate_docs.sh
    
    - name: Deploy to GitHub Pages
      uses: peaceiris/actions-gh-pages@v3
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: ./docs/doxygen/html
```

## Настройка Issues и Projects

### Создание шаблонов Issues

Создайте файл `.github/ISSUE_TEMPLATE/bug_report.md`:

```markdown
---
name: Bug report
about: Create a report to help us improve
title: ''
labels: bug
assignees: ''
---

**Describe the bug**
A clear and concise description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Go to '...'
2. Click on '....'
3. Scroll down to '....'
4. See error

**Expected behavior**
A clear and concise description of what you expected to happen.

**Screenshots**
If applicable, add screenshots to help explain your problem.

**Environment:**
 - OS: [e.g. Ubuntu 20.04]
 - Compiler: [e.g. GCC 9.3.0]
 - Version: [e.g. 1.0.0]

**Additional context**
Add any other context about the problem here.
```

### Создание шаблона Pull Request

Создайте файл `.github/pull_request_template.md`:

```markdown
## Описание изменений

Краткое описание того, что было изменено.

## Тип изменений

- [ ] Исправление ошибки
- [ ] Новая функция
- [ ] Изменение документации
- [ ] Рефакторинг
- [ ] Тесты

## Чек-лист

- [ ] Код соответствует стандартам проекта
- [ ] Добавлены тесты для новой функциональности
- [ ] Все тесты проходят
- [ ] Документация обновлена
- [ ] Изменения не нарушают существующую функциональность

## Связанные Issues

Closes #(номер issue)
```

## Настройка защиты веток

### Защита main ветки

1. Перейдите в Settings → Branches
2. Нажмите "Add rule"
3. Настройте правила:
   - Branch name pattern: `main`
   - Require pull request reviews before merging
   - Require status checks to pass before merging
   - Require branches to be up to date before merging

## Настройка автоматических релизов

### Создание workflow для релизов

Создайте файл `.github/workflows/release.yml`:

```yaml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  release:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake
    
    - name: Build project
      run: |
        mkdir build
        cd build
        cmake ..
        make
    
    - name: Create release archive
      run: |
        tar -czf client-server-app-${{ github.ref_name }}.tar.gz build/
    
    - name: Create Release
      uses: actions/create-release@v1
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
      with:
        tag_name: ${{ github.ref }}
        release_name: Release ${{ github.ref }}
        body: |
          Автоматический релиз версии ${{ github.ref }}
          
          ## Изменения
          - См. CHANGELOG.md
          
          ## Установка
          1. Скачайте архив
          2. Распакуйте
          3. Запустите сервер: ./build/server
          4. Запустите клиент: ./build/client
        draft: false
        prerelease: false
```

## Создание CHANGELOG.md

```markdown
# Changelog

Все значимые изменения в проекте будут документированы в этом файле.

## [1.0.0] - 2024-01-01

### Добавлено
- Базовая функциональность клиент-серверного приложения
- Поддержка Docker и Docker Compose
- Документация с Doxygen
- Тест-кейсы и описание дефектов
- CI/CD pipeline с GitHub Actions
- Автоматическая генерация документации

### Изменено
- Начальная версия проекта

### Исправлено
- Нет исправлений в первой версии
```

## Финальные шаги

1. **Создайте README.md** с описанием проекта (уже создан)
2. **Добавьте лицензию** (уже создана)
3. **Настройте теги** для версионирования
4. **Создайте первый релиз** с тегом v1.0.0

### Команды для создания первого релиза:

```bash
# Создание тега
git tag -a v1.0.0 -m "Release version 1.0.0"

# Отправка тега на GitHub
git push origin v1.0.0
```

## Полезные ссылки

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [GitHub Pages Documentation](https://docs.github.com/en/pages)
- [Docker Hub](https://hub.docker.com/) (для публикации образов)
- [GitHub Packages](https://github.com/features/packages) (альтернатива Docker Hub)
