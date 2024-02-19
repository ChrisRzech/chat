#Add compiler warning flags to target
#Usage: chat_add_compiler_warnings(<target>)
macro(chat_add_compiler_warnings target)
    target_compile_options(${target} PUBLIC
        -Werror
        -Wall
        -Wextra
        -Wpedantic
    )
endmacro()

#Add compiler options for the build type to target
#Usage: chat_add_build_type_compiler_options(<target>)
macro(chat_add_build_type_compiler_options target)
    if(${CMAKE_BUILD_TYPE} EQUAL "Debug")
        target_compile_options(${target} PUBLIC -O0 -g)
    elseif(${CMAKE_BUILD_TYPE} EQUAL "RelWithDebInfo")
        target_compile_options(${target} PUBLIC -O2 -g -DNDEBUG)
    elseif(${CMAKE_BUILD_TYPE} EQUAL "Release")
        target_compile_options(${target} PUBLIC -O3 -DNDEBUG)
    endif()
endmacro()

#Add a target for a library
#Usage: chat_add_library(<name>)
macro(chat_add_library library_name)
    add_library(${library_name} STATIC)
    add_library(chat::${library_name} ALIAS ${library_name})

    target_compile_features(${library_name} PUBLIC cxx_std_17)
    chat_add_compiler_warnings(${library_name})
    chat_add_build_type_compiler_options(${library_name})
endmacro()

#Add a target for an application
#Usage: chat_add_app(<name>)
macro(chat_add_app app_name)
    add_executable(${app_name})

    target_compile_features(${app_name} PUBLIC cxx_std_17)
    chat_add_compiler_warnings(${app_name})
    chat_add_build_type_compiler_options(${app_name})

    #TODO For some reason, running the executable in Git Bash doesn't seem to work correctly. The supposedly "static" server library doesn't
    #seem to be statically linked into the executable. This might be a Windows thing (and it probably is), so the "-static" linking flag
    #"forces" the chat::server library to be linked. For now, this flag will be used just for making development until there is a proper
    #solution to handle this.
    target_link_options(${app_name} PUBLIC -static)
endmacro()

#Add a target for a test
#Usage: chat_add_test(<name>)
macro(chat_add_test test_name)
    add_executable(${test_name})

    target_compile_features(${test_name} PUBLIC cxx_std_17)
    chat_add_compiler_warnings(${test_name})
    target_compile_options(${test_name} PUBLIC -g)

    #Use Catch2 library
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(${test_name} PRIVATE Catch2::Catch2WithMain)

    #See similar comment in `chat_add_app()` macro
    target_link_options(${test_name} PUBLIC -static)
endmacro()
