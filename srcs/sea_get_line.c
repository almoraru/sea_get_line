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
/*      Updated: 2025/11/09 16:57:03 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#include "sea_get_line.h"

typedef struct s_buffer {
	void	*data;
	size_t	size;
}	t_buffer;

static void	*sgl_memcpy(void *dest, const void *src, size_t n)
{
  if (n >= 16)
    return (sea_memcpy_fast(dest, src, n));
  else
    return (sea_memcpy(dest, src, n));
}

/**
 * @brief Frees the buffer data and resets the struct.
 */
static void	sgl_free_buf(t_buffer *buf)
{
  if (buf->data)
	{
      free(buf->data);
      buf->data = NULL;
	}
  buf->size = 0;
}

/**
 * @return 1 on success, -1 on malloc error.
 */
static int	sgl_join_mem(t_buffer *leftover, void *read_buf,
						ssize_t read_size)
{
  void	*new_data;

  if (leftover->data == NULL)
    {
      new_data = malloc(read_size);
      if (!new_data)
        return (-1);
      sgl_memcpy(new_data, read_buf, read_size);
      leftover->data = new_data;
      leftover->size = read_size;
      return (1);
	}

  new_data = malloc(leftover->size + read_size);
  if (!new_data)
    return (-1);
  sgl_memcpy(new_data, leftover->data, leftover->size);
  sgl_memcpy(new_data + leftover->size, read_buf, read_size);
  free(leftover->data);
  leftover->data = new_data;
  leftover->size += read_size;
  return (1);
}

/**
 * @brief Extracts a line from the buffer, updates the
 * leftover, and sets the *line pointer.
 * @return The length of the extracted line, or -1 on malloc error.
 */
static ssize_t	sgl_extract_line(t_buffer *buf, void **line,
                                 void *nl_ptr)
{
  ssize_t	line_len;
  size_t	new_leftover_size;
  void	*new_leftover_data;

  line_len = (nl_ptr - buf->data) + 1;
  *line = malloc(line_len);
  if (!*line)
    return (-1);

  sgl_memcpy(*line, buf->data, line_len);
  new_leftover_size = buf->size - line_len;
  if (new_leftover_size > 0)
    {
      new_leftover_data = malloc(new_leftover_size);
      if (!new_leftover_data)
          {
            free(*line);
            return (-1);
          }
      sgl_memcpy(new_leftover_data, nl_ptr + 1, new_leftover_size);
	}
  else
		new_leftover_data = NULL;
  free(buf->data);
  buf->data = new_leftover_data;
  buf->size = new_leftover_size;
  return (line_len);
}


ssize_t	sea_get_line(int fd, void **line)
{
  static t_buffer	leftover[FD_MAX];
  ssize_t			bytes_read;
  char			read_buf[BUFFER_SIZE];
  void			*nl_ptr;

  if (fd < 0 || fd >= FD_MAX || !line || BUFFER_SIZE <= 0)
    return (-1);
  *line = NULL;

  nl_ptr = sea_memchr(leftover[fd].data, '\n', leftover[fd].size);
    if (nl_ptr)
      return (sgl_extract_line(&leftover[fd], line, nl_ptr));

    while ((bytes_read = read(fd, read_buf, BUFFER_SIZE)) > 0)
      {
        if (sgl_join_mem(&leftover[fd], read_buf, bytes_read) == -1)
          {
            sgl_free_buf(&leftover[fd]);
            return (-1);
          }
        nl_ptr = sea_memchr(leftover[fd].data, '\n', leftover[fd].size);
        if (nl_ptr)
          return (sgl_extract_line(&leftover[fd], line, nl_ptr));
      }
    if (bytes_read < 0)
      {
        sgl_free_buf(&leftover[fd]);
        return (-1);
      }
    if (leftover[fd].size > 0)
      {
        *line = leftover[fd].data;
        ssize_t final_size = leftover[fd].size;
        leftover[fd].data = NULL;
        leftover[fd].size = 0;
        return (final_size);
      }

	sgl_free_buf(&leftover[fd]);
	return (0);
}
