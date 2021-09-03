#include <filesystem.hpp>
#include <string.hpp>
#include <kpkheap.hpp>

using namespace FILESYSTEM;

FILESYSTEM::Path pathMgr;

Path::Path() {
    this->cwd[0] = this->oldcwd[0] = '/';
}

size_t Path::getCwd(char *buffer) const {
    return strncpy(buffer, this->cwd, FILESYSTEM_PATH_MAX_SIZE);
}

size_t Path::getOldcwd(char *buffer) const {
    return strncpy(buffer, this->oldcwd, FILESYSTEM_PATH_MAX_SIZE);
}

size_t Path::setCwd(char *new_cwd) {
    strncpy(this->oldcwd, this->cwd, FILESYSTEM_PATH_MAX_SIZE);

    return strncpy(this->cwd, new_cwd, FILESYSTEM_PATH_MAX_SIZE);
}

parsedPath* Path::parsePath(char *path) const {
    parsedPath *newPath = (parsedPath*)KPKHEAP::kpkZalloc(sizeof(*newPath));
    parsedPath *pathScout = newPath;

    char *ptr = strtok(path, "/");
    do {
        strncpy(pathScout->currDir, ptr, FILESYSTEM_DIR_MAX_SIZE);

        ptr = strtok(NULL, "/");

        if (ptr != NULL) {
            pathScout->nextDir = (parsedPath*)KPKHEAP::kpkZalloc(sizeof(*pathScout));
            pathScout->nextDir->prevDir = pathScout;
            pathScout = pathScout->nextDir;

        } else {
            break;
        }

    } while (1);

    return newPath;
}
