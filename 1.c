#include <stdio.h>
#include <windows.h>

int initexist(const char *path)
{
    DWORD attributes = GetFileAttributes(path);
    return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}
int initcheckup(const char *currentDirectory)
{
    char neogitPath[MAX_PATH];
    while (1)
    {
        snprintf(neogitPath, sizeof(neogitPath), "%s\\neogit", currentDirectory);
        if (initexist(neogitPath))
        {
            printf("Error: 'neogit' folder already exists in this directory or an upper directory.\n");
            return 1;
        }
        snprintf(neogitPath, sizeof(neogitPath), "%s\\..", currentDirectory);
        if (GetFullPathName(neogitPath, MAX_PATH, (LPSTR)currentDirectory, NULL) == 0)
        {
            printf("Error getting full path name.\n");
            return 1;
        }
        if (_stricmp(currentDirectory, "C:\\Users") == 0)
            break;
    }
    return 0;
}
void init()
{
    char currentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDirectory);
    char neogitPath[MAX_PATH];
    snprintf(neogitPath, sizeof(neogitPath), "%s\\neogit", currentDirectory);
    if (initcheckup(currentDirectory))
        return;
    if (CreateDirectory(neogitPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
    {
        DWORD attributes = GetFileAttributes(neogitPath);
        SetFileAttributes(neogitPath, attributes | FILE_ATTRIBUTE_HIDDEN);
        printf("Repos 'neogit' created.\n");

        char staged[MAX_PATH];
        snprintf(staged, sizeof(staged), "%s\\staged", neogitPath);
        CreateDirectory(staged, NULL);

        char unstaged[MAX_PATH];
        snprintf(unstaged, sizeof(unstaged), "%s\\unstaged", neogitPath);
        CreateDirectory(unstaged, NULL);
    }
    else
        printf("Error creating 'neogit'.\n");
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
