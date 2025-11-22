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
/*      Updated: 2025/11/22 08:54:36 by espadara                              */
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

char	*sea_get_line(int fd);

#endif
