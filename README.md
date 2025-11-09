                       ______
                    .-"      "-.
                   /            \
       _          |              |          _
      ( \         |,  .-.  .-.  ,|         / )
       > "=._     | )(__/  \__)( |     _.=" <
      (_/"=._"=._ |/     /\     \| _.="_.="\_)
             "=._ (_     ^^     _)"_.="
                 "=\__|IIIIII|__/="
                _.="| \IIIIII/ |"=._
      _     _.="_.="\          /"=._"=._     _
     ( \_.="_.="     `--------`     "=._"=._/ )
      > _.="                            "=._ <
     (_/                                    \_)
     
🌊 SEA_GET_LINE 🏴‍☠️

A high-performance, binary-safe line reader for C!

"Why read a 'string' when ye can read the whole byte stream?"

📜 About This Treasure

sea_get_line be a swift and sturdy replacement for the common get_next_line, built to be a worthy companion to the sealib [cite: almoraru/sealib/sealib-de5a21485e2e83402aab7a8b5b4cdbc7a89146d9/README.MD] library. Crafted by Swittzy/espadara, this function charts a different course.

Instead of reading text "lines," sea_get_line reads byte streams up to a \n delimiter. It is binary-safe, meaning it handles embedded \0 null bytes without stopping, making it perfect for parsing binary protocols, network streams, or any file where \0 is valid data. It's built for speed, leveraging the fastest memory functions from sealib.

⚓ Features

✨ 100% Binary-Safe: Reads files with embedded \0 null bytes. It doesn't stop until it finds a \n or hits EOF.

🚀 High-Performance: Built on sealib's sea_memchr for fast newline detection and sea_memcpy_fast [cite: almoraru/sealib/sealib-de5a21485e2e83402aab7a8b5b4cdbc7a89146d9/srcs/sea_memcpy_fast.c] (SSE2-accelerated) for all memory-joining operations.

🎯 Robust & Clear API: Returns ssize_t to give you unambiguous success (> 0), EOF (0), or error (-1) codes. No more checking errno to see why NULL was returned!

🌊 sealib Dependency: Fully integrated with sealib [cite: almoraru/sealib/sealib-de5a21485e2e83402aab7a8b5b4cdbc7a89146d9/includes/sealib.h]. The Makefile fetches and builds it for you.

⚔️ Benchmark-Driven: Comes with a Python-powered test harness (test_main.c + gen_file.py) to verify performance and correctness from 1 line to 100MB files.

🗺️ Installation & Compilation

Prerequisites

gcc or clang compiler

make

git (for fetching sealib dependency)

python3 (for running the benchmark harness)

Setting Sail

Clone the repository:

git clone git@github.com:almoraru/sea_get_line.git
cd sea_get_line


Build the library:

make


This command will:
a. Automatically clone or pull the sealib dependency into a sealib/ directory.
b. Build sealib.a.
c. Build sea_get_line.a.

Build and run the benchmarks:

make test
./test_get_line


This will compile test_main.c and run the full benchmark suite, generating, reading, and verifying test files of various sizes.

Clean up object files:

make clean


Full cleanup (removes library, executables, and sealib dir):

make fclean


🏴‍☠️ Usage

Because sea_get_line is binary-safe, its API is different from get_next_line. It does not return a null-terminated string. Instead, it returns the size of the line and sets a void * pointer to the heap-allocated memory.

This means you are responsible for free()'ing the line.

#include "sea_get_line.h"
#include <fcntl.h>
#include <unistd.h> // for write()
#include <stdlib.h> // for free()
#include <stdio.h>  // for printf()

int main(void)
{p
    int      fd;
    void    *line_data;
    ssize_t  line_size;

    fd = open("my_file.txt", O_RDONLY);
    if (fd < 0)
        return (1);

    // Loop while sea_get_line returns a size > 0
    while ((line_size = sea_get_line(fd, &line_data)) > 0)
    {
        // We have a line! line_size includes the '\n'
        // We use write() because the data is not a C-string.
        printf("Read %zd bytes: ", line_size);
        write(STDOUT_FILENO, line_data, line_size);
        
        // CRITICAL: You must free the line when done.
        free(line_data);
        line_data = NULL;
    }

    // Check how the loop ended
    if (line_size == 0)
    {
        printf("\nEnd of file reached.\n");
    }
    else if (line_size == -1)
    {
        printf("\nAn error occurred while reading.\n");
    }

    close(fd);
    return (0);
}


📚 API Reference - The Captain's Manual

There is one function to rule them all:

/**
 * @brief Reads a 'line' (bytes ending in '\n') from a file
 * descriptor. This function is binary-safe and heap-allocates
 * the returned line.
 *
 * @param fd    The file descriptor to read from.
 * @param line  A pointer to a (void *) which will be set to the
 * address of a new, heap-allocated buffer
 * containing the line data.
 *
 * @return > 0  The number of bytes in the line (success).
 * @return 0    End of file reached. *line is set to NULL.
 * @return -1   An error occurred (read/malloc). *line is set to NULL.
 */
ssize_t sea_get_line(int fd, void **line);


🗂️ Project Structure

sea_get_line/
├── includes/
│   └── sea_get_line.h     # Main header file
├── srcs/
│   └── sea_get_line.c     # Core function logic
├── sealib/                # Cloned automatically by make
│   ├── includes/
│   ├── srcs/
│   └── ...
├── test_main.c            # Benchmark and verification harness
├── gen_file.py            # Python test file generator
├── Makefile               # Build configuration
└── README.md              # Ye be here!


🧭 Makefile Commands

Command

Description

make or make all

Build sea_get_line.a (and its sealib dependency).

make test

Build the library and create the benchmark executable.

make clean

Remove object files from sea_get_line and sealib.

make fclean

Remove all generated files, including executables and the sealib dir.

make re

Rebuild everything from scratch.

⚔️ Technical Details

Dependency: sealib (fetched and built via make).

Compilation Flags: -Wall -Wextra -Werror -g -msse2 (Requires SSE2 for sea_memcpy_fast).

Output: Static library sea_get_line.a.

🎯 Performance & Design

Binary Safety: The core design goal. By using sea_memchr [cite: almoraru/sealib/sealib-de5a21485e2e83402aab7a8b5b4cdbc7a89146d9/srcs/sea_memchr.c] instead of strchr and void* buffers instead of char* strings, the function never mistakes an embedded \0 for a string terminator.

SIMD Acceleration: All internal buffer joins (which happen when a line spans multiple reads) are performed by a helper that calls sea_memcpy_fast [cite: almoraru/sealib/sealib-de5a21485e2e83402aab7a8b5b4cdbc7a89146d9/srcs/sea_memcpy_fast.c] for copies >= 16 bytes.

Robust API: The ssize_t return value provides explicit status codes, eliminating the ambiguous NULL return that plagues get_next_line.

👥 Crew

Swittzy/espadara/almoraru - Captain of the Code Kraken

🤝 Contributing

This be a personal learning project, but feel free to explore the code, suggest improvements, or use it as inspiration for yer own voyages!

🌊 Fair Winds and Following Seas!

May yer reads be swift and yer buffers be binary-safe! ☕🏴‍☠️

     ~    ~   ~  ~     ~    ~ ~  ~      ~     ~  ~
  ~    ~  ~    Navigate the seas of C!   ~    ~   ~
~    ~      ~     ~    ~    ~    ~   ~      ~    ~


"In code we trust, in pirates we sail!" ⚓
