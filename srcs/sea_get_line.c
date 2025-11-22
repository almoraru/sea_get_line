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
/*      Updated: 2025/11/22 10:37:44 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#include "sea_get_line.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

typedef struct s_stash {
    unsigned char   *buf;
    size_t          start;
    size_t          end;
    size_t          cap;
} t_stash;

static void sgl_nuke(t_stash *s)
{
    if (s->buf)
        free(s->buf);
    s->buf = NULL;
    s->start = 0;
    s->end = 0;
    s->cap = 0;
}

static char *sgl_extract_window(t_stash *s, size_t nl_pos)
{
    char    *line;
    size_t  len;

    len = (nl_pos - s->start) + 1;

    if (!(line = malloc(len + 1)))
        return (NULL);

    sea_memcpy_fast(line, s->buf + s->start, len);
    line[len] = '\0';

    s->start += len;

    if (s->start == s->end)
    {
        s->start = 0;
        s->end = 0;
    }

    return (line);
}

static bool sgl_prepare_read(t_stash *s)
{
    unsigned char   *new_buf;
    size_t          data_len;
    size_t          new_cap;

    if (s->cap - s->end >= BUFFER_SIZE)
        return (true);

    data_len = s->end - s->start;

    if (s->start > 0 && (s->cap - data_len) >= BUFFER_SIZE)
    {
        memmove(s->buf, s->buf + s->start, data_len);
        s->start = 0;
        s->end = data_len;
        return (true);
    }

    new_cap = (s->cap == 0) ? BUFFER_SIZE : s->cap * 2;
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
        if (st[fd].end > st[fd].start)
        {
            nl_ptr = memchr(st[fd].buf + st[fd].start, '\n', st[fd].end - st[fd].start);
            if (nl_ptr)
            {

                return (sgl_extract_window(&st[fd], (size_t)(nl_ptr - st[fd].buf)));
            }
        }

        if (!sgl_prepare_read(&st[fd]))
        {
            sgl_nuke(&st[fd]);
            errno = ENOMEM;
            return (NULL);
        }


        bytes_read = read(fd, st[fd].buf + st[fd].end, st[fd].cap - st[fd].end);

        if (bytes_read < 0)
        {
            sgl_nuke(&st[fd]);
            return (NULL);
        }
        else if (bytes_read == 0)
        {
            if (st[fd].start == st[fd].end)
            {
                sgl_nuke(&st[fd]);
                return (NULL);
            }
            return (sgl_extract_window(&st[fd], st[fd].end - 1));
        }

        st[fd].end += bytes_read;
    }
}
