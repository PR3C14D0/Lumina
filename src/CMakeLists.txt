add_executable (Lumina "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Lumina PROPERTY CXX_STANDARD 20)
  set_property(TARGET Lumina PROPERTY WIN32_EXECUTABLE TRUE)
endif()


