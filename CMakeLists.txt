cmake_minimum_required(VERSION 3.16)
project(qt_modbus_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets SerialBus)
find_package(Qt6 REQUIRED COMPONENTS Core)

set(CMAKE_AUTOUIC ON)  # <-- включаем автоматическую генерацию ui_*.h
set(CMAKE_AUTOMOC ON)  # <-- чтобы moc тоже генерировался
set(CMAKE_AUTORCC ON)  # <-- если есть .qrc файлы

add_executable(qt_modbus_example
    main.cpp
    mainwindow.h mainwindow.cpp
    modbusconnect.h modbusconnect.cpp
    config.h config.cpp



)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/nlohmann)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/image)
target_link_libraries(qt_modbus_example
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::SerialBus
)
target_link_libraries(qt_modbus_example PRIVATE Qt6::Core)
