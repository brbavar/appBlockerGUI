# Install script for directory: /Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/build/cmake/lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/base/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/net/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/core/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/adv/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/aui/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/html/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/propgrid/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/ribbon/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/richtext/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/webview/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/stc/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/xrc/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/media/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/gl/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/qa/cmake_install.cmake")
  include("/Users/benbavar/appBlockerFrontend/wxWidgets-3.2.2.1/mac-build/libs/xml/cmake_install.cmake")

endif()

