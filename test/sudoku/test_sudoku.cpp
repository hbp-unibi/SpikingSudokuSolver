#include "gtest/gtest.h"

#include <algorithm>
#include <iostream>

#include "source/utils/Sudoku.hpp"

using namespace std;

TEST(sudoku, constructor)
{
    std::cout.rdbuf(nullptr);
    EXPECT_ANY_THROW(Sudoku ("Sudoku.json"));
    EXPECT_NO_THROW(Sudoku ("Test.json"));
    EXPECT_ANY_THROW(Sudoku (-1,3));
    EXPECT_ANY_THROW(Sudoku (3,-1));
    EXPECT_ANY_THROW(Sudoku (3,9));
    EXPECT_ANY_THROW(Sudoku (9,3));
    EXPECT_ANY_THROW(Sudoku (4,5));
    EXPECT_ANY_THROW(Sudoku (5,4));                                                                                           
}

TEST(sudoku, getEntryInSquare)
{
    std::cout.rdbuf(nullptr);
    Sudoku s("Test.json");
    EXPECT_EQ(2, s.getEntryInSquare(0,0));
    EXPECT_EQ(4, s.getEntryInSquare(1,3));
    EXPECT_ANY_THROW(s.getEntryInSquare(10,10));
}

TEST(sudoku, setEntryInSquare)
{
    std::cout.rdbuf(nullptr);
    Sudoku s("Test.json");
    s.setEntryInSquare(0,3,4);
    EXPECT_EQ(4, s.getEntryInSquare(0,3));
    EXPECT_ANY_THROW(s.setEntryInSquare(10,10,5));
    EXPECT_ANY_THROW(s.setEntryInSquare(0,0,10));
    EXPECT_ANY_THROW(s.setEntryInSquare(0,0,-5));
}

TEST(sudoku, reset)
{
    std::cout.rdbuf(nullptr);
    Sudoku s("Test.json");
    EXPECT_EQ(s.getSudokuBlank(), s.getSudoku());    
    s.setEntryInSquare(0,2,4);
    EXPECT_NE(s.getSudokuBlank(), s.getSudoku());
    s.reset();
    EXPECT_EQ(s.getSudokuBlank(), s.getSudoku());
}

TEST(sudoku, valid)
{
    std::cout.rdbuf(nullptr);
    Sudoku s("Test.json");
    EXPECT_TRUE(s.valid());
    s.setEntryInSquare(0,2,4);
    EXPECT_FALSE(s.valid());
    s.reset();
    EXPECT_TRUE(s.valid());
}
