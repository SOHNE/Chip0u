add_library(vendor INTERFACE)

# Raylib
set(CUSTOMIZE_BUILD ON CACHE BOOL "Customize Raylib build")
set(SUPPORT_MODULE_RMODELS OFF CACHE BOOL "Support module rmodels")
add_subdirectory( raylib )
target_link_libraries(vendor INTERFACE raylib)

# ImGui
add_subdirectory( imgui )
target_link_libraries(vendor INTERFACE imgui)

# ImGui Raylib
add_subdirectory( rlimgui )
target_link_libraries(vendor INTERFACE rlimgui)

# ImGui File Dialog
add_subdirectory( ImGuiFileDialog )
target_link_libraries(ImGuiFileDialog PRIVATE imgui)
target_link_libraries(vendor INTERFACE ImGuiFileDialog)

# stb from ImGuiFileDialog
target_include_directories(vendor INTERFACE ImGuiFileDialog/stb)

# dirent from ImGuiFileDialog
if(WIN32)
    target_include_directories(vendor INTERFACE ImGuiFileDialog/dirent)
endif()
