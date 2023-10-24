# FreqCounter

### Применение

Данный алгоритм считывает поток данных из файла(input.txt) и подсчитывает частоту слов, встречающихся в этом потоке.
Далее записывает в осортированном порядке по частоте в порядке убывания в другой файл(output.txt).
Файлы указываются в аргументах командной строки.

Пример запуска: 
./example input.txt output.txt

Пример входных данных: 
The time has come, the Walrus said,
to talk of many things...

Пример выходных данных:
  2 the
  1 come
  1 has
  1 many
  1 of
  1 said
  1 talk
  1 things
  1 time
  1 to
  1 walrus
  
### Build

Для сборки небходим cmake и компилятор поддерживающий c++17

По умолчанию стоит сборка с тестами, которая требует инициализации submodule и установки проекта.
Можно запустить просто скрипт build.sh, который выполнит процесс сборки со всей инициализацией.
Либо запустить конфигурацию cmake с флагом -DTESTING=OFF
