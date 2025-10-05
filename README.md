# Piece Table
This is my C++ implementation of the [Piece Table](https://en.wikipedia.org/wiki/Piece_table) data structure, which is mainly used in text editors, as it offers easy undo/redo operations and fast text modifications. 

## Features
- Efficient insertions and deletions
- Undo and redo support  
- Lightweight and simple implementation  
- Works with large texts efficiently  
- Includes unit tests with **Catch2**

---

## Build & Run
This project uses a build script (build.sh) to automate building with CMake.

### Steps
1. Make the script executable
``` 
chmod +x build.sh
```
2. Run script to build project
```
./build.sh
```
3. Run main program
```
./build/main
```
4. Run unit tests
```
./build/unit_test
```

---

## How it works

The Piece Table stores text as a sequence of **pieces**, referencing either the **original buffer** or the **added buffer**.  
This allows modifications without rewriting the entire text.

Example:

- `[ORIGINAL:0,6]` → 6 characters from the original buffer starting at index 0  
- `[ADDED:0,2]` → 2 characters from the added buffer starting at index 0  

---

## Usage
The main program demonstrates basic operations with the Piece table
``` c++
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
  ```

---

## Testing
Unit tests are implemented using [Catch2](https://github.com/catchorg/Catch2), a modern C++ testing framework, which is simple and efficient. <br />

