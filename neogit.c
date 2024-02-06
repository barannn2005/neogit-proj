#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <wincrypt.h>
#include <stdbool.h>
#include <time.h>

void config(const char *nameoremail, const char *input)
{
    if (strcmp(nameoremail, "user.name") == 0)
    {
        FILE *txt = fopen("username.txt", "w");
        fprintf(txt, input);
        fclose(txt);
        printf("username config done\n");
    }
    else if (strcmp(nameoremail, "user.email") == 0)
    {
        FILE *txt = fopen("email.txt", "w");
        fprintf(txt, input);
        fclose(txt);
        printf("email config done\n");
    }
    else
        printf("wrong use of command\n");
    return;
}
int initexist(const char *path)
{
    DWORD attr = GetFileAttributes(path);
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
}
int initcheck(const char *currdr)
{
    char neo[MAX_PATH];
    while (1)
    {
        snprintf(neo, sizeof(neo), "%s\\neogit", currdr);
        if (initexist(neo))
        {
            printf("neogit repos already exists in this dir or parent dirs\n");
            return 1;
        }
        snprintf(neo, sizeof(neo), "%s\\..", currdr);
        if (GetFullPathName(neo, MAX_PATH, (LPSTR)currdr, NULL) == 0)
        {
            printf("error getting parent dir\n");
            return 1;
        }
        if (_stricmp(currdr, "C:\\Users") == 0)
            break;
    }
    return 0;
}
void init()
{
    char currentdr[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentdr);
    char neo[MAX_PATH];
    snprintf(neo, sizeof(neo), "%s\\neogit", currentdr);
    if (initcheck(currentdr))
        return;
    if (CreateDirectory(neo, NULL))
    {
        DWORD att = GetFileAttributes(neo);
        SetFileAttributes(neo, att | FILE_ATTRIBUTE_HIDDEN);
        printf("Repos 'neogit' created\n");
        char current_branch[MAX_PATH];
        snprintf(current_branch, sizeof(current_branch), "%s\\current_branch.txt", neo);
        FILE *bfile = fopen(current_branch, "w");
        fprintf(bfile, "master");
        fclose(bfile);
        char staged[MAX_PATH];
        snprintf(staged, sizeof(staged), "%s\\staged", neo);
        CreateDirectory(staged, NULL);
        char commits[MAX_PATH];
        snprintf(commits, sizeof(commits), "%s\\commited", neo);
        CreateDirectory(commits, NULL);
        char commit_num[MAX_PATH];
        snprintf(commit_num, sizeof(commit_num), "%s\\commit_num.txt", commits);
        FILE *file = fopen(commit_num, "w");
        int num = 0;
        fprintf(file, "%d", num);
        fclose(file);
        char branches[MAX_PATH];
        snprintf(branches, sizeof(branches), "%s\\branch", neo);
        CreateDirectory(branches, NULL);
        char master[MAX_PATH];
        snprintf(master, sizeof(master), "%s\\master", branches);
        CreateDirectory(master, NULL);
    }
    else
        printf("error creating 'neogit'\n");
}
int areFilesIdentical(const char *filePath1, const char *filePath2) /* the function is written by chatgpt */
{
    FILE *file1 = fopen(filePath1, "rb");
    FILE *file2 = fopen(filePath2, "rb");
    if (file1 == NULL || file2 == NULL)
    {
        printf("file or dir has not been added at all\n");
        if (file1 != NULL)
            fclose(file1);
        if (file2 != NULL)
            fclose(file2);
        return 0;
    }
    int character1, character2;
    while (1)
    {
        character1 = fgetc(file1);
        character2 = fgetc(file2);
        if (character1 != character2)
        {
            fclose(file1);
            fclose(file2);
            return 0;
        }
        if (character1 == EOF)
            break;
    }
    fclose(file1);
    fclose(file2);
    return 1;
}
void copyFile(const char *source, const char *destin) /* the function is written by chatgpt */
{
    FILE *sourceFile, *destinationFile;
    unsigned char buffer[4000];
    size_t bytesRead, bytesWritten;
    sourceFile = fopen(source, "rb");
    if (sourceFile == NULL)
    {
        perror("cant open source file\n");
        return;
    }
    destinationFile = fopen(destin, "wb");
    if (destinationFile == NULL)
    {
        printf("error occured; make sure the current path is your repos\n");
        fclose(sourceFile);
        return;
    }
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0)
    {
        bytesWritten = fwrite(buffer, 1, bytesRead, destinationFile);
        if (bytesWritten != bytesRead)
        {
            perror("error writing to destination file\n");
            fclose(sourceFile);
            fclose(destinationFile);
            return;
        }
    }
    fclose(sourceFile);
    fclose(destinationFile);
    printf("done successfully\n");
    return;
}
void copyfiletodr(const char *source, const char *destin)
{
    const char *fileName = strrchr(source, '\\');
    if (fileName == NULL)
        fileName = source;
    else
        fileName++;
    char despath[200];
    snprintf(despath, sizeof(despath), "%s\\%s", destin, fileName);
    if (areFilesIdentical(source, despath))
        printf("the modification has been done no need to do it again\n");
    else
        copyFile(source, despath);
}
void copydr(const char *srdir, const char *desdir)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char sourcep[MAX_PATH];
    char destp[MAX_PATH];
    if (!CreateDirectory(desdir, NULL))
    {
        printf("error occured; make sure the current path is your repos\n");
        return;
    }
    snprintf(sourcep, sizeof(sourcep), "%s\\*", srdir);
    hFind = FindFirstFile(sourcep, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("error getting files in directory\n");
        return;
    }
    do
    {
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0)
        {
            snprintf(sourcep, sizeof(sourcep), "%s\\%s", srdir, findFileData.cFileName);
            snprintf(destp, sizeof(destp), "%s\\%s", desdir, findFileData.cFileName);
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                copydr(sourcep, destp);
            else
                copyFile(sourcep, destp);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
}
void add(const char *source)
{
    char cd[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, cd);
    char destin[MAX_PATH];
    snprintf(destin, sizeof(destin), "%s\\staged", cd);
    DWORD attributes = GetFileAttributes(source);
    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        printf("error getting file\n");
        return;
    }
    if (attributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        char destin1[MAX_PATH];
        snprintf(destin1, sizeof(destin1), "%s\\staged\\%s", cd, source);
        copydr(source, destin1);
    }
    else
        copyfiletodr(source, destin);
}
BOOL FileExistsInDirectory(const char *directory, const char *filename)
{
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s\\%s", directory, filename);
    DWORD attributes = GetFileAttributesA(path);
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}
BOOL CompareFileContents(const char *file1, const char *file2)
{
    FILE *fp1 = fopen(file1, "rb");
    FILE *fp2 = fopen(file2, "rb");
    if (!fp1 || !fp2)
    {
        if (fp1)
            fclose(fp1);
        if (fp2)
            fclose(fp2);
        return FALSE;
    }
    fseek(fp1, 0, SEEK_END);
    fseek(fp2, 0, SEEK_END);
    long size1 = ftell(fp1);
    long size2 = ftell(fp2);
    if (size1 != size2)
    {
        fclose(fp1);
        fclose(fp2);
        return FALSE;
    }
    fseek(fp1, 0, SEEK_SET);
    fseek(fp2, 0, SEEK_SET);
    char buffer1[1024];
    char buffer2[1024];
    while (!feof(fp1) && !feof(fp2))
    {
        size_t bytesRead1 = fread(buffer1, 1, sizeof(buffer1), fp1);
        size_t bytesRead2 = fread(buffer2, 1, sizeof(buffer2), fp2);
        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0)
        {
            fclose(fp1);
            fclose(fp2);
            return FALSE;
        }
    }
    fclose(fp1);
    fclose(fp2);
    return TRUE;
}
void compare(const char *dir1, const char *dir2)
{
    WIN32_FIND_DATAA findFileData;
    char path1[MAX_PATH];
    char path2[MAX_PATH];
    HANDLE hFind = FindFirstFileA(strcat(strcpy(path1, dir1), "\\*"), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Error opening directory %s\n", dir1);
        return;
    }
    do
    {
        const char *filename = findFileData.cFileName;
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }
        if (strcmp(filename, "username.txt") == 0)
        {
            continue;
        }
        if (strcmp(filename, "useremail.txt") == 0)
        {
            continue;
        }
        if (!FileExistsInDirectory(dir2, filename))
        {
            printf("file %s has not been added\n", filename, dir2);
        }
        else
        {
            snprintf(path1, sizeof(path1), "%s\\%s", dir1, filename);
            snprintf(path2, sizeof(path2), "%s\\%s", dir2, filename);
            if (!CompareFileContents(path1, path2))
            {
                printf("file %s has been added and then modified\n", filename);
            }
            else
            {
                printf("file %s has been added\n", filename);
            }
        }

    } while (FindNextFileA(hFind, &findFileData) != 0);
    FindClose(hFind);
}

void deleteFileOrDirectory(const char *path)
{
    WIN32_FIND_DATAA findFileData;
    char searchPath[MAX_PATH];
    snprintf(searchPath, MAX_PATH, "%s\\*", path);
    HANDLE hFind = FindFirstFileA(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND)
        {
            HANDLE hFile = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hFile);
                if (!DeleteFileA(path))
                {
                    printf("Error deleting file '%s'. Error code: %lu\n", path, GetLastError());
                }
                else
                {
                    printf("File deleted successfully: %s\n", path);
                }
            }
            else
            {
                printf("Error opening file '%s'. Error code: %lu\n", path, GetLastError());
            }
        }
        else
        {
            printf("Error finding files in directory. Error code: %lu\n", error);
        }
        return;
    }
    do
    {
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0)
        {
            char filePath[MAX_PATH];
            snprintf(filePath, MAX_PATH, "%s\\%s", path, findFileData.cFileName);

            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                deleteFileOrDirectory(filePath);
            }
            else
            {
                HANDLE hFile = CreateFileA(filePath, GENERIC_WRITE, FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, NULL);
                if (hFile != INVALID_HANDLE_VALUE)
                {
                    CloseHandle(hFile);
                    if (!DeleteFileA(filePath))
                    {
                        printf("Error deleting file '%s'. Error code: %lu\n", filePath, GetLastError());
                    }
                    else
                    {
                        printf("File deleted successfully: %s\n", filePath);
                    }
                }
                else
                {
                    printf("Error opening file '%s'. Error code: %lu\n", filePath, GetLastError());
                }
            }
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);

    if (!RemoveDirectoryA(path))
    {
        printf("Error deleting directory '%s'. Error code: %lu\n", path, GetLastError());
    }
    else
    {
        printf("Directory deleted successfully: %s\n", path);
    }
}
void changebranch(const char *namebr)
{
    char crbrname[100];
    FILE *filebr = fopen("current_branch.txt", "r");
    fscanf(filebr, "%s", crbrname);
    fclose(filebr);
    char currentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDirectory);
    WIN32_FIND_DATAA findFileData;
    char path1[MAX_PATH];
    HANDLE hFind = FindFirstFileA(strcat(strcpy(path1, currentDirectory), "\\*"), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Error opening directory %s\n", currentDirectory);
        return;
    }
    do
    {
        const char *filename = findFileData.cFileName;
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            char destinn[MAX_PATH];
            char sourcee[MAX_PATH];
            printf("Directory: %s\\%s\n", currentDirectory, filename);
            snprintf(destinn, sizeof(destinn), "%s\\branch\\%s\\%s", currentDirectory, crbrname, filename);
            snprintf(sourcee, sizeof(sourcee), "%s\\%s", currentDirectory, filename);
            copydr(sourcee, destinn);
            deleteFileOrDirectory(sourcee);
            continue;
        }
        else if (strcmp(filename, "username.txt") == 0)
        {
            printf("Ignoring file: %s\n", filename);
            continue;
        }
        else if (strcmp(filename, "useremail.txt") == 0)
        {
            printf("Ignoring file: %s\n", filename);
            continue;
        }
        else if (strcmp(filename, "branch") == 0)
        {
            printf("Ignoring file: %s\n", filename);
            continue;
        }
        else if (strcmp(filename, "commited") == 0)
        {
            printf("Ignoring file: %s\n", filename);
            continue;
        }
        else
        {
            char destinn[MAX_PATH];
            char sourcee[MAX_PATH];
            printf("file: %s\\%s\n", currentDirectory, filename);
            snprintf(destinn, sizeof(destinn), "%s\\branch\\%s\\%s", currentDirectory, crbrname, filename);
            snprintf(sourcee, sizeof(sourcee), "%s\\%s", currentDirectory, filename);
            copyfiletodr(sourcee, destinn);
        }

    } while (FindNextFileA(hFind, &findFileData) != 0);
    FindClose(hFind);
    FILE *filebrr = fopen("current_branch.txt", "w");
    fprintf(filebrr, "%s", namebr);
    fclose(filebrr);
    char new[MAX_PATH];
    snprintf(new, sizeof(new), "%s\\branch\\%s", currentDirectory, namebr);
    CreateDirectory(new, NULL);
}
void checkoutbranch(const char *brachname)
{
    char currentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDirectory);
    char txtfillee[MAX_PATH];
    snprintf(txtfillee, sizeof(txtfillee), "%s\\commited\\commit_num.txt", currentDirectory);
    int num;
    FILE *comnum = fopen(txtfillee, "r");
    fscanf(comnum, "%d", &num);
    fclose(comnum);
    for (int i = num; num > 1; i--)
    {
        char *brrr;
        char cmfillee[MAX_PATH];
        snprintf(cmfillee, sizeof(cmfillee), "%s\\commited\\branch_%d.txt", currentDirectory, num);
        FILE *combb = fopen(cmfillee, "r");
        fscanf(combb, "%s", &brrr);
        fclose(combb);
        if (strcmp(brachname, brrr) == 0)
        {
            char destinn[MAX_PATH];
            char sourcee[MAX_PATH];
            snprintf(sourcee, sizeof(sourcee), "%s\\commited\\%d", currentDirectory, num);
            snprintf(destinn, sizeof(destinn), "%s", currentDirectory);
            copydr(sourcee, destinn);
        }
    }
}
void checkoutid(const char *id)
{
    char currentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDirectory);
    char txtfillee[MAX_PATH];
    snprintf(txtfillee, sizeof(txtfillee), "%s\\commited\\commit_num.txt", currentDirectory);
    int num;
    FILE *comnum = fopen(txtfillee, "r");
    fscanf(comnum, "%d", &num);
    fclose(comnum);
    for (int i = num; num > 1; i--)
    {
        char *idd;
        char cmfillee[MAX_PATH];
        snprintf(cmfillee, sizeof(cmfillee), "%s\\commited\\id_%d.txt", currentDirectory, num);
        FILE *combb = fopen(cmfillee, "r");
        fscanf(combb, "%s", &idd);
        fclose(combb);
        if (strcmp(id, idd) == 0)
        {
            char destinn[MAX_PATH];
            char sourcee[MAX_PATH];
            snprintf(sourcee, sizeof(sourcee), "%s\\commited\\%d", currentDirectory, num);
            snprintf(destinn, sizeof(destinn), "%s", currentDirectory);
            copydr(sourcee, destinn);
        }
    }
}
void checkouthead()
{
    char currentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDirectory);
    char txtfillee[MAX_PATH];
    snprintf(txtfillee, sizeof(txtfillee), "%s\\commited\\commit_num.txt", currentDirectory);
    int num;
    FILE *comnum = fopen(txtfillee, "r");
    fscanf(comnum, "%d", &num);
    fclose(comnum);
    char destinn[MAX_PATH];
    char sourcee[MAX_PATH];
    snprintf(sourcee, sizeof(sourcee), "%s\\commited\\%d", currentDirectory, num);
    snprintf(destinn, sizeof(destinn), "%s", currentDirectory);
    copydr(sourcee, destinn);
}
void grep(const char *filen, const char *word)
{

    FILE *file = fopen(filen, "r");
    char line[5000];
    int ind = 0;
    while (fgets(line, 5000, file) != NULL)
    {
        ind++;
        if (strstr(line, word) != NULL)
        {
            printf("Line %d: %s", ind, line);
        }
    }
    fclose(file);
}

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "config") == 0)
    {
        if (strcmp(argv[2], "-global") == 0)
        {
            config(argv[3], argv[4]);
        }
        else
        {
            char currentpath[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, currentpath);
            char *lastBackslash = strrchr(currentpath, '\\');
            if (lastBackslash != NULL)
            {
                char *lastPart = lastBackslash + 1;
                if (strcmp(lastPart, "neogit") == 0)
                {
                    config(argv[2], argv[3]);
                }
                else
                {
                    printf("please first go into neogit directory.\n");
                    return 1;
                }
            }
        }
    }
    else if (strcmp(argv[1], "init") == 0)
    {
        init();
    }
    else if (strcmp(argv[1], "add") == 0)
    {
        if (argc == 3)
            add(argv[2]);
        else if (strcmp(argv[2], "-f") == 0)
        {
            for (int i = 3; i < argc; i++)
            {
                add(argv[i]);
            }
        }
    }
    else if (strcmp(argv[1], "reset") == 0)
    {
        deleteFileOrDirectory(argv[2]);
    }
    else if (strcmp(argv[1], "status") == 0)
    {
        char currentpath[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, currentpath);
        char destin[MAX_PATH];
        snprintf(destin, sizeof(destin), "%s\\staged", currentpath);
        compare(currentpath, destin);
    }
    else if (strcmp(argv[1], "commit") == 0)
    {
        if (argc == 2)
            printf("correct usage is = neogit commit -m comment\n");
        else if (strcmp(argv[2], "-m") == 0)
        {
            if (argc == 4)
            {
                char *comment = argv[3];
                if (strlen(comment) > 72)
                {
                    printf("Error: Comment is more than 72 characters\n");
                    return 1;
                }
                char currentDirectory[MAX_PATH];
                GetCurrentDirectory(MAX_PATH, currentDirectory);
                char txtfillee[MAX_PATH];
                snprintf(txtfillee, sizeof(txtfillee), "%s\\commited\\commit_num.txt", currentDirectory);
                int num;
                FILE *comnum = fopen(txtfillee, "r");
                fscanf(comnum, "%d", &num);
                fclose(comnum);
                char commenttxt[MAX_PATH];
                int newnum;
                newnum = num + 1;

                snprintf(commenttxt, sizeof(commenttxt), "%s\\commited\\1000_%d.txt", currentDirectory, newnum);
                FILE *commenti = fopen(commenttxt, "w");
                fprintf(commenti, "%s", comment);
                fclose(commenti);
                char destin[MAX_PATH];
                snprintf(destin, sizeof(destin), "%s\\commited\\%d", currentDirectory, newnum);
                CreateDirectory(destin, NULL);
                char source[MAX_PATH];
                snprintf(source, sizeof(source), "%s\\staged", currentDirectory);
                comnum = fopen(txtfillee, "w");
                fprintf(comnum, "%d", newnum);
                fclose(comnum);
                copydr(source, destin);
                printf("commit id = 1000_%d\n", newnum);
                printf("the comment = %s\n", comment);

                time_t currentTime = time(NULL);
                struct tm *localTime = localtime(&currentTime);

                printf("Current date and time: %s", asctime(localTime));
                char tmtxt[MAX_PATH];
                snprintf(tmtxt, sizeof(tmtxt), "%s\\commited\\time_%d.txt", currentDirectory, newnum);
                FILE *timefile = fopen(tmtxt, "w");
                fprintf(timefile, "%s", asctime(localTime));
                fclose(timefile);
            }

            else
            {
                printf("enter comment\n");
            }
        }
        else
        {
            printf("correct usage is = neogit commit -m comment\n");
        }
    }
    else if (strcmp(argv[1], "branch") == 0)
    {
        if (argc == 3)
        {

            char brname[100];
            FILE *filebr = fopen("current_branch.txt", "r");
            fscanf(filebr, "%s", brname);
            if (strcmp(argv[2], brname) == 0)
                printf("this branch exists");
            else
            {
                changebranch(argv[2]);
            }
            fclose(filebr);
        }
        else if (argc == 2)
        {
            char currentDirectory[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, currentDirectory);
            char branches[MAX_PATH];
            snprintf(branches, sizeof(branches), "%s\\branch", currentDirectory);
            WIN32_FIND_DATAA findFileData;
            char path1[MAX_PATH];
            HANDLE hFind = FindFirstFileA(strcat(strcpy(path1, branches), "\\*"), &findFileData);
            if (hFind == INVALID_HANDLE_VALUE)
            {
                printf("Error opening directory %s\n", currentDirectory);
                return 1;
            }
            do
            {
                const char *filename = findFileData.cFileName;
                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    printf("branch : %s\n", filename);
                    continue;
                }
            } while (FindNextFileA(hFind, &findFileData) != 0);
        }
    }
    else if (strcmp(argv[1], "checkout") == 0)
    {
        if (strcmp(argv[2], "-b") == 0)
        {
            checkoutbranch(argv[3]);
        }
        else if (strcmp(argv[2], "-i") == 0)
        {
            checkoutid(argv[3]);
        }
        else if (strcmp(argv[2], "HEAD") == 0)
        {
            checkouthead();
        }
    }
    else if (strcmp(argv[1], "revert") == 0)
    {
        checkoutid(argv[3]);
    }
    else if (strcmp(argv[1], "grep") == 0)
    {
        grep(argv[2], argv[3]);
    }
    else
    {
        printf("Unknown command: %s\n", argv[1]);
    }
    return 0;
}
