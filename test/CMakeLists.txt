file(GLOB_RECURSE test_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.c *.cpp)

create_test_sourcelist(test_sources_driver test_sos.c ${test_sources})

add_executable(test_sos ${test_sources_driver})
target_link_libraries(test_sos PRIVATE sos)

foreach(test ${test_sources})
  get_filename_component(test_name ${test} NAME_WE)
  add_test(NAME ${test_name} COMMAND test_sos ${test_name})
endforeach ()
