# Force CMAKE to use at least C++11

include(CheckCXXCompilerFlag)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message("Using Clang")
  CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
  CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX1Y)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

  # Linux (gcc stdc++ lib) dont seems to mesh well...
  CHECK_CXX_COMPILER_FLAG("-std=gnu++0x" COMPILER_SUPPORTS_GNU0X)

  if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    set(CXX_HAS_CPP14 true)
  elseif(COMPILER_SUPPORTS_CXX1Y)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
    set(CXX_HAS_CPP14 true)
  elseif(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CXX_HAS_CPP14 false)
  elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    set(CXX_HAS_CPP14 false)
  else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
  endif()


elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    message("Using GCC")
    # Linux (gcc stdc++ lib) dont seems to mesh well...
    CHECK_CXX_COMPILER_FLAG("-std=gnu++14" COMPILER_SUPPORTS_GNU14)
    CHECK_CXX_COMPILER_FLAG("-std=gnu++11" COMPILER_SUPPORTS_GNU11)

    if(COMPILER_SUPPORTS_GNU11)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
      set(CXX_HAS_CPP14 false)
    else()
      message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()


endif()
