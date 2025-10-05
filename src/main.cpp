#include <iostream>
#include "../include/PieceTable.h"

int main()
{
    std::string text = "abcdef";
    PieceTable p(text);

    // Print text
    std::cout << "Text is: " << p.getText() << std::endl;

    // Insert text
    p.insertPiece(6, "gh");
    std::cout << "Text after insert: " << p.getText() << std::endl;

    // Erase text
    p.erasePiece(1, 3);
    std::cout << "Text after erase: " << p.getText() << std::endl;

    // Undo
    p.undoPiece();
    std::cout << "Text after undo: " << p.getText() << std::endl;

    // Redo
    p.redoPiece();
    std::cout << "Text after redo: " << p.getText() << std::endl;

    // Replace text
    p.replacePiece(1, 3, "BC");
    std::cout << "Text after replace: " << p.getText() << std::endl;

    return 0;
}