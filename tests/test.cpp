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
    p.erasePiece(8, 11);
    REQUIRE(p.getText() == text1);

    // "This is just a plain text!" -> "This is just a text!"
    p.erasePiece(15, 20);
    REQUIRE(p.getText() == text2);

    // "This is just a text!" -> "This is just a text"
    p.erasePiece(19, 19);
    REQUIRE(p.getText() == text3);

    // "This is just a text" -> "This is a text"
    p.erasePiece(8, 12);
    REQUIRE(p.getText() == text4);

    // "This is a text" -> "a text"
    p.erasePiece(0, 7);
    REQUIRE(p.getText() == text5);

    // "a text" -> "a txt"
    p.erasePiece(3, 3);
    REQUIRE(p.getText() == text6);

    // "a txt" -> " txt"
    p.erasePiece(0, 0);
    REQUIRE(p.getText() == text7);

    // " txt" -> ""
    p.erasePiece(0, p.getText().length()-1);
    REQUIRE(p.getText() == text8);

    // "" -> "final erase test" -> "f"
    p.insertPiece(0, text9);
    REQUIRE(p.getText() == text9);
    p.erasePiece(1, p.getText().length()-1);
    REQUIRE(p.getText() == text10);
}

TEST_CASE("Piece Table: undo operation", "[undo]")
{
    std::string text = "This is not just a plain text!";
    std::string text1 = "";
    std::string text2 = "This is just a plain text!";
    std::string text3 = "This is just a plain text";
    PieceTable p(text);

    // "This is not just a plain text!" -> ""
    p.undoPiece();
    REQUIRE(p.getText() == text1);

    // "" -> "This is just a plain text!" -> "This is just a plain text" -> "This is just a plain text!"
    p.insertPiece(0, text2);
    p.erasePiece(25, 25);
    REQUIRE(p.getText() == text3);
    p.undoPiece();
    REQUIRE(p.getText() == text3);
}

TEST_CASE("Piece Table: undo insert", "[undo]") {
    std::string base = "Hello world";
    PieceTable p(base);

    // Insert at end
    p.insertPiece(11, "!");
    REQUIRE(p.getText() == "Hello world!");

    // Undo insertion
    p.undoPiece();
    REQUIRE(p.getText() == base);

    // Insert in middle
    p.insertPiece(5, ", brave new");
    REQUIRE(p.getText() == "Hello, brave new world");

    // Undo insertion
    p.undoPiece();
    REQUIRE(p.getText() == base);
}

TEST_CASE("Piece Table: undo erase", "[undo]") {
    std::string base = "abcdefg";
    PieceTable p(base);

    // Erase middle ("cd")
    p.erasePiece(2, 3);
    REQUIRE(p.getText() == "abefg");

    // Undo erase -> restore original
    p.undoPiece();
    REQUIRE(p.getText() == base);

    // Erase at start ("ab")
    p.erasePiece(0, 1);
    REQUIRE(p.getText() == "cdefg");

    // Undo erase
    p.undoPiece();
    REQUIRE(p.getText() == base);

    // Erase at end ("fg")
    p.erasePiece(5, 6);
    REQUIRE(p.getText() == "abcde");

    // Undo erase
    p.undoPiece();
    REQUIRE(p.getText() == base);
}

TEST_CASE("Piece Table: undo multiple operations", "[undo]") {
    PieceTable p("12345");

    // Insert "AB" after "1" -> "1AB2345"
    p.insertPiece(1, "AB");
    REQUIRE(p.getText() == "1AB2345");

    // Erase "34" -> "1AB25"
    p.erasePiece(4, 5);
    REQUIRE(p.getText() == "1AB25");

    // Undo erase -> back to "1AB2345"
    p.undoPiece();
    REQUIRE(p.getText() == "1AB2345");

    // Undo insert -> back to "12345"
    p.undoPiece();
    REQUIRE(p.getText() == "12345");
}

TEST_CASE("Piece Table: undo consecutive inserts", "[undo]") {
    PieceTable p("X");

    p.insertPiece(1, "Y");
    REQUIRE(p.getText() == "XY");

    p.insertPiece(2, "Z");
    REQUIRE(p.getText() == "XYZ");

    // Undo last insert -> back to "XY"
    p.undoPiece();
    REQUIRE(p.getText() == "XY");

    // Undo again -> back to "X"
    p.undoPiece();
    REQUIRE(p.getText() == "X");
}
