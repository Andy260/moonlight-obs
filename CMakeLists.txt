cmake_minimum_required(VERSION 3.28...3.30)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" OFF)
option(ENABLE_QT "Use Qt functionality" OFF)

include(compilerconfig)
include(defaults)
include(helpers)

add_library(${CMAKE_PROJECT_NAME} MODULE)

# Add Moonlight Common C dependency
# and link as a static library
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(deps/moonlight-common-c)
include_directories(deps/moonlight-common-c/src)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE moonlight-common-c)

# Add mdns dependency
include_directories(deps/mdns)

# Add libcurl dependency
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${CURL_LIBRARIES})

# Add TinyXML2 dependency
include_directories(deps/tinyxml2)
target_sources(${CMAKE_PROJECT_NAME} 
    PRIVATE deps/tinyxml2/tinyxml2.cpp
)

find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

if(ENABLE_FRONTEND_API)
  find_package(obs-frontend-api REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)
endif()

if(ENABLE_QT)
  find_package(Qt6 COMPONENTS Widgets Core)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
  target_compile_options(
    ${CMAKE_PROJECT_NAME}
    PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header -Wno-comma>
  )
  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
  )
endif()

target_sources(${CMAKE_PROJECT_NAME} 
  PRIVATE src/Connections/Address.cpp
          src/Discovery/LANSearcher.cpp
          src/Discovery/mDNSRecordExtractor.cpp
          src/Forms/FindHostsDialog.cpp
          src/Forms/ManualPairingDialog.cpp
          src/plugin-main.cpp
          src/Properties.cpp
          src/OBSSource.cpp
)

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})
