/******************************************************************************
 * clide - command line interface document editor
 * Copyright (c) 2017-2025 Niklas Benfer
 * Licensed under the terms of the MIT License.
 *****************************************************************************/

#include "clide.h"

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);
    initialize_clide();
    open_document_editor();
    while (handle_input(getch()));
    close_document_editor();
    quit_clide();
    return EXIT_SUCCESS;
}