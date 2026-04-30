cmake_minimum_required(VERSION 4.0)
project(BitmapManipulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(BitmapManipulator Source/main.cpp
        Source/Bitmap.cpp
        Header/Bitmap.h
        Source/Terminal.cpp
        Header/Terminal.h
        Source/MoveGenerator.cpp
        Header/MoveGenerator.h
        Source/ChessBoard.cpp
        Header/ChessBoard.h
        Source/MoveList.cpp
        Source/PerformanceTest.cpp
        Header/PerformanceTest.h
        Source/Evaluator.cpp
        Header/Evaluator.h
        Source/Engine.cpp
        Header/Engine.h
        Source/TranspositionTable.cpp
        Header/TranspositionTable.h
        Source/UCI.cpp
        Header/UCI.h
        #        Source/MagicNumberGenerator.cpp
#        Header/MagicNumberGenerator.h
)
