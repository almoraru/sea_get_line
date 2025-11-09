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
/*      Filename: sea_get_line.h                                              */
/*      By: espadara <espadara@pirate.capn.gg>                                */
/*      Created: 2025/11/09 16:40:44 by espadara                              */
/*      Updated: 2025/11/09 16:41:07 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#ifndef SEA_GET_LINE_H
# define SEA_GET_LINE_H

# include "sealib.h"

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif

# ifndef FD_MAX
#  define FD_MAX 1024
# endif

/**
 * @note The caller is responsible for free()'ing the buffer
 * pointed to by *line.
 */
ssize_t	sea_get_line(int fd, void **line);

#endif
