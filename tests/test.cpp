#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../PieceTable.h"

TEST_CASE("Piece Table: index operation", "[indexing]")
{
    std::string text = "This is just a text";
    unsigned long e, b;

    PieceTable p1(text);
    p1.indexPiece(8, e, b);
    REQUIRE(e == 0);
    REQUIRE(b == 8);
}

TEST_CASE("Piece Table: split operation", "[split]")
{
    std::string text = "This is just a text";
    std::string text1 = "This is ";
    std::string text2 = "just a text";
    std::string text3 = "just ";
    std::string text4 = "a text";
    std::string text5 = "j";
    std::string text6 = "ust ";
    PieceTable p(text);
    Piece p1{}, p2{}, p3{}, p4{};
    p1.start = 0;
    p1.length = text.length();
    p1.type = ORIGINAL;

    // "This is just a text" -> "This is " + "just a text"
    PieceTable::splitPiece(p1, p2, 8, p1.type);
    REQUIRE(p1.length == 8);
    REQUIRE(p2.length == 11);
    REQUIRE(text.substr(p1.start, p1.length) == text1);
    REQUIRE(text.substr(p2.start, p2.length) == text2);
    // "just a text" -> "just " + "a text"
    PieceTable::splitPiece(p2, p3, 5, p2.type);
    REQUIRE(p2.length == 5);
    REQUIRE(p3.length == 6);
    REQUIRE(text.substr(p2.start, p2.length) == text3);
    REQUIRE(text.substr(p3.start, p3.length) == text4);
    // "just " -> "j" + "ust "
    PieceTable::splitPiece(p2, p4, 1, p2.type);
    REQUIRE(p2.length == 1);
    REQUIRE(p4.length == 4);
    REQUIRE(text.substr(p2.start, p2.length) == text5);
    REQUIRE(text.substr(p4.start, p4.length) == text6);

    REQUIRE(p.getText() == text);
}

TEST_CASE("Piece Table: insert operation", "[insertion]")
{
    std::string text1 = "this is just a text";
    std::string text2 = "this is not just a text";
    std::string text3 = "this is not just a plain text";
    std::string text4 = "Well, this is not just a plain text";
    std::string text5 = "Well, this is not just a plain text!";
    std::string text6 = "\nThis is an insertion test.";
    std::string text7 = "Well, this is not just a plain text!\nThis is an insertion test.";
    PieceTable p(text1);

    REQUIRE(p.getText() == text1);

    p.insertPiece(8, "not ");
    REQUIRE(p.getText() == text2);

    p.insertPiece(19, "plain ");
    REQUIRE(p.getText() == text3);

    p.insertPiece(0, "Well, ");
    REQUIRE(p.getText() == text4);

    p.insertPiece(35, "!");
    REQUIRE(p.getText() == text5);

    p.insertPiece(36, text6);
    REQUIRE(p.getText() == text7);
}

TEST_CASE("Piece Table: delete operation", "[deletion]")
{
    std::string text = "This is not just a plain text!";
    std::string text1 = "This is just a plain text!";
    std::string text2 = "This is just a text!";
    std::string text3 = "This is just a text";
    std::string text4 = "This is a text";
    std::string text5 = "a text";
    std::string text6 = "a txt";
    std::string text7 = " txt";
    std::string text8 = "";
    std::string text9 = "final erase test";
    std::string text10 = "f";
    PieceTable p(text);

    unsigned long e, b;

    // "This is not just a plain text!" -> "This is just a plain text!"
    p.indexPiece(8, e, b);
    REQUIRE(e == 0);
    REQUIRE(b == 8);
    p.erasePiece(8, 12);
    REQUIRE(p.getText() == text1);

    // "This is just a plain text!" -> "This is just a text!"
    p.erasePiece(15, 21);
    REQUIRE(p.getText() == text2);

    // "This is just a text!" -> "This is just a text"
    p.erasePiece(19, 20);
    REQUIRE(p.getText() == text3);

    // "This is just a text" -> "This is a text"
    p.erasePiece(8, 13);
    REQUIRE(p.getText() == text4);

    // "This is a text" -> "a text"
    p.erasePiece(0, 8);
    REQUIRE(p.getText() == text5);

    // "a text" -> "a txt"
    p.erasePiece(3, 4);
    REQUIRE(p.getText() == text6);

    // "a txt" -> " txt"
    p.erasePiece(0, 1);
    REQUIRE(p.getText() == text7);

    // " txt" -> ""
    p.erasePiece(0, p.getText().length());
    REQUIRE(p.getText() == text8);

    // "" -> "final erase test" -> "f"
    p.insertPiece(0, text9);
    REQUIRE(p.getText() == text9);
    p.erasePiece(1, p.getText().length());
    REQUIRE(p.getText() == text10);
}

TEST_CASE("Piece Table: getTextInBetween", "[getTextInBetween]")
{
    std::string text = "abcdef";
    std::string text1 = "d";
    std::string text2 = "";
    std::string text3 = "ef";
    PieceTable p(text);

    // Entire text
    REQUIRE(p.getTextInBetween(0, 6) == text);

    // Single character
    REQUIRE(p.getTextInBetween(3, 4) == text1);

    // Zero length text
    REQUIRE(p.getTextInBetween(2, 2) == text2);

    // Out-of-bounds range
    REQUIRE(p.getTextInBetween(4, 10) == text3);
}

TEST_CASE("Piece Table: undo-insert", "[undo]")
{
    std::string text = "This is an undo insert test";
    std::string text1 = "This is an undo insert test!";
    std::string text2 = "with ";
    std::string text3 = "This is an undo with insert test";
    PieceTable p(text);

    // "This is an undo insert test" -> "This is an undo insert test!"
    p.insertPiece(27, "!");
    REQUIRE(p.getText() == text1);

    // "This is an undo insert test!" -> "This is an undo insert test"
    p.undoPiece();
    REQUIRE(p.getText() == text);

    // "This is an undo insert test" -> "This is an undo with insert test"
    p.insertPiece(16, text2);
    REQUIRE(p.getText() == text3);

    // "This is an undo with insert test" -> "This is an undo insert test"
    p.undoPiece();
    REQUIRE(p.getText() == text);
}

TEST_CASE("Piece Table: undo-erase", "[undo]")
{
    std::string text = "abcdefg";
    std::string text1 = "abefg";
    std::string text2 = "cdefg";
    std::string text3 = "abcde";
    PieceTable p(text);

    // "abcdefg" -> "abefg"
    p.erasePiece(2, 4);
    REQUIRE(p.getText() == text1);

    // "abefg" -> "abcdefg"
    p.undoPiece();
    REQUIRE(p.getText() == text);

    // "abcdefg" -> "cdefg"
    p.erasePiece(0, 2);
    REQUIRE(p.getText() == text2);

    // "cdefg" -> "abcdefg"
    p.undoPiece();
    REQUIRE(p.getText() == text);

    // "abcdefg" -> "abcde"
    p.erasePiece(5, 7);
    REQUIRE(p.getText() == text3);

    // "abcde" -> "abcdefg"
    p.undoPiece();
    REQUIRE(p.getText() == text);
}

TEST_CASE("Piece Table: undo insert/erase", "[undo]")
{
    std::string text = "12345";
    std::string text1 = "1AB2345";
    std::string text2 = "AB";
    std::string text3 = "1AB25";
    PieceTable p(text);

    // "12345" -> "1AB2345"
    p.insertPiece(1, text2);
    REQUIRE(p.getText() == text1);

    // "1AB2345" -> "1AB25"
    p.erasePiece(4, 6);
    REQUIRE(p.getText() == text3);

    // "1AB25" -> "1AB2345"
    p.undoPiece();
    REQUIRE(p.getText() == text1);

    // "1AB2345" -> "12345"
    p.undoPiece();
    REQUIRE(p.getText() == text);
}

TEST_CASE("Piece Table: redo insert/erase", "[redo]")
{
    std::string text = "Hello";
    std::string text1 = " World";
    std::string text2 = "Hello World";
    PieceTable p(text);

    // "Hello" -> "Hello World"
    p.insertPiece(5, text1);
    REQUIRE(p.getText() == text2);

    // "Hello World" -> "Hello"
    p.undoPiece();
    REQUIRE(p.getText() == text);

    // "Hello" -> "Hello World"
    p.redoPiece();
    REQUIRE(p.getText() == text2);

    // "Hello World" -> "Hello"
    p.erasePiece(5, 11);
    REQUIRE(p.getText() == text);

    // "Hello" -> "Hello World"
    p.undoPiece();
    REQUIRE(p.getText() == text2);

    // "Hello World" -> "Hello"
    p.redoPiece();
    REQUIRE(p.getText() == text);
}

TEST_CASE("Piece Table: undo/redo sequence", "[undo][redo]")
{
    std::string text = "abcde";
    std::string text1 = "FG";
    std::string text2 = "abcdeFG";
    std::string text3 = "XY";
    std::string text4 = "abXYcdeFG";
    std::string text5 = "acdeFG";
    PieceTable p(text);

    // "abcde" -> "abcdeFG"
    p.insertPiece(5, text1);
    REQUIRE(p.getText() == text2);

    // "abcdeFG" -> "abXYcdeFG"
    p.insertPiece(2, text3);
    REQUIRE(p.getText() == text4);

    // "abXYcdeFG" -> "acdeFG"
    p.erasePiece(1, 4);
    REQUIRE(p.getText() == text5);

    // "acdeFG" -> "abXYcdeFG"
    p.undoPiece();
    REQUIRE(p.getText() == text4);

    // "abXYcdeFG" -> "abcdeFG"
    p.undoPiece();
    REQUIRE(p.getText() == text2);

    // "abcdeFG" -> "abXYcdeFG"
    p.redoPiece();
    REQUIRE(p.getText() == text4);

    // "abXYcdeFG" -> "acdeFG"
    p.redoPiece();
    REQUIRE(p.getText() == text5);
}
