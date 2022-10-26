cmake_minimum_required(VERSION 3.23)
project(FIXParser)

set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(__GCC__)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

add_library(FIXParser STATIC ./src/FIXParser.cpp ./src/FIXMsgBuffer.cpp ./src/FIXParserDefines.cpp ./src/FIXMessageMetadata.cpp
        ./src/FIXGroupMetadata.cpp ./src/FIXGroup.cpp ./src/FIXGroupEntry.cpp ./src/Utils.cpp ./src/FIXMessage.cpp
        ./src/dictionary/FIX44Builder.cpp ./src/dictionary/FIXBlockDefinition.cpp ./src/dictionary/FIXGroupDefinition.cpp
        ./src/dictionary/FIXMsgDefinition.cpp ./src/dictionary/FIXTagDefinition.cpp ./src/dictionary/FIXTagVocabulary.cpp
        )

add_executable(FIXParserTest ./test/test.cpp ./test/testFIXMsgBuffer.cpp ./test/testFIXGroupMetadata.cpp ./test/testBitmap.cpp 
        ./test/testFIXGroup.cpp ./test/testFIXMessageMetadata.cpp ./test/testFIXMessage.cpp ./test/FIXTestBuilder.cpp ./test/testFIXParser.cpp)

add_executable(FIXParserBenchmark ./benchmarks/bench_test.cpp)

target_link_libraries(FIXParserTest gtest pthread FIXParser)

target_link_libraries(FIXParserBenchmark pthread FIXParser)

