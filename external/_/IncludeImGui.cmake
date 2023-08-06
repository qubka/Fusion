set(IMGUI_SOURCES
        "imgui/imgui.cpp"
        "imgui/imgui_demo.cpp"
        "imgui/imgui_draw.cpp"
        "imgui/imgui_tables.cpp"
        "imgui/imgui_widgets.cpp"
        "imguizmo/GraphEditor.cpp"
        "imguizmo/ImCurveEdit.cpp"
        "imguizmo/ImGradient.cpp"
        "imguizmo/ImGuizmo.cpp"
        "imguizmo/ImSequencer.cpp"

        "imgui-plugins/ImTextEditor.cpp"
        )

set(IMGUI_HEADERS
        "imgui/imconfig.h"
        "imgui/imgui.h"
        "imgui/imgui_internal.h"
        #"imgui/imstb_rectpack.h"
        #"imgui/imstb_textedit.h"
        #"imgui/imstb_truetype.h"

        "imgui-plugins/ImFileBrowser.h"
        "imgui-plugins/ImTextEditor.h"

        "imguizmo/GraphEditor.h"
        "imguizmo/ImCurveEdit.h"
        "imguizmo/ImGradient.h"
        "imguizmo/ImGuizmo.h"
        "imguizmo/ImSequencer.h"
        "imguizmo/ImZoomSlider.h"
        )

add_library(imgui
        STATIC
        ${IMGUI_SOURCES}
        ${IMGUI_HEADERS}
        )

target_compile_definitions(imgui
        PRIVATE
        #IMGUI_IMPL_OPENGL_LOADER_GLAD=1
        USE_IMGUI_API
        )

target_include_directories(imgui
        PUBLIC
        "imgui/"
        "imguizmo/"
        "imgui-plugins/"
        "${GLFW_INC_PATH}"
        #"${GLAD_INC_PATH}"
        )

list(APPEND ALL_LIBS imgui)