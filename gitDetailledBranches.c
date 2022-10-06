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
    while (fread(&c, sizeof(c), 1, output) && i < 2047)
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

void    printMergedBranch(char **command, char **branch, int isBranchMerged, int shouldDelete, char **branchLastLog) {
    if (isBranchMerged == 1 && shouldDelete == 1 && strcmp(*branch, "master") != 0) {
        *command = strcpy(*command, "git branch -d ");
        *command = strcat(*command, *branch);
        system(*command);
        printf("\n\033[1;37mBranch : \033[1;32m%s\t\033[1;31m[DELETED]\n", *branch);
    } else {
        printf("\n\033[1;37mBranch : \033[1;32m%s\n", *branch);
    }
    printf("\033[1;37mMerged in master : ");
    if (isBranchMerged == 1) {
        printf("\033[0;32mYes\n");
    } else {
        printf("\033[0;31mNo\n");
    }
    printf("\033[1;37mLast commit : ");
    printf("\033[0;37m%s", *branchLastLog);
    printf("\n");
}

void    printMergedBranches(char ***allBranches, char ***mergedInMaster, int shouldDelete) {
    char    *branchLastLog = calloc(2048, sizeof(char));
    char    *command = NULL;
    int     isBranchMerged = 0;

    for (int i = 0; (*allBranches)[i] != NULL; i++) {
        command = malloc((strlen((*allBranches)[i]) + 150) * sizeof(char));
        strcpy(command, "git log ");
        strcat(command, (*allBranches)[i]);
        strcat(command, " -1\0");
        readCommandOutput(command, &branchLastLog);
        isBranchMerged = isBranchInArray((*allBranches)[i], mergedInMaster);
        printMergedBranch(&command, &(*allBranches)[i], isBranchMerged, shouldDelete, &branchLastLog);
        free(command);
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
    int     shouldRemove = 0;

    if (av[1] != NULL && (strcmp(av[1], "-d") == 0 || strcmp(av[1], "--delete") == 0)) {
        shouldRemove = 1;
    }
    readBranchesOutput("git branch --merge master", &branchesMergedInMaster);
    readBranchesOutput("git branch", &allBranches);
    printMergedBranches(&allBranches, &branchesMergedInMaster, shouldRemove);
    freeBranches(&branchesMergedInMaster);
    freeBranches(&allBranches);
    return 0;
}
