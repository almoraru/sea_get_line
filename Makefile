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
#      Filename: Makefile                                                      #
#      By: espadara <espadara@pirate.capn.gg>                                  #
#      Created: 2025/11/02 14:13:51 by espadara                                #
#      Updated: 2025/11/09 17:08:28 by espadara                                #
#                                                                              #
# ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; #

CC = gcc
FLAGS = -Wall -Wextra -Werror -g -msse2

# --- Project Config ---
NAME		= sea_get_line.a
SRCS_PATH	= srcs/
OBJ_PATH	= objs/
SOURCES		= sea_get_line.c
OBJS		= $(addprefix $(OBJ_PATH), $(SOURCES:.c=.o))
VPATH		= $(SRCS_PATH)

# --- Library Config (sealib) ---
SEALIB_DIR	= sealib/
SEALIB_LIB	= $(addprefix $(SEALIB_DIR), sealib.a)
SEALIB_URL	= git@github.com:almoraru/sealib.git

# --- Includes ---
INC			= -I includes/ -I $(SEALIB_DIR)includes

# --- Test Program Config ---
TEST_NAME	= test_get_line
TEST_SRC	= test_main.c

# --- Main Rule ---
all: $(NAME)

# --- Build sea_get_line.a ---
# This library 'sea_get_line.a' depends on its objects.
# The objects, in turn, depend on sealib.a (see %.o rule below).
# This correctly forces sealib to build before this archive is made.
$(NAME): $(OBJS)
	@echo "Archiving $(NAME)..."
	@ar rc $(NAME) $(OBJS)
	@ranlib $(NAME)
	@echo "Library '$(NAME)' created. 🌊"

# --- Build Test Executable ---
# This rule now only needs to depend on 'all' (which is $(NAME)).
# The 'test' executable links all .a files, which are guaranteed
# to be built by the time this rule is run.
test: all
	@echo "Linking to create $(TEST_NAME)..."
	@$(CC) $(FLAGS) $(INC) $(TEST_SRC) $(NAME) $(SEALIB_LIB) -o $(TEST_NAME)
	@echo "Executable '$(TEST_NAME)' created. ✨ Run with ./$(TEST_NAME)"

# --- Build Sealib (from Git) ---
$(SEALIB_DIR):
	@if [ -d "$(SEALIB_DIR)/.git" ]; then \
		echo "Updating sealib in $(SEALIB_DIR)..."; \
		(cd $(SEALIB_DIR) && git pull); \
	else \
		echo "Cloning sealib into $(SEALIB_DIR)..."; \
		git clone $(SEALIB_URL) $(SEALIB_DIR); \
	fi

$(SEALIB_LIB): $(SEALIB_DIR)
	@echo "Building sealib..."
	@$(MAKE) -C $(SEALIB_DIR)

# --- Compile Object Files ---
#
# *** THIS IS THE CRITICAL FIX ***
#
# Each object file for sea_get_line (e.g., sea_get_line.o)
# now explicitly depends on $(SEALIB_LIB).
# Make will see this, find the rule for $(SEALIB_LIB),
# and run the git clone and 'make -C sealib' *before*
# it even tries to compile sea_get_line.c.
#
$(OBJ_PATH)%.o: %.c $(SEALIB_LIB) | $(OBJ_PATH)
	@echo "Compiling [sea_get_line]: $<"
	$(CC) $(FLAGS) $(INC) -c $< -o $@

$(OBJ_PATH):
	@mkdir -p $(OBJ_PATH)

# --- Clean Rules ---
clean:
	@if [ -d "$(SEALIB_DIR)" ]; then \
		$(MAKE) -C $(SEALIB_DIR) clean; \
	fi
	@/bin/rm -rf $(OBJ_PATH)
	@echo "[sea_get_line] object files removed."

fclean:
	@if [ -d "$(SEALIB_DIR)" ]; then \
		$(MAKE) -C $(SEALIB_DIR) fclean; \
	fi
	@/bin/rm -rf $(OBJ_PATH)
	@/bin/rm -f $(NAME)
	@echo "[sea_get_line] library '$(NAME)' removed."
	@/bin/rm -f $(TEST_NAME)
	@echo "[sea_get_line] executable '$(TEST_NAME)' removed."
	@/bin/rm -rf $(SEALIB_DIR)
	@echo "[sea_get_line] library 'sealib' directory removed."

re:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re test $(SEALIB_DIR)
