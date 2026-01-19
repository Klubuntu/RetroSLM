cmake_minimum_required(VERSION 3.10)
project(RetroSLM)

set(CMAKE_CXX_FLAGS "")
set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS_INIT "")
set(CMAKE_C_FLAGS_INIT "")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(ftxui_DIR "${CMAKE_SOURCE_DIR}/ftxui/install/lib/cmake/ftxui")


add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8 -fwide-exec-charset=UTF-16LE)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()



# ============================
#   Pliki źródłowe
# ============================
set(SOURCES
    main.cpp
    src/Magazyn.cpp
    src/Klienci.cpp
    src/Faktury.cpp
    src/Historia.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# ============================
#   FTXUI (zainstalowane przez make install)
# ============================
#set(ftxui_DIR "${CMAKE_SOURCE_DIR}/lib/cmake/ftxui")
find_package(ftxui REQUIRED)

foreach(target ftxui::screen ftxui::dom ftxui::component)
    get_target_property(opts ${target} INTERFACE_COMPILE_OPTIONS)
    if(opts)
        list(REMOVE_ITEM opts "/utf-8")
        set_target_properties(${target} PROPERTIES INTERFACE_COMPILE_OPTIONS "${opts}")
    endif()
endforeach()




target_link_libraries(${PROJECT_NAME}
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)

# ============================
#   Nagłówki JSON + FTXUI
# ============================
target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)
