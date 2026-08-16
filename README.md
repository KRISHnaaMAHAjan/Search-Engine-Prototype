# My C++ Search Engine Project

This is a search engine I built for my systems programming class. It can search for text in big files using multiple threads to make it faster.

## How it works

The search tool uses a multi-stage pipeline to perform highly efficient searches across files:

1. **Tokenization**: Words in target files are tokenized by extracting alphanumeric sequences, optionally converting them to lowercase for case-insensitive searches.
2. **Per-file Bloom Filter Indexing**: During initialization, a Bloom Filter is built for each target file. All tokenized words from the file are added to its respective Bloom Filter.
3. **Query-time Check**: The search query is also tokenized. Before running the search on any file, the tool checks if all query tokens are possibly present in the file's Bloom Filter. If any token is definitely absent (meaning the Bloom Filter returns `false`), the file is skipped entirely, avoiding expensive disk I/O and Rabin-Karp searches.
4. **Candidate Dispatch**: Files that pass the Bloom Filter check are dispatched to the search engine.
5. **Rabin-Karp Search with Multi-threading**: The dispatched files are searched using the Rabin-Karp rolling hash algorithm. The file is split into chunks, and each chunk is searched in parallel using a thread pool with overlapping boundaries to ensure no matches are missed.

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
