#include <string>
#include "../include/PieceTable.h"

PieceTable::PieceTable(const std::string &textBuffer)
{
    Piece newpiece = newPiece(0, static_cast<int>(textBuffer.length()), ORIGINAL);
    pieces.push_back(newpiece);
    original_buffer = textBuffer;
    added_buffer = "";
}

PieceTable::~PieceTable() = default;

Piece PieceTable::newPiece(const unsigned long startIndex, const unsigned long bufferLength, Buffer bufferType)
{
    Piece newpiece{};
    newpiece.start = startIndex;
    newpiece.length = bufferLength;
    newpiece.type = bufferType;
    return newpiece;
}

std::string PieceTable::getBufferText(const Piece &piece) const
{
    const std::string& buffer = (piece.type == ORIGINAL) ? original_buffer : added_buffer;
    if (piece.start >= buffer.size() || piece.length == 0) return "";
    return buffer.substr(piece.start, std::min(piece.length, buffer.size() - piece.start));
}

std::string PieceTable::getText()
{
    std::string text;
    for (const auto &p : pieces) text += getBufferText(p);
    return text;
}

bool PieceTable::indexPiece(unsigned long bufferIndex, unsigned long &bufferEntry, unsigned long &textPosition) const
{
    if (pieces.empty()) return false;

    unsigned long pos = 0;
    for (size_t i = 0; i < pieces.size(); i++) {
        if (bufferIndex < pos + pieces[i].length) {
            bufferEntry = i;
            textPosition = bufferIndex - pos;
            return true;
        }
        pos += pieces[i].length;
    }
    // If we reach here, bufferIndex == total length -> append at end
    bufferEntry = pieces.size() - 1;
    textPosition = pieces.back().length;
    return true;
}

std::string PieceTable::takeSubStringFromPiece(const Piece& piece, unsigned long localStart, unsigned long localLength)
{
    const std::string &buffer = (piece.type == ORIGINAL) ? original_buffer : added_buffer;
    // Absolute start in the underlying buffer
    unsigned long start = piece.start + localStart;
    if (localLength == 0 || start >= buffer.size()) return "";
    auto maxl= buffer.size() - start;
    auto length = std::min(localLength, maxl);
    return buffer.substr(start, length);
}

std::string PieceTable::getTextInBetween(const unsigned long startIndex, const unsigned long endIndex)
{
    unsigned long e1, p1;
    if (!indexPiece(startIndex, e1, p1)) return "";
    unsigned long e2, p2;
    if (!indexPiece(endIndex, e2, p2)) return "";

    if (e1 > e2) return "";
    if (e1 == e2 && p1 > p2) return "";

    // Same piece
    if (e1 == e2) return takeSubStringFromPiece(pieces[e1], p1, p2 - p1);

    // Multiple pieces
    std::string result;
    // First piece: from local p1 to end of that piece
    result += takeSubStringFromPiece(pieces[e1], p1, pieces[e1].length - p1);
    // Middle full pieces
    for (unsigned long e = e1+1; e < e2; e++)
        result += takeSubStringFromPiece(pieces[e], 0, pieces[e].length);
    // End piece: from piece start up to local p2
    result += takeSubStringFromPiece(pieces[e2], 0, p2);

    return result;
}

void PieceTable::splitPiece(Piece &piece1, Piece &piece2, unsigned long bufferPosition, Buffer bufferType)
{
    // Clamp bufferPosition to valid range
    if (bufferPosition > piece1.length) bufferPosition = piece1.length;
    piece2 = newPiece(piece1.start + bufferPosition, piece1.length - bufferPosition, bufferType);
    piece1.length = bufferPosition;
}

void PieceTable::insertPiece(unsigned long textIndex, const std::string &textBuffer, bool pushToUndoStack)
{
    // If buffer empty do nothing
    if (textBuffer.empty()) return;

    // Check if indexes are within bounds
    unsigned long length = 0;
    for (const auto &p : pieces) length += p.length;
    if (textIndex > length) textIndex = length;

    unsigned long entry;
    unsigned long position;
    if (!indexPiece(textIndex, entry, position)) return;

    Piece piece1 = pieces[entry];
    Piece piece2{};
    splitPiece(piece1, piece2, position, piece1.type);

    // Replace the old piece1 with the updated one
    pieces[entry] = piece1;

    // Add new text to added buffer
    auto l = textBuffer.length();
    Piece newpiece = newPiece(static_cast<int>(added_buffer.length()), l, ADDED);
    added_buffer += textBuffer;

    // piece1 -- newpiece -- piece2
    pieces.insert(pieces.begin() + entry + 1, newpiece);
    if (piece2.length > 0) pieces.insert(pieces.begin() + entry + 2, piece2);

    if (pushToUndoStack) {
        Operations op = {INSERT, textIndex, l, textBuffer};
        undo_stack.push(op);
        // New user action invalidates the redo stack
        while (!redo_stack.empty()) redo_stack.pop();
    }
}

void PieceTable::erasePiece(unsigned long startIndex, unsigned long endIndex, bool pushToUndoStack)
{
    // Ensure startIndex is less than or equal to endIndex
    if (startIndex > endIndex) return;

    // Identify the piece entries and positions for the start and end of the deletion
    unsigned long e1, p1;
    if (!indexPiece(startIndex, e1, p1)) return;
    unsigned long e2, p2;
    if (!indexPiece(endIndex-1, e2, p2)) return;
    p2 += 1;

    // Get text to be deleted
    std::string buffer = getTextInBetween(startIndex, endIndex);

    // First, split the starting piece at p1 and and adjust it
    Piece left = pieces[e1];
    Piece right{};

    // Check if we need to erase text from more than an entry
    if (e1 == e2) {
        // Split entry into 3 parts to delete middle
        Piece middle{};
        splitPiece(left, middle, p1, left.type);
        splitPiece(middle, right, p2 - p1, left.type);
        // Update the piece table
        pieces[e1] = left;
        if (right.length > 0) pieces.insert(pieces.begin() + e1 + 1, right);
    } else {
        // Split e1 entry to delete right piece
        splitPiece(left, right, p1, left.type);
        pieces[e1] = left;

        // Split e2 entry to delete last piece (left piece)
        Piece last = pieces[e2];
        Piece remainder{};
        splitPiece(last, remainder, p2, last.type);

        // Erase all the pieces in between
        pieces.erase(pieces.begin() + e1 + 1, pieces.begin() + e2 + 1);

        // If there's remaining text after the deletion in the last piece, insert it
        if (remainder.length > 0) pieces.insert(pieces.begin() + e1 + 1, remainder);
    }

    if (pushToUndoStack) {
        Operations op = {ERASE, startIndex, buffer.length(), buffer};
        undo_stack.push(op);
        // New user action invalidates the redo stack
        while (!redo_stack.empty()) redo_stack.pop();
    }
}

void PieceTable::replacePiece(unsigned long startIndex, unsigned long endIndex, const std::string &textBuffer)
{
    erasePiece(startIndex, endIndex);
    insertPiece(startIndex, textBuffer);
}

void PieceTable::undoPiece()
{
    if (undo_stack.empty()) return;

    auto op = undo_stack.top();
    undo_stack.pop();
    redo_stack.push(op);
    performUndo(op);
}

void PieceTable::redoPiece()
{
    if (redo_stack.empty()) return;

    auto op = redo_stack.top();
    redo_stack.pop();
    undo_stack.push(op);
    performRedo(op);
}

void PieceTable::performUndo(const Operations& operation) {
    if (operation.type == INSERT)
        erasePiece(operation.start, operation.start + operation.length, false);
    else if (operation.type == ERASE)
        insertPiece(operation.start, operation.buffer, false);
}

void PieceTable::performRedo(const Operations& operation) {
    if (operation.type == INSERT)
        insertPiece(operation.start, operation.buffer, false);
    else if (operation.type == ERASE)
        erasePiece(operation.start, operation.start + operation.length, false);
}
