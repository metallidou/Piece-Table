#include <string>
#include "PieceTable.h"

PieceTable::PieceTable(const std::string &textBuffer)
{
    Piece newpiece = newPiece(0, static_cast<int>(textBuffer.length()), ORIGINAL);
    this->piece.push_back(newpiece);
    this->original_buffer = textBuffer;
    this->added_buffer = "";

    Operations op = {INSERT, 0, textBuffer.length(), textBuffer};
    undo_stack.push(op);
}

PieceTable::~PieceTable() = default;

std::string PieceTable::getText()
{
    std::string text;
    for (const auto &p : this->piece) text += this->getBufferText(p);
    return text;
}

std::string PieceTable::getBufferText(const Piece &piece) const
{
    const std::string& buffer = (piece.type == ORIGINAL) ? this->original_buffer : this->added_buffer;
    if (piece.start >= buffer.size() || piece.length == 0) return "";
    return buffer.substr(piece.start, std::min(piece.length, buffer.size() - piece.start));
}

std::string PieceTable::takeSubStringFromPiece(const Piece& piece, unsigned long localStart, unsigned long localLength)
{
    const std::string &buffer = (piece.type == ORIGINAL) ? this->original_buffer : this->added_buffer;
    // Absolute start in the underlying buffer
    unsigned long absStart = piece.start + localStart;
    if (localLength == 0 || absStart >= buffer.size()) return "";
    auto maxLen = buffer.size() - absStart;
    auto useLen = std::min(localLength, maxLen);
    return buffer.substr(absStart, useLen);
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
    if (e1 == e2) return takeSubStringFromPiece(this->piece[e1], p1, p2 - p1);

    // Multiple pieces
    std::string result;
    // First piece: from local p1 to end of that piece
    result += takeSubStringFromPiece(this->piece[e1], p1, this->piece[e1].length - p1);
    // Middle full pieces
    for (unsigned long e = e1 + 1; e < e2; e++)
        result += takeSubStringFromPiece(this->piece[e], 0, this->piece[e].length);
    // End piece: from piece start up to local p2
    result += takeSubStringFromPiece(this->piece[e2], 0, p2);

    return result;
}


Piece PieceTable::newPiece(const unsigned long startIndex, const unsigned long bufferLength, Buffer bufferType)
{
    Piece newpiece{};
    newpiece.start = startIndex;
    newpiece.length = bufferLength;
    newpiece.type = bufferType;
    return newpiece;
}

bool PieceTable::indexPiece(unsigned long bufferIndex, unsigned long &bufferEntry, unsigned long &textPosition) const
{
    if (this->piece.empty()) return false;

    unsigned long pos = 0;
    for (size_t i = 0; i < this->piece.size(); i++) {
        if (bufferIndex < pos + piece[i].length) {
            bufferEntry = i;
            textPosition = bufferIndex - pos;
            return true;
        }
        pos += piece[i].length;
    }
    // If we reach here, bufferIndex == total length -> append at end
    bufferEntry = piece.size() - 1;
    textPosition = piece.back().length;
    return true;
}

unsigned long PieceTable::indexText(unsigned long bufferEntry, unsigned long textPosition) const
{
    unsigned long index = 0;
    unsigned long e = 0;

    while(e < bufferEntry) {
        index += this->piece[e++].length;
    }
    return index + textPosition;
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
    for (const auto &p : this->piece) length += p.length;
    if (textIndex > length) textIndex = length;

    unsigned long entry;
    unsigned long position;
    if (!this->indexPiece(textIndex, entry, position)) return;

    Piece piece1 = this->piece[entry];
    Piece piece2{};
    PieceTable::splitPiece(piece1, piece2, position, piece1.type);

    // Replace the old piece1 with the updated one
    this->piece[entry] = piece1;

    // Add new text to added buffer
    auto l = textBuffer.length();
    Piece newpiece = PieceTable::newPiece(static_cast<int>(this->added_buffer.length()), l, ADDED);
    this->added_buffer += textBuffer;

    // piece1 -- newpiece -- piece2
    this->piece.insert(this->piece.begin() + entry + 1, newpiece);
    if (piece2.length > 0) this->piece.insert(this->piece.begin() + entry + 2, piece2);

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
    if (!this->indexPiece(startIndex, e1, p1)) return;
    unsigned long e2, p2;
    if (!this->indexPiece(endIndex-1, e2, p2)) return;
    p2 += 1;

    // Get text to be deleted
    std::string buffer = this->getTextInBetween(startIndex, endIndex);

    // First, split the starting piece at p1 and and adjust it
    Piece left = this->piece[e1];
    Piece right{};

    // Check if we need to erase text from more than an entry
    if (e1 == e2) {
        // Split entry into 3 parts to delete middle
        Piece middle{};
        PieceTable::splitPiece(left, middle, p1, left.type);
        PieceTable::splitPiece(middle, right, p2 - p1, left.type);
        // Update the piece table
        this->piece[e1] = left;
        if (right.length > 0) this->piece.insert(this->piece.begin() + e1 + 1, right);
    } else {
        // Split e1 entry to delete right piece
        PieceTable::splitPiece(left, right, p1, left.type);
        this->piece[e1] = left;

        // Split e2 entry to delete last piece (left piece)
        Piece last = this->piece[e2];
        Piece remainder{};
        PieceTable::splitPiece(last, remainder, p2, last.type);

        // Erase all the pieces in between
        this->piece.erase(this->piece.begin() + e1 + 1, this->piece.begin() + e2 + 1);

        // If there's remaining text after the deletion in the last piece, insert it
        if (remainder.length > 0) this->piece.insert(this->piece.begin() + e1 + 1, remainder);
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
    auto op = undo_stack.top();
    undo_stack.pop();
    redo_stack.push(op);
    this->performUndo(op);
}

void PieceTable::redoPiece()
{
    auto op = redo_stack.top();
    redo_stack.pop();
    undo_stack.push(op);
    this->performRedo(op);
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
