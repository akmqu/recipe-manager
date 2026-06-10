cmake_minimum_required(VERSION 3.5)

project(RecipeManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Sql Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Charts)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources.qrc

    database/DatabaseManager.cpp
    database/DatabaseManager.h

    utils/RecipeImageStorage.cpp
    utils/RecipeImageStorage.h

    models/Recipe.h

    widgets/recipecard.cpp
    widgets/recipecard.h
    widgets/recipecard.ui
    widgets/RecipeGridBrowser.cpp
    widgets/RecipeGridBrowser.h
    widgets/recipegridbrowser.ui

    pages/AllRecipesPage.cpp
    pages/AllRecipesPage.h
    pages/AddRecipePage.cpp
    pages/AddRecipePage.h
    pages/AddRecipePage.ui 
    

    pages/FavoritesPage.cpp
    pages/FavoritesPage.h
    pages/ShoppingListPage.cpp
    pages/ShoppingListPage.h
    pages/ShoppingListPage.ui
    pages/StatisticsPage.cpp
    pages/StatisticsPage.h
    pages/StatisticsPage.ui
    pages/MealPlannerPage.cpp
    pages/MealPlannerPage.h
    pages/MealPlannerPage.ui
    pages/RecipeDetailsPage.cpp
    pages/RecipeDetailsPage.h
    pages/RecipeDetailsPage.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RecipeManager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RecipeManager APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RecipeManager SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RecipeManager
        ${PROJECT_SOURCES}  
    )
    endif()
endif()

target_link_libraries(RecipeManager PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Charts
)

target_include_directories(RecipeManager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RecipeManager)
endif()
set_target_properties(RecipeManager PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS RecipeManager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RecipeManager)
endif()
