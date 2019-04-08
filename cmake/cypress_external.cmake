
find_package(PythonLibs 2.7 REQUIRED )
find_package(PythonInterp 2.7 REQUIRED)

include(ExternalProject)
ExternalProject_Add(cypress_ext
    GIT_REPOSITORY        "https://github.com/hbp-unibi/cypress/"
    GIT_TAG               master
    UPDATE_COMMAND        git pull
    CMAKE_ARGS            -DCMAKE_BUILD_TYPE=${DCMAKE_BUILD_TYPE} -DSTATIC_LINKING=${STATIC_LINKING} -DCMAKE_INSTALL_PREFIX:path=<INSTALL_DIR> 
    INSTALL_COMMAND 	  ""
    EXCLUDE_FROM_ALL      1
)
ExternalProject_Get_Property(cypress_ext SOURCE_DIR BINARY_DIR)


set(CYPRESS_INCLUDE_DIRS 
	${SOURCE_DIR}/
	${SOURCE_DIR}/external/pybind11/include/
	${BINARY_DIR}/include/
	${PYTHON_INCLUDE_DIRS}
)
set(CYPRESS_LIBRARY
	${BINARY_DIR}/libcypress.a
    ${PYTHON_LIBRARIES}
    -pthread
)

set(GTEST_LIBRARIES
    ${BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/gtest/libgtest.a
	${BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/gtest/libgtest_main.a
)
set(GTEST_INCLUDE_DIRS ${BINARY_DIR}/googletest-prefix/src/googletest/googletest/include/)
set(GTEST_FOUND TRUE)

execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
            "from __future__ import print_function\nimport numpy; print(numpy.get_include(), end='')"
            OUTPUT_VARIABLE numpy_path)
            
find_path(PYTHON_NUMPY_INCLUDE_DIR numpy/arrayobject.h 
    HINTS "${numpy_path}" "${PYTHON_INCLUDE_PATH}" NO_DEFAULT_PATH)

include_directories(
	${CYPRESS_INCLUDE_DIRS}
	${GTEST_INCLUDE_DIRS}
    ${PYTHON_NUMPY_INCLUDE_DIR}
)

