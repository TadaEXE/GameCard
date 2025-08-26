add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/app)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/util)

set(SRC_INCLUDES 
  ${CMAKE_CURRENT_LIST_DIR}/
  CACHE INTERNAL ""
)

message("Includes: " ${SRC_INCLUDES})

set(SRC_SOURCES
  ${APP_SOURCES}
  ${UTIL_SOURCES}
  CACHE INTERNAL ""
)

message("Sources: " ${SRC_SOURCES})
