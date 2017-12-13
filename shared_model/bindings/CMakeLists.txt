# Copyright 2017 Soramitsu Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


#add_library(bindings
#        model_builder.cpp
#        model_crypto.cpp
#        model_transaction_proto.cpp
#        )
#
#target_link_libraries(bindings
#        shared_model_proto_backend
#        shared_model_ed25519_sha3
#        )
#
#
#
#if (SWIG_PYTHON OR SWIG_JAVA OR SWIG_JS)
#    find_package(SWIG 3.0.12 REQUIRED)
#    include(${SWIG_USE_FILE})
#
#    set(CMAKE_SWIG_FLAGS "")
#    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#    set_source_files_properties(bindings.i PROPERTIES CPLUSPLUS ON)
#endif()
#
#if (SWIG_PYTHON)
#    find_package(PythonLibs 2.7 REQUIRED)
#    # path to where Python.h is found
#    include_directories(${PYTHON_INCLUDE_DIRS})
#
#    if (${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
#        set(MAC_OPTS "-flat_namespace -undefined suppress")
#    endif()
#
#    swig_add_library(iroha LANGUAGE python SOURCES bindings.i)
#    swig_link_libraries(iroha ${Python_LIBRARIES} bindings ${MAC_OPTS})
#endif()
#
#if (SWIG_JAVA)
#    find_package(JNI REQUIRED)
#    # the include path to jni.h
#    include_directories(${JAVA_INCLUDE_PATH})
#    # the include path to jni_md.h
#    include_directories(${JAVA_INCLUDE_PATH2})
#
#    swig_add_library(iroha LANGUAGE java SOURCES bindings.i)
#    swig_link_libraries(iroha ${Java_LIBRARIES} bindings)
#endif()

#if (SWIG_JS)
#    include_directories(/usr/local/Cellar/node@6/6.12.2/include/node/)
#    set_property (SOURCE bindings.i PROPERTY SWIG_FLAGS "-node")
#    swig_add_library(irohajs LANGUAGE javascript SOURCES bindings.i)
#    swig_link_libraries(irohajs bindings "-flat_namespace -undefined suppress")
#endif()

include_directories(${CMAKE_JS_INC})
add_library(irohajs SHARED
        model_builder.cpp
        model_crypto.cpp
        model_transaction_proto.cpp)
set_target_properties(irohajs PROPERTIES PREFIX "" SUFFIX ".node")
target_include_directories(irohajs PRIVATE ${CMAKE_JS_INC})
target_link_libraries(irohajs ${CMAKE_JS_LIB} shared_model_proto_backend shared_model_ed25519_sha3)
