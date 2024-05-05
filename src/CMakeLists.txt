file(GLOB_RECURSE SOURCES "*.cpp" "*.cxx" "*.cc")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS *.ui)
file(GLOB_RECURSE RESOURCES "*.qrc")

source_group("src" FILES ${HEADERS} ${SOURCES} ${FORMS})
source_group("src\\qrc" FILES ${RESOURCES})
set(SRCS ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})

add_executable(${PROJECT_NAME} WIN32 ${SRCS})

target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Widgets Qt5::Concurrent)
target_link_libraries(${PROJECT_NAME} qhotkey)
