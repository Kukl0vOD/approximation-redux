# redux

Расчетный C++20-проект для работы с PVT-свойствами многокомпонентной углеводородной смеси.

Проект рассчитывает:

- молярный объем;
- удельный объем;
- K-values по определению;
- K-values по корреляции Wilson;
- аппроксимации объемов и K-values;
- относительные ошибки аппроксимаций.

Для расчетов используется кубическое уравнение состояния Peng-Robinson и корреляция Gao для бинарных коэффициентов взаимодействия.

## Структура

```text
include/
  model/        расчетная модель: EOS, Solution, K-values, аппроксимации
  application/  сервисы расчетов по диапазонам, запись JSON, анализ ошибок
  view/         описание и отрисовка графиков
src/
  model/        реализация расчетной модели
  application/  реализация прикладного слоя
  view/         реализация отрисовки
resources/
  data/         входные и сгенерированные JSON-данные
  plots/        PNG-графики
lib/
  matplot.lib   импортируемая статическая библиотека Matplot++
scripts/
  plot.gp       gnuplot-скрипт
```

## Сборка

Требования:

- CMake 3.20 или новее;
- компилятор C++20;
- MSVC/Windows для использования `lib/matplot.lib`;
- gnuplot, доступный из `PATH`, для сохранения графиков через Matplot++.

Сборка:

```bash
cmake -S . -B build
cmake --build build --config Release
```

Опция CMake:

```bash
cmake -S . -B build -DREDUX_BUILD_APP=OFF
```

`REDUX_BUILD_APP=OFF` собирает библиотеки без исполняемого файла `redux`.

## Запуск

После сборки CMake копирует файлы из `resources/data` в каталог с исполняемым файлом. Программа ожидает, что `extracted_solution_data.json` находится в рабочем каталоге.

Пример запуска для Visual Studio/MSVC:

```bash
build/Release/redux.exe
```

Текущий сценарий запуска задан в `src/main.cpp`. В нем создается `Controller`, выводится список состояний смеси и строится сравнительный график K-values для пары состояний `{0, 1}`.

## Входные данные

Основной входной файл:

```text
resources/data/extracted_solution_data.json
```

Файл содержит:

- `Pressure_dimension` - единицы давления;
- `Solution` - список компонентов смеси;
- `Concentrations` - набор концентрационных состояний;
- массив `P` для давления;
- концентрации компонентов для каждого состояния.

Компонент смеси содержит:

- `name`;
- `concentration`;
- `critical_temperature`;
- `critical_pressure`;
- `accentric_factor`;
- `molar_mass`.

## Выходные данные

Расчетные методы записывают JSON-файлы с результатами в текущий рабочий каталог. Имена формируются через `Controller::generateJsonName`.

Графики сохраняются как PNG-файлы. Имена формируются через `Plotter::generatePNGName`.

Основные типы выходных файлов:

- `exact_molar_*.json`;
- `exact_specific_*.json`;
- `exact_kvalue_*.json`;
- `wilson_kvalue_*.json`;
- `app_molar_*.json`;
- `app_specific_*.json`;
- `app_kvalue_*.json`;
- `rel_error_*.json`;
- `*.png`.

## Основные классы

- `sol::Solution` - состояние смеси и расчет объемов;
- `eos::PengRobinson` - реализация уравнения состояния Peng-Robinson;
- `approx::Approximator` - аппроксимация объемов и K-values;
- `Controller` - загрузка данных, настройка единиц, запуск расчетов;
- `Plotter` - подготовка расчетов для графиков;
- `MatplotRenderer` - сохранение графиков через Matplot++.

## Ограничения

- Исполняемый файл не принимает аргументы командной строки.
- Сценарий расчетов меняется в `src/main.cpp`.
- В CMake подключена готовая библиотека `lib/matplot.lib`; сборка ориентирована на окружение, совместимое с этой библиотекой.
- Автоматические тесты в проекте не настроены.
