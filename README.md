Постановка задачи
=================

    Расписатор подразумевает гибкую, модульную и универсальную систему для
развертывания системы коллективного тайм-менеджмента.

    В качестве оригинальных иллюстраций:
     1. традиционная задача Летней Школы, интеллектуальное генерирование
         расписания дежурств по-хозяйству;  
     2. конкуренция с Microsoft Exchange + Outlook везде, где это возможно;
     3. контроль человеческих русурсов по аналогии с пакетами бухгалтерского
         учета.

Программные требования
======================

    1. В данный момент для сборки требуется компилятор, поддерживающий C++11,
актуальный список можно найти тут:
http://wiki.apache.org/stdcxx/C++0xCompilierSupport
    2. Также для сборки требуется CMake версии не ниже 2.8.

Как собрать
===========

    Для сборки на UNIX & UNIX like системах исполните в директории сборки:

    cmake [OPTIONS] path/to/source/directory && make
    
    Где path/to/source/directory - путь до директории с исходным кодом.
Настоятельно не рекомендуется сборка в директории репозитория (коммиты с файлами
сборки будут отбрасываться). OPTIONS - параметры сборки.

Опции сборки
============
    
    Ниже перечисленны переменные cmake, влияющие на конфигурацию сборки
программы. Перед запуском с новыми аргументами необходимо удалить CMakeCache.txt
в директории сборки. Пример использования:

    cmake -D WITH_SQLITE=Y ../UniSched

    Подготовить проект к сборке без поддержки sqlite

    +--------------+------------+------------------------------------------------------+
    | Опция        |Значение по | Описание                                             |
    |              |умолчанию   |                                                      |
    +==============+============+======================================================+
    | WITH_SQLITE  |    N       | Собирать бэкэнд к базе данных sqlite3 (необходимо    |
    |              |            | наличие в системе библиотеки и заголовков).          |
    +--------------+------------+------------------------------------------------------+
    | WITH_DUMMYUI |    Y       | Собирать пользовательский интерфейс-заглушку (ничего |
    |              |            | полезного не делает, просто сообщает о себе).        |
    +--------------+------------+------------------------------------------------------+
    | WITH_CLI     |    Y       | Собирать интерфейс коммандной строки (в разработке)  |
    +--------------+------------+------------------------------------------------------+


Переменные среды
================

    На работу программы влияют следующие переменные среды:

    +-----------------------+----------------------------------------------------------+
    | Переменная            | Описание                                                 |
    +=======================+==========================================================+
    | UNISCHED_MODULES_PATH | Путь поиска модулей. Перечисление директорий через ':'   |
    |                       | Например:                                                |
    |                       | UNISCHED_MODULES_PATH=src/modules/ui:src/modules/storage |
    +-----------------------+----------------------------------------------------------+

Как запустить
=============

    На текущий момент, работает только интерфейс-заглушка пользователя.

    ./src/raspisator --iface DummyUI


Code Style
==========

    Предлагается следование следующим правилам:

    F001 Source files should not use the '\r' (CR) character
    F002 File names should be well-formed
    L001 No trailing whitespace
    L002 Don't use tab characters
    L003 No leading and no trailing empty lines
    L004 Line cannot be too long
    L005 There should not be too many consecutive empty lines
    T001 One-line comments should not have forced continuation
    T002 Reserved names should not be used for preprocessor macros
    T003 Some keywords should be followed by a single space
    T004 Some keywords should be immediately followed by a colon
    T005 Keywords break and continue should be immediately followed by a
            semicolon
    T006 Keywords return and throw should be immediately followed by a semicolon
            or a single space
    T007 Semicolons should not be isolated by spaces or comments from the rest
            of the code
    T008 Keywords catch, for, if, switch and while should be followed by a
            single space
    T009 Comma should not be preceded by whitespace, but should be followed by
            one
    T010 Identifiers should not be composed of 'l' and 'O' characters only
    T011 Curly brackets from the same pair should be either in the same line or
            in the same column
    T017 Unnamed namespaces are not allowed in header files
    T018 Using namespace is not allowed in header files
    T019 Control structures should have complete curly-braced block of code 

    Подробнее смотри http://www.inspirel.com/vera/ce/doc/rules/index.html

    Тулза для проверки называется vera++, профиль настроек лежит в файле style_rules
