cmake_minimum_required(VERSION 3.10.0)
project(ocr_recorder VERSION 1.1 LANGUAGES C CXX)


# 添加共享库目标
add_library(dll_ocr_recorder SHARED src/dll_ocr_recorder.cpp)
set_target_properties(dll_ocr_recorder PROPERTIES PREFIX "")
# target_link_libraries(dll_ocr_recorder lua gdiplus gdi32 ole32 tesseract opencv_calib3d4120 opencv_core4120 opencv_dnn4120 opencv_features2d4120 opencv_flann4120 opencv_gapi4120 opencv_highgui4120 opencv_imgcodecs4120 opencv_imgproc4120 opencv_ml4120 opencv_objdetect4120 opencv_photo4120 opencv_stitching4120 opencv_video4120 opencv_videoio4120 uxtheme urlmon shlwapi leptonica)
target_link_libraries(dll_ocr_recorder lua gdiplus gdi32 ole32 tesseract.dll opencv_calib3d455.dll opencv_core455.dll opencv_dnn455.dll opencv_features2d455.dll opencv_flann455.dll opencv_gapi455.dll opencv_highgui455.dll opencv_imgcodecs455.dll opencv_imgproc455.dll opencv_ml455.dll opencv_objdetect455.dll opencv_photo455.dll opencv_stitching455.dll opencv_video455.dll opencv_videoio455.dll uxtheme urlmon shlwapi leptonica.dll)

# 编译器标志设置
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")   # 防止控制台窗口弹出


# 创建共享库
add_library(region_selector SHARED src/region_selector_dll.cpp)

# 链接系统库
# link_directories(D:/ProgKits/msys2/mingw64/lib1)
target_link_libraries(region_selector uxtheme gdi32)
set_target_properties(region_selector PROPERTIES PREFIX "")


# LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(test_dll_ocr_recorder src/test_dll_ocr_recorder.cpp)
TARGET_LINK_LIBRARIES(test_dll_ocr_recorder dll_ocr_recorder)
# TARGET_LINK_LIBRARIES(test_dll_ocr_recorder dll_ocr_recorder lua gdiplus gdi32 ole32 tesseract_x64 opencv_calib3d452 opencv_core452 opencv_dnn452 opencv_features2d452 opencv_flann452 opencv_gapi452 opencv_highgui452 opencv_imgcodecs452 opencv_imgproc452 opencv_ml452 opencv_objdetect452 opencv_photo452 opencv_stitching452 opencv_video452 opencv_videoio452 uxtheme urlmon shlwapi leptonica)


# # 设置可执行文件输出路径
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_custom_command(TARGET test_dll_ocr_recorder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/src/dll_ocr_recorder.lua
        ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy  
        ${CMAKE_SOURCE_DIR}/src/ocr_recorder.patterns.config
        ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy  
        ${CMAKE_SOURCE_DIR}/Images/example08.png
        ${CMAKE_BINARY_DIR}/Images/example08.png
    COMMENT "Copying resource files"
)


# 设置AutoHotkey编译器路径和参数
set(AHK_COMPILER "D:/Program Files/AutoHotkey/Compiler/Ahk2Exe.exe")
set(AHK_SOURCE "${CMAKE_SOURCE_DIR}/src/ocr_recorder.ahk")
# set(AHK_BASE "D:/Program Files/AutoHotkey/v1.1.37.02/Unicode 64-bit.bin")
set(AHK_BASE "D:/Program Files/AutoHotkey/v1.1.34.04/Unicode 64-bit.bin")
set(AHK_ICON "${CMAKE_SOURCE_DIR}/Images/ocr_recorder_logo.ico")
set(AHK_EXE "${CMAKE_BINARY_DIR}/ocr_recorder.exe")


# 添加自定义编译命令
add_custom_command(
    OUTPUT "${AHK_EXE}"
    COMMAND "${AHK_COMPILER}" 
            /in "${AHK_SOURCE}"
            /out "${AHK_EXE}"  # 关键修改：添加/out参数
            /base "${AHK_BASE}" 
            /icon "${AHK_ICON}"
    DEPENDS "${AHK_SOURCE}"
    COMMENT "Compiling AutoHotkey script to EXE"
    VERBATIM
)


# 修改自定义目标，确保依赖关系
add_custom_target(projectMain ALL
    DEPENDS region_selector "${AHK_EXE}" test_dll_ocr_recorder
    COMMENT "Building AHK executable with region_selector dependency"
)

# 添加测试配置
add_test(
    NAME solid_box_create_hide_test
    COMMAND "${AHK_EXE}"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_test(NAME test1 COMMAND test_dll_ocr_recorder) 




# 先设置所有CPack变量
# set(CPACK_PACKAGE_VERSION "1.1.0")
set(CPACK_PACKAGE_VENDOR "ChaosMage")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})


set(CPACK_GENERATOR "NSIS64")  # 使用NSIS生成Windows安装包



# # NSIS相关参数参见 https://cmake.org/cmake/help/latest/cpack_gen/nsis.html
set(CPACK_NSIS_INSTALL_ROOT "D:\\\\Program Files")  # 设置NSIS安装根目录
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME}) # 设置安装目录名称


# 最后再包含CPack模块
include(CPack)



install(PROGRAMS ${AHK_EXE}
    DESTINATION bin
    RENAME "ocr_recorder.exe"
)

# # 确保可执行文件被安装
install(TARGETS test_dll_ocr_recorder dll_ocr_recorder region_selector
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)



# 可选：安装其他资源文件（如文档、配置文件等）
install(FILES README.md DESTINATION docs)

install(FILES src/dll_ocr_recorder.lua src/ocr_recorder.patterns.config src/ocr_recorder.words.config src/ocr_tesser.config
    DESTINATION bin
)

install(FILES Images/example08.png DESTINATION bin/Images)



include(CTest)
enable_testing()


#[[
# 参考资料：
# CMake官方文档：https://cmake.org/documentation/
# CPack官方文档：https://cmake.org/cmake/help/latest/module/CPack
]]
