if (WIN32)
    cmake_minimum_required(VERSION 3.0)
    project(ametrine C)

    set(CMAKE_C_STANDARD 11)

    add_executable(ametrine ametrine.c external/glad/src/glad.c)

    find_package(OpenGL REQUIRED)

    include_directories(external/glad/include ${OPENGL_INCLUDE_DIRS} ${X11_INCLUDE_DIR} external/stb_image m)

    target_link_libraries(ametrine ${OPENGL_LIBRARIES} ${X11_LIBRARIES} m gdi32 winmm)

else()

    cmake_minimum_required(VERSION 3.0)
    project(ametrine C)

    set(CMAKE_C_STANDARD 11)

    add_executable(ametrine compute_test.c external/glad/src/glad.c)

    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)


    include_directories(external/glad/include ${OPENGL_INCLUDE_DIRS} ${X11_INCLUDE_DIR} external/stb_image m)
    target_link_libraries(ametrine ${OPENGL_LIBRARIES} ${X11_LIBRARIES} m dl)

endif (WIN32)