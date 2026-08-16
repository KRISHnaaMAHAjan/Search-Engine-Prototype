# My C++ Search Engine Project

This is a search engine I built for my systems programming class. It can search for text in big files using multiple threads to make it faster.

## How it works

1. **Rabin-Karp Algorithm**: I used the Rabin-Karp rolling hash algorithm. It's cool because it doesn't have to check every single character unless the hashes match. This makes it way faster than a normal nested loop search.
2. **Multi-threading**: I used `std::thread` to split the file into chunks. Each thread searches its own chunk, and then we combine the results. I added some overlap between chunks so we don't miss words that are split in half.
3. **C++**: I used C++17 features like `std::filesystem` to list files in a directory.

## How to build it (PowerShell)

I used CMake for this project. To build it on Windows:

```powershell
# Create a build directory
mkdir build
cd build

# Configure the project
cmake ..

# Build the project (this works for both Visual Studio and MinGW)
cmake --build . --config Release
```

## How to use it

Run the program from the PowerShell terminal:

```powershell
# Search a specific file
.\Debug\searchtool.exe "C:\path\to\logs\server.log" "ERROR"

# Search a whole folder
.\Debug\searchtool.exe "C:\path\to\logs" "timeout"
```

*Note: If you built it in Release mode, it might be in the `\Release` folder instead of `\Debug`.*

## Future Ideas
- Add a way to ignore case (uppercase/lowercase).
- Make the output prettier with colors.
- Support searching with regular expressions.

Hope you like my project!
