# build libraries first
add_subdirectory(common)
add_subdirectory(root)
add_subdirectory(digitizer)

# now build executables
add_subdirectory(exec)
