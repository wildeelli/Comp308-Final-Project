SET(SOURCES 
	loadAssets.cpp
	loadShader.cpp
	main.cpp
	HeightField.cpp
	Mesh.cpp
	
)

SET(HEADERS
	loadAssets.hpp
	loadShader.h
	HeightField.h
	Mesh.hpp
	sphere.h
)

#add_library(main ${SOURCES} ${HEADERS})

list(APPEND LIBRARIES_impl glew ${GLEW_LIBRARIES} glfw ${GLFW_LIBRARIES} zlib png16_static)
list(APPEND DEFINITIONS_impl ${GLEW_DEFINITIONS} ${ZLIB_DEFINITIONS})

set(LIBRARIES ${LIBRARIES_impl} CACHE STRING "required libraries" FORCE)
set(DEFINITIONS ${DEFINITIONS_impl} CACHE STRING "required definitions" FORCE)

add_definitions(${DEFINITIONS})

add_executable(final ${SOURCES} ${HEADERS})
add_definitions(${DEFINITIONS})
target_link_libraries(final ${LIBRARIES})

set_target_properties(final PROPERTIES LINKER_LANGUAGE CXX)

