link_libraries(common)

add_executable(bk-mcf-main implicit.cc mcf.cc mcfutil.cc output.cc pbeampp.cc pbla.cc pflowup.cc psimplex.cc pstart.cc readmin.cc treeup.cc)
target_link_libraries(bk-mcf-main common)

# target_link_options(mcf-main PRIVATE -Wl,--no-as-needed,-lprofiler,--as-needed)
target_link_options(bk-mcf-main PRIVATE -Wl,--no-as-needed,--as-needed)
