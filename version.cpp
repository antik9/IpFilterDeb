#include "version.h"

#define CONCAT_NX(A, B, C) A##.##B##.##C
#define CONCAT(A, B, C) CONCAT_NX(A, B, C)
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

std::string version() {
    return STRINGIZE(CONCAT(
                PROJECT_VERSION_MAJOR, 
                PROJECT_VERSION_MINOR, 
                PROJECT_VERSION_PATCH));
}

