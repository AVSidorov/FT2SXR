cmake_minimum_required(VERSION 3.10)

cmake_path(SET BARDYDIR NORMALIZE ${CMAKE_SOURCE_DIR}/../..)
cmake_path(SET BARDYLIB NORMALIZE ${BARDYDIR}/bin)
cmake_path(SET GIPCYDIR NORMALIZE ${BARDYDIR}/gipcy)
cmake_path(SET GIPCYLIB NORMALIZE ${GIPCYDIR}/lib)
cmake_path(SET GIPCYINC NORMALIZE ${GIPCYDIR}/include)

message("BARDYDIR is ${BARDYDIR}")
message("BARDYLIB is ${BARDYLIB}")
message("GIPCYDIR is ${GIPCYDIR}")
message("GIPCYLIB is ${GIPCYLIB}")
message("GIPCYINC is ${GIPCYINC}")

# set the project name
project(exam_adc)

# change output directory for executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/bin)

add_compile_definitions(__IPC_LINUX__)

set(SOURCE_FILES exam_adc.cpp
		 adc_ctrl.cpp
		 mem_ctrl.cpp
                 cont_ctrl.cpp	
                 ../../BRDLIBS/reg_rw_spd.cpp)

set(HEADERS adc_ctrl.h
            mu_ctrl.h)

# add precompiled library
find_library(brd  NAMES libbrd.so libbrd
		PATHS ${BARDYLIB} ${BARDYDIR}
		PATH_SUFFIXES bin lib
		REQUIRED)

add_library(libbrd SHARED IMPORTED)


set_property(TARGET libbrd PROPERTY
             IMPORTED_LOCATION ${brd})

target_include_directories(libbrd INTERFACE  "${BARDYDIR}/BRDINC"
					     "${BARDYDIR}/BRDINC/ctrladmpro")

find_library(gipcy  NAMES libgipcy.so
		PATHS ${GIPCYLIB} ${GIPCYDIR}
		PATH_SUFFIXES lib
		REQUIRED)

add_library(libgipcy SHARED IMPORTED)

set_property(TARGET libgipcy PROPERTY
             IMPORTED_LOCATION ${gipcy})

target_include_directories(libgipcy INTERFACE  "${GIPCYINC}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


# add the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE libbrd
                                              libgipcy
											  Threads::Threads)

