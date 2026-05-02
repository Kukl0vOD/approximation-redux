cmake_minimum_required(VERSION 3.20)

project(redux LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(REDUX_BUILD_APP "Build the redux executable with controller and plotter layers" ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>)
endif()

add_library(matplot STATIC IMPORTED GLOBAL)
set_target_properties(matplot PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/matplot.lib
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(redux_model STATIC
    src/model/approximation.cpp
    src/model/eos.cpp
    src/model/k_value_calculator.cpp
    src/model/mixture_properties.cpp
    src/model/solution.cpp
    src/model/thermo_calculator.cpp
    src/model/utilities.cpp
    src/model/z_factor_selector.cpp
)

target_include_directories(redux_model
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(redux_model PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(redux_model PRIVATE /W3 /permissive-)
else()
    target_compile_options(redux_model PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(redux_application STATIC
    src/application/approximation_range_service.cpp
    src/application/error_analysis_service.cpp
    src/application/json_result_writer.cpp
    src/application/k_value_range_service.cpp
    src/application/phase_pair_factory.cpp
    src/application/thermo_range_service.cpp
    src/controller.cpp
    src/json.cpp
    src/json_builder.cpp
    src/json_parser.cpp
)

target_link_libraries(redux_application
    PUBLIC
        redux_model
)

target_compile_features(redux_application PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(redux_application PRIVATE /W3 /permissive-)
else()
    target_compile_options(redux_application PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(redux_view STATIC
    src/plotter.cpp
    src/view/matplot_renderer.cpp
    src/view/plot_file_name_formatter.cpp
)

target_link_libraries(redux_view
    PUBLIC
        redux_application
        matplot
)

target_compile_features(redux_view PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(redux_view PRIVATE /W3 /permissive-)
else()
    target_compile_options(redux_view PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(REDUX_BUILD_APP)
    add_executable(redux
        src/main.cpp
    )

    target_link_libraries(redux
        PRIVATE
            redux_view
    )

    if(MSVC)
        target_compile_options(redux PRIVATE /W3 /permissive-)
    else()
        target_compile_options(redux PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_custom_command(TARGET redux POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/resources/data
                $<TARGET_FILE_DIR:redux>
    )

    set_property(TARGET redux PROPERTY
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:redux>"
    )
endif()
