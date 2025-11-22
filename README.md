
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

# 🌊 SEA_GET_LINE 🏴‍☠️

**A blazing fast, modern C line reader!**

> *"Why reallocate memory when ye can slide through the window?"*

## 📜 About This Treasure

**sea_get_line** is a highly optimized replacement for the traditional `get_next_line` or `fgets`, built to be a worthy companion to the **sealib** library. 

While most line readers suffer from constant memory allocation and copying (O(N) or O(N²)), **sea_get_line** utilizes a **Sliding Window Buffer** strategy. It keeps memory alive and simply slides a window over the byte stream, resulting in near O(1) overhead for buffer management.

It aligns with modern C standards: it returns a standard `char *` string and utilizes `errno` for error handling, making it robust and easy to integrate.

## ⚓ Features

- 🚀 **Sliding Window Architecture** - Zero-copy buffer management for the vast majority of reads. No more `malloc`/`free` churn on every line!
- 📈 **Geometric Growth** - Buffer capacity doubles automatically when full, preventing performance degradation on massive lines.
- ✨ **Binary Safe Logic** - Internally handles embedded `\0` null bytes without stopping (reads until `\n` or EOF).
- ⚡ **SIMD Accelerated** - Uses **sealib**'s `sea_memcpy_fast` for data movement.
- 🛠️ **Standard C API** - Returns a `char *` and sets `errno`. No more ambiguous return values.
- ⚔️ **Benchmark-Driven** - Proven to handle 100MB+ files in ~1 second on modern hardware.

## 🗺️ Installation & Compilation

### Prerequisites
- `gcc` or `clang` compiler
- `make`
- `git` (for fetching sealib dependency)

### Setting Sail

1. **Clone the repository:**
```bash
git clone git@github.com:almoraru/sea_get_line.git
cd sea_get_line
````

2.  **Build the library:**

<!-- end list -->

```bash
make
```

This will:

  - Automatically clone or pull the `sealib` dependency.
  - Build `sealib.a`.
  - Create `sea_get_line.a` - yer treasure chest of functions\!

<!-- end list -->

3.  **Run Benchmarks (Optional):**

<!-- end list -->

```bash
make test
./test_get_line
```

## 🏴‍☠️ Usage

The API is designed to be simple and familiar to standard C users. It returns a heap-allocated string that you must free. It returns `NULL` on both **EOF** and **Error**. To distinguish between them, check `errno`.

### Example Usage

```c
#include "sea_get_line.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h> // Essential for error checking

int main(void)
{
    int   fd;
    char  *line;

    fd = open("my_file.txt", O_RDONLY);
    if (fd < 0)
        return (1);

    // 1. Reset errno before the loop
    errno = 0;

    // 2. Loop while a line is returned
    while ((line = sea_get_line(fd)) != NULL)
    {
        printf("%s", line); // line includes the \n
        
        // CRITICAL: You must free the line!
        free(line);
        
        // Reset errno for the next iteration safety
        errno = 0;
    }

    // 3. Check why the loop stopped
    if (errno != 0)
    {
        perror("Error reading file");
    }
    else
    {
        printf("End of file reached successfully.\n");
    }

    close(fd);
    return (0);
}
```

## 📚 API Reference - The Captain's Manual

```c
/**
 * @brief Reads a line from a file descriptor.
 *
 * Uses a sliding window buffer to minimize memory allocations.
 * The returned string is null-terminated and includes the newline 
 * (unless EOF was reached without one).
 *
 * @param fd  The file descriptor to read from.
 * @return    Pointer to the heap-allocated string.
 * Returns NULL on EOF or Error.
 * Check errno to distinguish (errno == 0 means EOF).
 */
char *sea_get_line(int fd);
```

## ⚙️ Tuning

You can adjust the buffer size at compile time to match your storage media (SSD/HDD) for maximum throughput.

```c
// For modern NVMe SSDs, larger buffers reduce syscall overhead:
#define BUFFER_SIZE 65536  // 64KB
```

## 🗂️ Project Structure

```
sea_get_line/
├── includes/
│   └── sea_get_line.h     # Main header
├── srcs/
│   └── sea_get_line.c     # The optimized sliding window logic
├── sealib/                # SIMD optimized helpers
├── test_main.c            # Performance harness
├── Makefile               # Build configuration
└── README.md              # Ye be here!
```

## ⚔️ Technical Details

  - **Algorithm**: Sliding Window with Lazy Compaction.
  - **Complexity**: Amortized O(1) for buffer management.
  - **Memory**: Heap-allocated static buffer per FD.
  - **Compilation Flags**: `-Wall -Wextra -Werror -msse2` (Required for SSE optimizations).

## 👥 Crew

  - **Swittzy/espadara/almoraru** - *Captain of the Code Kraken*

## 🤝 Contributing

This be a personal learning project, but feel free to explore the code, suggest improvements, or use it as inspiration for yer own voyages\!

## 📝 License

This project sails under the MIT License. Use it wisely, share it freely\!

## 🌊 Fair Winds and Following Seas\!

May yer reads be swift and yer syscalls be few\! ☕🏴‍☠️

```
     ~    ~   ~  ~     ~    ~ ~  ~      ~     ~  ~
  ~    ~  ~    Navigate the seas of C!   ~    ~   ~
~    ~      ~     ~    ~    ~    ~   ~      ~    ~
```


