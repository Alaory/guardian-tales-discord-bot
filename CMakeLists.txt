cmake_minimum_required(VERSION 3.2)


project("GT-BOT")



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)




include_directories("${CMAKE_SOURCE_DIR}/Dep/include")
link_directories("${CMAKE_SOURCE_DIR}/Dep")



set(TOKEN )
add_compile_definitions( TOKEN=\"${TOKEN}\")



add_executable(${PROJECT_NAME} ./src/Main.cpp)
target_link_libraries(${PROJECT_NAME} dpp myhtml ssl crypto firebase_storage firebase_app)
