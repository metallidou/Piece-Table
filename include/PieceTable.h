#ifndef PIECETABLE_H
#define PIECETABLE_H

#include <iostream>
#include <vector>
#include <stack>

enum Operation {
    INSERT,
    ERASE
};

struct Operations {
    Operation type;
    unsigned long start;
    unsigned long length;
    std::string buffer;
};

enum Buffer {
    ORIGINAL,
    ADDED
};

struct Piece {
    Buffer type;
    unsigned long length;
    unsigned long start;
};

class PieceTable {
    public:
        PieceTable(const std::string& textBuffer = "");
        ~PieceTable();
        std::string getText();
        std::string getBufferText(const Piece &piece) const;
        static void splitPiece(Piece &piece1, Piece &piece2, unsigned long bufferPosition, Buffer bufferType);
        bool indexPiece(unsigned long bufferIndex, unsigned long &bufferEntry, unsigned long &textPosition) const;
        void insertPiece(unsigned long textIndex, const std::string &textBuffer, bool pushToUndoStack = true);
        void erasePiece(unsigned long startIndex, unsigned long endIndex, bool pushToUndoStack = true);
        void replacePiece(unsigned long startIndex, unsigned long endIndex, const std::string &textBuffer);
        std::string getTextInBetween(unsigned long startIndex, unsigned long endIndex);
        void undoPiece();
        void redoPiece();

    private:
        std::string original_buffer;
        std::string added_buffer;
        std::vector<Piece> pieces;
        std::stack<Operations> undo_stack;
        std::stack<Operations> redo_stack;
        static Piece newPiece(unsigned long startIndex, unsigned long bufferLength, Buffer bufferType);
        std::string takeSubStringFromPiece(const Piece& piece, unsigned long localStart, unsigned long localLength);
        void performUndo(const Operations& operation);
        void performRedo(const Operations& operation);
};

#endif
