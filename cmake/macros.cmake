# Create a CMake option
# Usage: chat_option(<variable> <description> <default>)
macro(chat_option variable description default)
    option(${variable} ${description} ${default})
    message(STATUS "${variable}: '${${variable}}'")
endmacro()

#Add compiler warning flags to target
#Usage: chat_add_compiler_warnings(<target>)
macro(chat_add_compiler_warnings target)
    target_compile_options(${target} PUBLIC
        -Wall
        -Wextra
        -Wpedantic
    )

    if(${CHAT_ENABLE_WARNING_AS_ERROR})
        target_compile_options(${target} PUBLIC -Werror)
    endif()
endmacro()

#Instrument target with sanitizers
#Usage: chat_add_sanitizers(<target>)
macro(chat_add_sanitizers target)
    if(${CHAT_ENABLE_ASAN})
        target_compile_options(${target} PUBLIC -fsanitize=address)
        target_link_options(${target} PUBLIC -fsanitize=address)
    endif()

    if(${CHAT_ENABLE_TSAN})
        target_compile_options(${target} PUBLIC -fsanitize=thread)
        target_link_options(${target} PUBLIC -fsanitize=thread)
    endif()

    if(${CHAT_ENABLE_UBSAN})
        target_compile_options(${target} PUBLIC -fsanitize=undefined)
        target_link_options(${target} PUBLIC -fsanitize=undefined)
    endif()

    if(${CHAT_ENABLE_LSAN})
        target_compile_options(${target} PUBLIC -fsanitize=leak)
        target_link_options(${target} PUBLIC -fsanitize=leak)
    endif()
endmacro()

#Add a target for a library
#Usage: chat_add_library(<name>)
macro(chat_add_library library_name)
    add_library(${library_name} STATIC)
    add_library(chat::${library_name} ALIAS ${library_name})

    target_compile_features(${library_name} PUBLIC cxx_std_17)
    chat_add_compiler_warnings(${library_name})
    chat_add_sanitizers(${library_name})
endmacro()

#Add a target for an application
#Usage: chat_add_app(<name>)
macro(chat_add_app app_name)
    add_executable(${app_name})

    target_compile_features(${app_name} PUBLIC cxx_std_17)
    chat_add_compiler_warnings(${app_name})
    chat_add_sanitizers(${app_name})
endmacro()

#Add a target for a test
#Usage: chat_add_test(<name>)
macro(chat_add_test test_name)
    add_executable(${test_name})

    target_compile_features(${test_name} PUBLIC cxx_std_17)
    chat_add_compiler_warnings(${test_name})
    chat_add_sanitizers(${test_name})
    target_compile_options(${test_name} PUBLIC -g)

    #Use Catch2 library
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(${test_name} PRIVATE Catch2::Catch2WithMain)
    include(Catch) # For `catch_discover_tests()`
    catch_discover_tests(${test_name})
endmacro()
