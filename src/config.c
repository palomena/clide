#include "clide.h"

static const char program_help[] = {
	"CLIde - command line interface document editor\n"
	"Usage: %s [OPTIONS]... [FILE]\n"
	"Options:\n"
	"\tA value is expected if the option is marked with an asterisk (*).\n"
	"\t-h       Print program help string\n"
	"\t-v       Print program version string\n"
    "\t-c   *   Use a color theme\n"
	"\t-s   *   Stream file contents on demand\n"
	"\t-t   *   Override the tabsize (default: 8)\n"
	"Default keymap:\n"
	"\tCtrl+A :  Select everything\n"
	"\tCtrl+C :  Copy selection\n"
	"\tCtrl+F :  Search text\n"
	"\tCtrl+G :  Goto line number, column number\n"
	"\tCtrl+H :  Display help window\n"
	"\tCtrl+I :  Display info window\n"
	"\tCtrl+L :  Select current line\n"
	"\tCtrl+O :  Open a new file\n"
	"\tCtrl+Q :  Quit editor\n"
	"\tCtrl+R :  Replace text\n"
	"\tCtrl+S :  Save file to disk\n"
	"\tCtrl+V :  Paste clipboard contents\n"
	"\tCtrl+X :  Cut selection\n"
	"\tCtrl+Y :  Redo last action\n"
	"\tCtrl+Z :  Undo last action\n"
    "Color themes:\n"
    "\tdark  : black background, white foreground\n"
    "\tlight : white background, black foreground\n"
};

static void print_program_help(const char *program_name) {
	printf(program_help, program_name);
}

static const char program_version[] = {
	"clide, version 0.0.1, %s\n"
	"Copyright (c) 2017-2025, Niklas Benfer\n"
};

static void print_program_version(void) {
	printf(program_version, curses_version());
}

struct Config config = {
    .input = "New Document",
    .stream_file_contents = false,
    .tabsize = 8,
    .theme = "dark"
};

void parse_arguments(int argc, char *argv[]) {
	static const char options[] = "c:hst:v";
	for (;;) {
		switch (getopt(argc, argv, options)) {
			case -1:
                for (int i = optind; i < argc; i++) {
                    config.input = argv[i];
                }
				return;
			default:
				fprintf(stderr, "Invalid option: %c!\n", optopt);
				exit(EXIT_FAILURE);
			case 'h':
				print_program_help(argv[0]);
				exit(EXIT_SUCCESS);
			case 'v':
				print_program_version();
				exit(EXIT_SUCCESS);
            case 'c':
                config.theme = optarg;
                break;
			case 's':
				config.stream_file_contents = true;
				break;
			case 't':
				config.tabsize = strtol(optarg, NULL, 10);
				set_tabsize(config.tabsize);
				break;
		}
	}
}