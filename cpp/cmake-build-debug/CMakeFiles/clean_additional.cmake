# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "/Users/admin/git/Pir-SDK/cpp/cgo/pirGO_wrap.cxx"
  "/Users/admin/git/Pir-SDK/cpp/cjava/pir.java"
  "/Users/admin/git/Pir-SDK/cpp/cjava/pirJAVA_wrap.cxx"
  "/Users/admin/git/Pir-SDK/cpp/cjava/pirJNI.java"
  )
endif()
