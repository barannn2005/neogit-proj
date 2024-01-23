#include <stdio.h>
#include <windows.h>

int init_exist(const char *path)
{
    DWORD attributes = GetFileAttributes(path);
    return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}
void init()
{
    char currentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDirectory);

    char neogitPath[MAX_PATH];
    snprintf(neogitPath, sizeof(neogitPath), "%s\\neogit", currentDirectory);

    if (init_exist(neogitPath))
    {
        printf("Error: 'neogit' folder already exists in this directory.\n");
        return;
    }

    char upperDirectory[MAX_PATH];
    snprintf(upperDirectory, sizeof(upperDirectory), "%s\\..\\neogit", currentDirectory);

    if (init_exist(upperDirectory))
    {
        printf("Error: 'neogit' folder already exists in the upper directory.\n");
        return;
    }

    if (CreateDirectory(neogitPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
    {
        DWORD attributes = GetFileAttributes(neogitPath);
        SetFileAttributes(neogitPath, attributes | FILE_ATTRIBUTE_HIDDEN);
        printf("Repository 'neogit' created and hidden.\n");
    }
    else
    {
        printf("Error creating 'neogit' repository.\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("how to use: %s <command>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "init") == 0)
    {
        init();
    }
    else
    {
        printf("Unknown command: %s\n", argv[1]);
    }

    return 0;
}
