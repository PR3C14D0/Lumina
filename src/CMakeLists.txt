add_executable (Lumina "main.cpp"   "public/Engine/Core.h" "private/Engine/Core.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Lumina PROPERTY CXX_STANDARD 20)
  set_property(TARGET Lumina PROPERTY WIN32_EXECUTABLE TRUE)
endif()

set(SOURCE_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/public")
set(GLOBAL_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_include_directories(Lumina PRIVATE "${SOURCE_INCLUDES}")
target_include_directories(Lumina PUBLIC "${GLOBAL_INCLUDES}")
