﻿add_executable (Lumina "main.cpp"   "public/Engine/Core.h" "private/Engine/Core.cpp" "public/Engine/Util.h" "public/Engine/ScreenQuad.h" "public/DirectXIncludes.h" "private/Engine/ScreenQuad.cpp" "public/Engine/Vertex.h" "public/Engine/Shader/Shader.h" "private/Engine/Shader/Shader.cpp" "public/Engine/Scene/Scene.h" "private/Engine/Scene/Scene.cpp" "public/Engine/GameObject/GameObject.h" "private/Engine/GameObject/GameObject.cpp" "public/Math/Vector3.h" "private/Math/Vector3.cpp" "public/Math/Transform.h" "private/Math/Transform.cpp" "public/Engine/Camera/Camera.h" "private/Engine/Camera/Camera.cpp" "public/Engine/Scene/SceneManager.h" "private/Engine/Scene/SceneManager.cpp" "public/Engine/ConstantBuffers.h" "public/Engine/GameObject/Component/Component.h" "public/Engine/GameObject/Component/Mesh.h" "private/Engine/GameObject/Component/Mesh.cpp" "private/Engine/GameObject/Component/Component.cpp" "public/Engine/Input.h" "private/Engine/Input.cpp" "private/Engine/Camera/EditorCamera.cpp" "public/Engine/Camera/EditorCamera.h" "public/Engine/Time.h" "private/Engine/Time.cpp" "public/Engine/Editor/Editor.h" "private/Engine/Editor/Editor.cpp" "public/Engine/ResourceManager.h" "private/Engine/ResourceManager.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Lumina PROPERTY CXX_STANDARD 20)
  set_property(TARGET Lumina PROPERTY WIN32_EXECUTABLE TRUE)
endif()

# Required by assimp
add_compile_definitions(NOMINMAX)

set(SOURCE_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/public")
set(GLOBAL_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(LIB "${CMAKE_CURRENT_SOURCE_DIR}/../lib")
set(DX_LIB "dxgi.lib;d3d12.lib;d3dcompiler.lib")
set(ASSIMP_LIB "${LIB}/assimp/assimp-vc143-mt.lib")

set(DXTK_DEBUG "${LIB}/directxtk/DirectXTK12-Debug.lib")
set(DXTK "${LIB}/directxtk/DirectXTK12-Release.lib")

file(GLOB "IMGUI_SRC" 
	"${GLOBAL_INCLUDES}/imgui/imgui.cpp"
	"${GLOBAL_INCLUDES}/imgui/imgui_draw.cpp"
	"${GLOBAL_INCLUDES}/imgui/imgui_tables.cpp"
	"${GLOBAL_INCLUDES}/imgui/imgui_widgets.cpp"
	"${GLOBAL_INCLUDES}/imgui/backends/imgui_impl_dx12.cpp"
	"${GLOBAL_INCLUDES}/imgui/backends/imgui_impl_win32.cpp"
)

add_library("ImGui" STATIC "${IMGUI_SRC}")

# Link libraries
target_link_libraries(Lumina "${DX_LIB}")
target_link_libraries(Lumina "${ASSIMP_LIB}")
target_link_libraries(Lumina debug "${DXTK_DEBUG}")
target_link_libraries(Lumina optimized "${DXTK}")
target_link_libraries(Lumina ImGui)

target_include_directories(Lumina PRIVATE "${SOURCE_INCLUDES}")
target_include_directories(Lumina PUBLIC "${GLOBAL_INCLUDES}")
target_include_directories(ImGui PUBLIC "${GLOBAL_INCLUDES}")
