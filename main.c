#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#define VERSION \
    "tsh version 1.0.0\n"

#define USAGE \
    "Usage: tsh [GNU long option] [option]... \n \
      tsh [GNU long option] [option] script-file ...\n \
      long option:\n \
          --version\n \
          --help\n"

static struct option longoptions [] = {
    {"version", 0, NULL, 'v'},
    {"help", 0, NULL, 'h'}
};

void usage(void);
void version(void);

int main(int argc, char *argv[]) {
    int opt = 0;
    if (argc < 2) {
        usage();
    }

    while ((opt = getopt_long(argc, argv, "vh", longoptions, NULL)) != EOF) {
        switch (opt) {
            case 'h':
                usage();
                break;
            case 'v':
                version();
                break;
            default:
                usage();
                break;
        }
    }
    
    return 0;
}

void usage(void) {
    fprintf(stdout, USAGE);
}

void version(void) {
    fprintf(stdout, VERSION);
}
