# set(SRC_LIST calluserservice.cc ../user.pb.cc)
set(SRC_LIST callfriendservice.cc ../friend.pb.cc)
add_executable(customer ${SRC_LIST})
target_link_libraries(customer mprpc protobuf)