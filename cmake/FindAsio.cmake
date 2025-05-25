# This module finds the Asio library and creates a target for it.
#
# This module defines the following variables:
# - Asio_INCLUDE_DIR
# - Asio_VERSION
#
# This module defines the following targets:
# - Asio
#
# Asio is a header-only library, so there are no library files to look for. The
# typical file structure of Asio's installed files is as follows:
# include/
# ├── asio/
# │   ├── any_completion_executor.hpp
# │   ├── any_completion_handler.hpp
# │   ├── any_io_executor.hpp
# │   └── ...
# └── asio.hpp
#
# The include directory can be found by looking for the directory that contains
# the `asio.hpp` header file.

find_path(Asio_INCLUDE_DIR NAMES asio.hpp REQUIRED)
add_library(Asio INTERFACE)
add_library(Asio::Asio ALIAS Asio)
target_include_directories(Asio INTERFACE ${Asio_INCLUDE_DIR})

file(READ "${Asio_INCLUDE_DIR}/asio/version.hpp" _Asio_ver_file)
string(REGEX MATCH "#define ASIO_VERSION ([0-9]+)" _ "${_Asio_ver_file}")
math(EXPR _Asio_major_ver "${CMAKE_MATCH_1} / 100000")
math(EXPR _Asio_minor_ver "${CMAKE_MATCH_1} / 100 % 1000")
math(EXPR _Asio_subminor_ver "${CMAKE_MATCH_1} % 100")
set(Asio_VERSION "${_Asio_major_ver}.${_Asio_minor_ver}.${_Asio_subminor_ver}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Asio
  REQUIRED_VARS
    Asio_INCLUDE_DIR
  VERSION_VAR
    Asio_VERSION
)
