include(CTest)

set(test_sources
cmp.c fmt.c simple.c
)

create_test_sourcelist(test_sources_driver test_sos.c ${test_sources})

add_executable(test_sos ${test_sources_driver})
target_link_libraries(test_sos PRIVATE sos)

foreach(test ${test_sources})
  get_filename_component(test_name ${test} NAME_WE)
  add_test(NAME ${test_name} COMMAND test_sos ${test_name})
endforeach ()
