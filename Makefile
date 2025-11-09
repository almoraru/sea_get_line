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
#      Updated: 2025/11/02 15:34:44 by espadara                                #
#                                                                              #
# ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; #

CC = gcc
FLAGS = -Wall -Wextra -Werror -g

# --- Project Config ---
NAME		= libseaprintf.a
SRCS_PATH	= srcs/
OBJ_PATH	= objs/
SOURCES		= sea_printf.c sea_printf_handlers.c sea_printf_arena.c			\
				sea_printf_bonus.c
OBJS		= $(addprefix $(OBJ_PATH), $(SOURCES:.c=.o))
VPATH		= $(SRCS_PATH)

# --- Library Config (sealib) ---
SEALIB_DIR	= sealib/
SEALIB_LIB	= $(addprefix $(SEALIB_DIR), sealib.a)
SEALIB_URL	= git@github.com:almoraru/sealib.git

# --- Includes ---
INC			= -I includes/ -I $(SEALIB_DIR)includes

# --- Test Program Config ---
TEST_NAME	= test_seaprintf
TEST_SRC	= test_main.c

# --- Main Rule ---
all: $(NAME)

# --- Build libseaprintf.a ---
$(NAME): $(OBJS) $(SEALIB_LIB)
	@echo "Archiving $(NAME)..."
	@ar rc $(NAME) $(OBJS)
	@ranlib $(NAME)
	@echo "Library '$(NAME)' created. 🌊"

# --- Build Test Executable ---
test: all
	@echo "Linking to create $(TEST_NAME)..."
	@$(CC) $(FLAGS) $(INC) $(TEST_SRC) $(NAME) $(SEALIB_LIB) -o $(TEST_NAME) -lm
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
$(OBJ_PATH)%.o: %.c $(SEALIB_LIB) | $(OBJ_PATH)
	@echo "Compiling [sea_printf]: $<"
	$(CC) $(FLAGS) $(INC) -c $< -o $@

$(OBJ_PATH):
	@mkdir -p $(OBJ_PATH)

# --- Clean Rules ---
clean:
	@if [ -d "$(SEALIB_DIR)" ]; then \
		$(MAKE) -C $(SEALIB_DIR) clean; \
	fi
	@/bin/rm -rf $(OBJ_PATH)
	@echo "[sea_printf] object files removed."

fclean:
	@if [ -d "$(SEALIB_DIR)" ]; then \
		$(MAKE) -C $(SEALIB_DIR) fclean; \
	fi
	@/bin/rm -rf $(OBJ_PATH)
	@/bin/rm -f $(NAME)
	@echo "[sea_printf] library '$(NAME)' removed."
	@/bin/rm -f $(TEST_NAME)
	@echo "[sea_printf] executable '$(TEST_NAME)' removed."
	@/bin/rm -rf $(SEALIB_DIR)
	@echo "[sea_printf] library 'sealib' directory removed."

re:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re test $(SEALIB_DIR)
