cmake_minimum_required(VERSION 3.16)
project(poker_gui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

qt_add_library(
    poker_game
    card.cpp
    card.hpp
    deck.cpp
    deck.hpp
    computer_strategy.hpp
    computer_strategy.cpp
    player.cpp
    player.hpp
    poker_game.cpp
    poker_game.hpp
    pokerhand.cpp
    pokerhand.hpp
    poker_hand_evaluation.cpp
    poker_hand_evaluation.hpp
    poker_hand_evaluator.cpp
    poker_hand_evaluator.hpp
    poker_hand_visitor.cpp
    poker_hand_visitor.hpp
    poker_engine_state.hpp
    poker_engine_state.cpp
    poker_engine.hpp
    poker_engine.cpp
    move.hpp
    observer.hpp
    game_event.hpp
    console_logger.hpp
)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.hpp
    mainwindow.ui
    resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(poker_gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET poker_gui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(poker_gui SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(poker_gui
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(poker_game PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(poker_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets poker_game)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.poker_gui)
endif()
set_target_properties(poker_gui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS poker_gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(poker_gui)
endif()

if(APPLE)
    include_directories(SYSTEM /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include)

    set(CMAKE_OSX_ARCHITECTURES "arm64")

    # Add C++ standard library flags for Clang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

  # Find Qt path from the Qt library targets
    get_target_property(_qmake_executable Qt::qmake IMPORTED_LOCATION)
    if(NOT _qmake_executable)
      get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    endif()

    if(_qmake_executable)
      # Extract Qt directory from qmake path
      get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
      # Find macdeployqt in the same directory as qmake
      find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    endif()

    if(NOT MACDEPLOYQT_EXECUTABLE)
      # Fallback search paths if qmake wasn't found
      find_program(MACDEPLOYQT_EXECUTABLE macdeployqt
        HINTS
          "$ENV{QTDIR}/bin"
          "/usr/local/opt/qt/bin"
          "/usr/local/Homebrew/opt/qt/bin"
          "/opt/homebrew/opt/qt/bin"
      )
    endif()

    # if(MACDEPLOYQT_EXECUTABLE)
    #   # Use macdeployqt to handle all deployment tasks automatically
    #   add_custom_command(TARGET poker_gui POST_BUILD
    #       COMMAND "${MACDEPLOYQT_EXECUTABLE}"
    #           "${CMAKE_CURRENT_BINARY_DIR}/poker_gui.app"
    #           -always-overwrite
    #       COMMENT "Running macdeployqt to create a standalone application bundle"
    #   )
    #   message(STATUS "Found macdeployqt: ${MACDEPLOYQT_EXECUTABLE}")
    # else()
    #   message(WARNING "macdeployqt not found. The application bundle may not be properly created.")
    # endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include(CTest)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.16.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_ASSEMBLY_TESTS OFF)

add_executable(
  poker_hand_evaluator_tests
  tests/poker_hand_evaluator_tests.cpp
)

target_link_libraries(
  poker_hand_evaluator_tests
  GTest::gtest_main
  poker_game
)

add_executable(
  poker_engine_tests
  tests/poker_engine_tests.cpp
)

target_link_libraries(
  poker_engine_tests
  GTest::gtest_main
  poker_game
)

add_executable(
  poker_computer_strategy_tests
  tests/poker_computer_strategy_test.cpp
)

target_link_libraries(
  poker_computer_strategy_tests
  GTest::gtest_main
  poker_game
)

include(GoogleTest)
gtest_discover_tests(poker_computer_strategy_tests)
