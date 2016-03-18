# Locate QT
# This module defines:
# QT_FOUND
#
# $QT_DIR is an environment variable that would
# correspond to the ./configure --prefix=$QT_DIR


IF(APPLE)
    SET(QT_LIB_DIR "clang")
ELSEIF(CMAKE_COMPILER_IS_GNUCXX)
    SET(QT_LIB_DIR "gcc4")
ELSEIF(MSVC80)
    SET(QT_LIB_DIR "msvc2005")
ELSEIF(MSVC90)
    SET(QT_LIB_DIR "msvc2008")
ELSEIF(MSVC10)
    SET(QT_LIB_DIR "msvc2010")
ELSEIF(MSVC11)
    SET(QT_LIB_DIR "msvc2012")
ELSEIF(MSVC12)
    SET(QT_LIB_DIR "msvc2013")
ELSEIF(MSVC14)
    SET(QT_LIB_DIR "msvc2015")
ENDIF()

IF(APPLE)
    # do nothing
ELSEIF(CMAKE_CL_64)
    SET(QT_LIB_DIR ${QT_LIB_DIR}_64)
ELSEIF(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(QT_LIB_DIR ${QT_LIB_DIR}_64)
ELSE()
    # do nothing
ENDIF()



SET( QT_SEARCH_PATHS
    $ENV{QT_DIR}
    "C:/Qt/Qt5.6.0/5.6/${QT_LIB_DIR}"
    "/Users/$ENV{USER}/Qt5.6.0/5.6/${QT_LIB_DIR}"
)

MESSAGE( "Finding QT " ${QT_SEARCH_PATHS}  )


find_package(Qt5Widgets REQUIRED 
    PATHS ${QT_SEARCH_PATHS}
)


