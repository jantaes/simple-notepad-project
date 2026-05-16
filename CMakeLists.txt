cmake_minimum_required(VERSION 4.0)
project(simple_notepad_project)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(
        notepad
        main.cpp
        main_window.cpp
        main_window.h
        text_transform.h
        notepad_exception.h
        find_replace_dialog.ui
        word_frequency_dialog.ui
)
target_link_libraries(notepad PRIVATE Qt6::Widgets)