#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void    readCommandOutput(char *command, char **arr) {
    FILE    *output;
    int     i = 0;
    char    c = 0;

    if (0 == (output = (FILE*)popen(command, "r")))
    {
        perror("popen() failed.");
        exit(EXIT_FAILURE);
    }
    while (fread(&c, sizeof(c), 1, output) && i < 127)
    {
        (*arr)[i] = c;
        i++;
    }
    (*arr)[i] = '\0';
    pclose(output);
}

void    readBranchesOutput(char *command, char ***arr) {
    FILE    *output;
    int     i = 0;
    char    c = 0;
    int     character_count = 0;
    int     branches_count = 0;
    if (0 == (output = (FILE*)popen(command, "r")))
    {
        perror("popen() failed.");
        exit(EXIT_FAILURE);
    }
    while (fread(&c, sizeof(c), 1, output))
    {
        if ((*arr)[branches_count] == NULL) {
            (*arr)[branches_count] = malloc(sizeof(char) * 256);
        }
        if (c == '\n') {
            (*arr)[branches_count][character_count] = '\0';
            branches_count++;
            character_count = 0;
        } else if (c != ' ' && c != '*') {
            (*arr)[branches_count][character_count] = c;
            character_count++;
        }
    }
    (*arr)[branches_count] = NULL;
    pclose(output);
}

int   isBranchInArray(char *branchName, char ***mergedInMaster) {
    for (int i = 0; (*mergedInMaster)[i] != NULL; i++) {
        if (strcmp((*mergedInMaster)[i], branchName) == 0) {
            return 1;
        }
    }
    return 0;
}

void    printBranches(char ***allBranches) {
    for (int i = 0; (*allBranches)[i] != NULL; i++) {
        printf("%s\n", (*allBranches)[i]);
    }
}

void    unescape(char **unescapedCommand, const char *str) {
    int     j = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (i > 0 && str[i - 1] == '%' && str[i] == '%') {
            i++;
        }
        (*unescapedCommand)[j++] = str[i];
    }
    (*unescapedCommand)[j] = '\0';
}

void    printMergedBranches(char ***allBranches, char ***mergedInMaster) {
    char    *branchLastLog = calloc(128, sizeof(char));
    char    *command = NULL;
    char    *unescapedCommand = NULL;

    for (int i = 0; (*allBranches)[i] != NULL; i++) {
        command = malloc((strlen((*allBranches)[i]) + 150) * sizeof(char));
        unescapedCommand = malloc(50 * sizeof(char));
        unescape(&unescapedCommand, " -1 --pretty=format:\"%%h - %%an, %%ar : %%s\"\0");
        strcpy(command, "git log \0");
        strcat(command, (*allBranches)[i]);
        strcat(command, unescapedCommand);
        readCommandOutput(command, &branchLastLog);
        if (isBranchInArray((*allBranches)[i], mergedInMaster) == 1) {
            printf("%-30.30s merged in master ", (*allBranches)[i]);
        } else {
            printf("%-48s", (*allBranches)[i]);
        }
        printf("%s", branchLastLog);
        printf("\n");
        free(command);
        free(unescapedCommand);
    }
    free(branchLastLog);
}

void    freeBranches(char ***branches) {
    for (int i = 0; (*branches)[i] != NULL; i++) {
        free((*branches)[i]);
    }
    free(*branches);
}

int main(int ac, char **av) {
    char    **branchesMergedInMaster = calloc(200, sizeof(char*));
    char    **allBranches = calloc(200, sizeof(char*));

    readBranchesOutput("git branch --merge master", &branchesMergedInMaster);
    readBranchesOutput("git branch", &allBranches);
    //printBranches(&branchesMergedInMaster);
    //printBranches(&allBranches);
    printMergedBranches(&allBranches, &branchesMergedInMaster);
    freeBranches(&branchesMergedInMaster);
    freeBranches(&allBranches);
    return 0;
}