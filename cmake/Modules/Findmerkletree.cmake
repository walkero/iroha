add_library(merkletree UNKNOWN IMPORTED)

find_path(merkletree_INCLUDE_DIR merkletree/merkle_tree.h)
mark_as_advanced(merkletree_INCLUDE_DIR)

find_library(merkletree_LIBRARY merkletree)
mark_as_advanced(merkletree_LIBRARY)

find_package_handle_standard_args(merkletree DEFAULT_MSG
    merkletree_INCLUDE_DIR
    merkletree_LIBRARY
    )

set(URL https://github.com/nickaleks/merkletree)
set(VERSION e147cc797db7217bb61b4a5f61a4bd2fe76818f0)
set_target_description(merkletree "Merkle Tree implementation" ${URL} ${VERSION})


if (NOT merkletree_FOUND)
  externalproject_add(merkletree_impl
      GIT_REPOSITORY ${URL}
      GIT_TAG        ${VERSION}
      CMAKE_ARGS -DBUILD=STATIC
      INSTALL_COMMAND "" # remove install step
      TEST_COMMAND "" # remove test step
      UPDATE_COMMAND "" # remove update step
      )
  externalproject_get_property(merkletree_impl source_dir binary_dir)
  set(merkletree_INCLUDE_DIR ${source_dir}/include)
  set(merkletree_LIBRARY ${binary_dir}/libmerkletree.a)
  file(MAKE_DIRECTORY ${merkletree_INCLUDE_DIR})

  add_dependencies(merkletree merkletree_impl)
endif ()

set_target_properties(merkletree PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${merkletree_INCLUDE_DIR}
    IMPORTED_LOCATION ${merkletree_LIBRARY}
    )
