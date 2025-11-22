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
/*      Filename: test_main.c                                                 */
/*      By: espadara <espadara@pirate.capn.gg>                                */
/*      Created: 2025/11/09 17:05:23 by espadara                              */
/*      Updated: 2025/11/22 08:59:01 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#include "sea_get_line.h"
#include "sealib.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define TEST_FILE "temp_test_file.txt"
#define OUTPUT_FILE "temp_output_file.txt"
#define PYTHON_CMD "python3 gen_file.py"

typedef struct {
	const char	*name;
	int			line_count;
	int			line_length;
}	t_test_case;

/**
 * @brief Runs a single test case.
 * 1. Generates the file using the Python script.
 * 2. Opens the test file and a new output file.
 * 3. Times sea_get_line reading the test file and writing to output.
 * 4. Verifies the output file matches the test file.
 * 5. Cleans up.
 * @return 0 on PASS, 1 on FAIL.
 */
int	run_test_case(t_test_case *test)
{
	char	cmd_buf[256];
	int		in_fd, out_fd;
	char	*line;
	clock_t	start, end;
	double	cpu_time_used;
	int		ret_status = 0; // 0 = PASS
	errno = 0;

	printf("--- Test: %s (%d lines of %d bytes) ---\n",
		test->name, test->line_count, test->line_length);

	// 1. Generate the test file
	sprintf(cmd_buf, "%s %s %d %d",
		PYTHON_CMD, TEST_FILE, test->line_count, test->line_length);
	if (system(cmd_buf) != 0)
	{
		fprintf(stderr, "Error: Python script failed to run. Is python3 in your PATH?\n");
		return (1);
	}

	// 2. Open files
	in_fd = open(TEST_FILE, O_RDONLY);
	out_fd = open(OUTPUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (in_fd < 0 || out_fd < 0)
	{
		perror("Error opening test files");
		return (1);
	}

	// 3. Run and time the function
	printf("Running sea_get_line...\n");
	start = clock();
	while ((line = sea_get_line(in_fd)))
	{
		// Write the buffer to the output file to verify its contents
		write(out_fd, line, sea_strlen(line));
		free(line);
		line = NULL;
	}
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

	close(in_fd);
	close(out_fd);

	if (errno != 0)
	{
		fprintf(stderr, "Error: sea_get_line returned -1\n");
		ret_status = 1; // FAIL
		goto cleanup; // Go to cleanup
	}

	printf("Time taken: %f seconds\n", cpu_time_used);

	// 4. Verify the output
	printf("Verifying output... ");
	// Use diff to check if the generated file and the output file are identical
	if (system("diff -q " TEST_FILE " " OUTPUT_FILE) == 0)
	{
		printf("Result: \033[32mPASS\033[0m\n");
	}
	else
	{
		printf("Result: \033[31mFAIL\033[0m (Output does not match input)\n");
		ret_status = 1; // FAIL
	}

// 5. Cleanup
cleanup:
	printf("Cleaning up files...\n");
	remove(TEST_FILE);
	remove(OUTPUT_FILE);

	printf("\n");
	return (ret_status);
}

int	main(void)
{
  t_test_case tests[] = {
      {"1. Single Line", 1, 80},
        {"2. 20 Lines", 20, 80},
        {"3. 100 Lines", 100, 80},
        {"4. 1024 Lines", 1024, 80},
        {"5. 4096 Lines (Small file)", 4096, 80},
        {"6. ~1MB File (13107 lines @ 80B)", 13107, 80},
        {"7. ~25MB File (327680 lines @ 80B)", 327680, 80},
        {"8. Single 1MB Line", 1, 1024 * 1024},
        {"9. ~100MB File (1310720 lines @ 80B)", 1310720, 80}, // Uncomment for full power
        {NULL, 0, 0} // Sentinel to mark the end
  };

	int i = 0;
	int failures = 0;

	while (tests[i].name != NULL)
	{
		failures += run_test_case(&tests[i]);
		i++;
	}

	printf("===================================\n");
	printf("Benchmark Complete. %d test(s) failed.\n", failures);
	printf("===================================\n");

	return (failures > 0 ? 1 : 0);
}
