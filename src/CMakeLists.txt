add_executable (Lumina "main.cpp"   "public/Engine/Core.h" "private/Engine/Core.cpp" "public/Engine/Util.h" "public/Engine/Scene/SceneManager.h" "private/Engine/Scene/SceneManager.cpp" "public/Engine/Scene/Scene.h" "private/Engine/Scene/Scene.cpp" "public/Engine/ScreenQuad.h" "public/DirectXIncludes.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Lumina PROPERTY CXX_STANDARD 20)
  set_property(TARGET Lumina PROPERTY WIN32_EXECUTABLE TRUE)
endif()

set(SOURCE_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/public")
set(GLOBAL_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(DX_LIB "dxgi.lib;d3d12.lib")

target_link_libraries(Lumina "${DX_LIB}")

target_include_directories(Lumina PRIVATE "${SOURCE_INCLUDES}")
target_include_directories(Lumina PUBLIC "${GLOBAL_INCLUDES}")
