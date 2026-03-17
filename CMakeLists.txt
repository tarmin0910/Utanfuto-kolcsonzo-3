cmake_minimum_required(VERSION 3.16)
project(trailo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Widgets Sql REQUIRED)

if (MSVC)
  add_compile_options(/utf-8)
endif()

qt_standard_project_setup()

qt_add_executable(trailo
    main.cpp
    LoginDialog.h LoginDialog.cpp
    RegisterDialog.h RegisterDialog.cpp
    ResetPasswordDialog.h ResetPasswordDialog.cpp
    MainWindow.h MainWindow.cpp
    TrailerCard.h TrailerCard.cpp
    DatabaseManager.h DatabaseManager.cpp
    RentDialog.h RentDialog.cpp
    ReservationDetailsDialog.h ReservationDetailsDialog.cpp
)

qt_add_resources(trailo "resources"
    PREFIX "/"
    FILES
        images/open.jpg
        images/ponyva.png
        images/motor.jpg
        images/auto.png
        images/billeno.png
)

target_link_libraries(trailo PRIVATE Qt6::Widgets Qt6::Sql)
