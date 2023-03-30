link_libraries(common)

add_executable(mcf-main implicit.c mcf.c mcfutil.c output.c pbeampp.c pbla.c pflowup.c psimplex.c pstart.c readmin.c treeup.c)
target_link_libraries(mcf-main common)

# target_link_options(mcf-main PRIVATE -Wl,--no-as-needed,-lprofiler,--as-needed)
target_link_options(mcf-main PRIVATE -Wl,--no-as-needed,--as-needed)
