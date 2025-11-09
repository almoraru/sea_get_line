# ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; #
#                                                                              #
#                        ______                                                #
#                     .-"      "-.                                             #
#                    /            \                                            #
#        _          |              |          _                                #
#       ( \         |,  .-.  .-.  ,|         / )                               #
#        > "=._     | )(__/  \__)( |     _.=" <                                #
#       (_/"=._"=._ |/     /\     \| _.="_.="\_)                               #
#              "=._ (_     ^^     _)"_.="                                      #
#                  "=\__|IIIIII|__/="                                          #
#                 _.="| \IIIIII/ |"=._                                         #
#       _     _.="_.="\          /"=._"=._     _                               #
#      ( \_.="_.="     `--------`     "=._"=._/ )                              #
#       > _.="                            "=._ <                               #
#      (_/                                    \_)                              #
#                                                                              #
#      Filename: gen_file.py                                                   #
#      By: espadara <espadara@pirate.capn.gg>                                  #
#      Created: 2025/11/09 17:05:07 by espadara                                #
#      Updated: 2025/11/09 17:05:07 by espadara                                #
#                                                                              #
# ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; #

#!/usr/bin/env python3

"""
Generates a test file with a specified number of lines and length.
Usage: python3 gen_file.py <filename> <line_count> <line_length>
"""

import sys
import os

def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <filename> <line_count> <line_length>", file=sys.stderr)
        sys.exit(1)

    filename = sys.argv[1]
    try:
        line_count = int(sys.argv[2])
        line_length = int(sys.argv[3])
    except ValueError:
        print("Error: line_count and line_length must be integers.", file=sys.stderr)
        sys.exit(1)

    if line_length < 2:
        print("Error: line_length must be at least 2 (for content + newline).", file=sys.stderr)
        sys.exit(1)

    base_line = 'a' * (line_length - 1) + '\n'
    total_bytes = 0

    try:
        with open(filename, 'w') as f:
            for i in range(line_count):
                # Write a slightly varying line
                line_to_write = f"Line{i}:".ljust(line_length - 1, 'x') + '\n'

                # For very large files, create a simpler, faster line
                if line_count > 10000:
                    line_to_write = base_line

                f.write(line_to_write)
                total_bytes += len(line_to_write)

        mb = total_bytes / (1024 * 1024)
        print(f"[Python] Generated '{filename}': {line_count} lines, {total_bytes} bytes (~{mb:.2f} MB)")

    except Exception as e:
        print(f"Error writing to {filename}: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
