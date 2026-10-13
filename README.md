# 3D Curves (DLL + exe, умные указатели)

Структура:
  Curves.sln          - решение Visual Studio 2022 (x64)
  Curves/
    Curves.vcxproj    - проект DLL (CURVES_EXPORTS, C++17)
    curves.h          - объявления классов + renderCurves (как ты прислал)
    curves.cpp        - реализации Point3D/Circle/Ellipse/Helix (восстановлены)
    render.cpp        - окно raylib, renderCurves (рендер-цикл из старого main, без изменений)
  Main/
    Main.vcxproj      - проект exe (линкуется с Curves.lib, копирует Curves.dll рядом с exe)
    main.cpp          - как ты прислал (shared_ptr, dynamic_pointer_cast, sort)
  include/raylib.h    - raylib v6.0 (как ты прислал)
  lib/                - сюда положи raylib.lib (x64, MSVC)!

Как собрать:
  1. Положи raylib.lib (версия для x64/MSVC) в папку lib/.
  2. Открой Curves.sln в Visual Studio 2022.
  3. Выбери конфигурацию x64 (Debug или Release) -> Build -> Build Solution.
  4. Запускай Main (F5). Curves.dll копируется рядом с exe автоматически.

Внимание: конфигурации только x64. Если твоя raylib.lib собрана под Win32 -
добавь конфигурацию x86 в обоих проектах или возьми x64-сборку raylib
(официальный релиз raylib для Windows - под MinGW; для MSVC x64 проще всего
взять raylib из vcpkg: vcpkg install raylib:x64-windows).
