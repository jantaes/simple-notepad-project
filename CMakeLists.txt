cmake_minimum_required(VERSION 4.0)
project(simple_notepad_project)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Gui)
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
        spell_checker.cpp
        spell_checker.h
        spell_checker_highlighter.cpp
        spell_checker_highlighter.h
        recent_file_manager.cpp
        recent_file_manager.h
        print_manager.cpp
        print_manager.h
        format_manager.cpp
        format_manager.h
        line_number.cpp
        line_number.h
)
target_link_libraries(notepad
        PRIVATE Qt6::Widgets
        Qt6::PrintSupport
        Qt6::Gui)