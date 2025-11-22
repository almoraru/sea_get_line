/* ************************************************************************** */
/*                                                                            */
/*                        ______                                              */
/*                     .-"      "-.                                           */
/*                    /            \                                          */
/*        _          |              |          _                              */
/*       ( \         |,  .-.  .-.  ,|         / )                             */
/*        > "=._     | )(__/  \__)( |     _.=" <                              */
/*       (_/"=._"=._ |/     /\     \| _.="_.="\_)                             */
/*              "=._ (_     ^^     _)"_.="                                    */
/*                  "=\__|IIIIII|__/="                                        */
/*                 _.="| \IIIIII/ |"=._                                       */
/*       _     _.="_.="\          /"=._"=._     _                             */
/*      ( \_.="_.="     `--------`     "=._"=._/ )                            */
/*       > _.="                            "=._ <                             */
/*      (_/                                    \_)                            */
/*                                                                            */
/*      Filename: sea_get_line.c                                              */
/*      By: espadara <espadara@pirate.capn.gg>                                */
/*      Created: 2025/11/09 16:47:07 by espadara                              */
/*      Updated: 2025/11/22 09:20:27 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#include "sea_get_line.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/* TUNING:
   64KB is often the sweet spot for kernel pipe/read buffers.
   4KB is too small for high throughput on modern SSDs.
*/
#ifndef BUFFER_SIZE
# define BUFFER_SIZE 4096
#endif

#ifndef FD_MAX
# define FD_MAX 1024
#endif

/* External fast copy */
void    *sea_memcpy_fast(void *dest, const void *src, size_t n);

typedef struct s_stash {
    unsigned char   *buf;
    size_t          start;  // Index where valid data begins
    size_t          end;    // Index where valid data ends
    size_t          cap;    // Total capacity
} t_stash;

/* Cleans up the stash completely.
*/
static void sgl_nuke(t_stash *s)
{
    if (s->buf)
        free(s->buf);
    s->buf = NULL;
    s->start = 0;
    s->end = 0;
    s->cap = 0;
}

/*
   Extracts a line from the stash using the sliding window.
   Crucially: IT DOES NOT REALLOCATE THE STASH.
*/
static char *sgl_extract_window(t_stash *s, size_t nl_pos)
{
    char    *line;
    size_t  len;

    len = (nl_pos - s->start) + 1; // Length including \n

    // We only malloc the result line. We NEVER malloc the stash here.
    if (!(line = malloc(len + 1)))
        return (NULL);

    // Copy line from the window
    sea_memcpy_fast(line, s->buf + s->start, len);
    line[len] = '\0';

    // SLIDE THE WINDOW: Just advance the start index.
    // Zero copies. Zero reallocs.
    s->start += len;

    // Optimization: If stash is empty after extraction, reset indices to 0
    if (s->start == s->end)
    {
        s->start = 0;
        s->end = 0;
    }

    return (line);
}

/*
   Ensures space for reading.
   1. If there is space at the tail, do nothing.
   2. If buffer is full but has dead space at start, COMPACT it (memmove).
   3. If buffer is physically full, GROW it.
*/
static bool sgl_prepare_read(t_stash *s)
{
    unsigned char   *new_buf;
    size_t          data_len;
    size_t          new_cap;

    // Case 1: Enough space at the end? (We want to read at least BUFFER_SIZE/2 or 4k)
    // Adjust threshold as needed.
    if (s->cap - s->end >= BUFFER_SIZE)
        return (true);

    data_len = s->end - s->start;

    // Case 2: We have garbage space at the start. Compact!
    // Only compact if the garbage is significant (e.g. > 25% of cap)
    if (s->start > 0 && (s->cap - data_len) >= BUFFER_SIZE)
    {
        // Move valid data to 0
        memmove(s->buf, s->buf + s->start, data_len);
        s->start = 0;
        s->end = data_len;
        return (true);
    }

    // Case 3: Buffer is actually full. Grow.
    new_cap = (s->cap == 0) ? BUFFER_SIZE : s->cap * 2;
    // Safety check for overflow (optional but good practice)
    if (new_cap < s->cap) return (false);

    if (!(new_buf = malloc(new_cap)))
        return (false);

    if (data_len > 0)
        sea_memcpy_fast(new_buf, s->buf + s->start, data_len);

    free(s->buf);
    s->buf = new_buf;
    s->start = 0;
    s->end = data_len;
    s->cap = new_cap;
    return (true);
}

char *sea_get_line(int fd)
{
    static t_stash  st[FD_MAX];
    ssize_t         bytes_read;
    unsigned char   *nl_ptr;

    if (fd < 0 || fd >= FD_MAX || BUFFER_SIZE <= 0)
    {
        errno = EBADF;
        return (NULL);
    }

    while (true)
    {
        // 1. FAST SCAN: Use memchr (SIMD Optimized by libc)
        // We look from start to end.
        if (st[fd].end > st[fd].start)
        {
            // Calculate where to start searching.
            // In a perfect world, we track where we last searched,
            // but searching the whole active window is safer and usually fast enough.
            nl_ptr = memchr(st[fd].buf + st[fd].start, '\n', st[fd].end - st[fd].start);
            if (nl_ptr)
            {
                // Calculate absolute index of newline
                return (sgl_extract_window(&st[fd], (size_t)(nl_ptr - st[fd].buf)));
            }
        }

        // 2. Prepare Buffer (Compact or Grow)
        if (!sgl_prepare_read(&st[fd]))
        {
            sgl_nuke(&st[fd]);
            errno = ENOMEM;
            return (NULL);
        }

        // 3. READ (Append to end)
        bytes_read = read(fd, st[fd].buf + st[fd].end, st[fd].cap - st[fd].end);

        if (bytes_read < 0)
        {
            sgl_nuke(&st[fd]);
            return (NULL); // errno set by read
        }
        else if (bytes_read == 0)
        {
            if (st[fd].start == st[fd].end)
            {
                sgl_nuke(&st[fd]);
                return (NULL); // True EOF
            }
            // Return remainder
            return (sgl_extract_window(&st[fd], st[fd].end - 1));
        }

        st[fd].end += bytes_read;
    }
}
