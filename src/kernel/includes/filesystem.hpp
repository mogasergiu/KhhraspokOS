#include <stddef.h>
#include <stdint.h>

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#define FILESYSTEM_PATH_MAX_SIZE 100
#define FILESYSTEM_DIR_MAX_SIZE 20

namespace FILESYSTEM {

    struct parsedPath {
        char currDir[FILESYSTEM_DIR_MAX_SIZE];
        parsedPath *nextDir, *prevDir;
    };

    class Path {
        private:
            char cwd[FILESYSTEM_PATH_MAX_SIZE];
            char oldcwd[FILESYSTEM_PATH_MAX_SIZE];

        public:
            Path();

            size_t getCwd(char *buffer) const;

            size_t getOldcwd(char *buffer) const;

            size_t setCwd(char *new_cwd);

            parsedPath* parsePath(char *path) const;
    };
};

extern FILESYSTEM::Path pathMgr;

#endif  /* FILESYSTEM_HPP */
