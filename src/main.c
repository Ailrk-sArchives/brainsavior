#include "brainsalvation.h"

int main(int argc, char** argv) {
    tape_ptr = 0;
    tape_init();
    if (argc < 2 || argc > 4){
        usage();
        return 0;
    } 
    execute_bs(read_file(argv[1]));

    return 0;
}
