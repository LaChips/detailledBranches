#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void    readOutput(char *command, FILE **output, char ***arr, int *currentBranchIndex) {
    int     i = 0;
    char    c = 0;
    int     character_count = 0;
    int     branches_count = 0;
    if (0 == (*output = (FILE*)popen(command, "r")))
    {
        perror("popen() failed.");
        exit(EXIT_FAILURE);
    }
    while (fread(&c, sizeof(c), 1, *output))
    {
        if ((*arr)[branches_count] == NULL) {
            (*arr)[branches_count] = malloc(sizeof(char) * 200);
        }
        if (c == '\n') {
            (*arr)[branches_count][character_count] = '\0';
            branches_count++;
            character_count = 0;
        } else if (c != ' ' && c != '*') {
            (*arr)[branches_count][character_count] = c;
            character_count++;
        } else if (c == '*') {
            *currentBranchIndex = branches_count;
        }
    }
    (*arr)[branches_count] = NULL;
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

void    printMergedBranches(char ***allBranches, char ***mergedInMaster, int *currentBranchIndex) {
    for (int i = 0; (*allBranches)[i] != NULL; i++) {
        if (isBranchInArray((*allBranches)[i], mergedInMaster) == 1)
            printf("%s\tmerged in master\n", (*allBranches)[i]);
        else {
            printf("%s\n", (*allBranches)[i]);
        }
    }
}

void    freeBranches(char ***branches) {
    for (int i = 0; (*branches)[i] != NULL; i++) {
        free((*branches)[i]);
    }
    free(*branches);
}

int main(int ac, char **av) {
    FILE    *allBranchesOutput;
    FILE    *mergedInMasterOutput;
    char    **branchesMergedInMaster = calloc(200, sizeof(char*));
    char    **allBranches = calloc(200, sizeof(char*));
    int     currentBranchIndex = -1;

    readOutput("git branch --merge master", &mergedInMasterOutput, &branchesMergedInMaster, &currentBranchIndex);
    readOutput("git branch", &allBranchesOutput, &allBranches, &currentBranchIndex);
    pclose(allBranchesOutput);
    pclose(mergedInMasterOutput);
    //printBranches(&branchesMergedInMaster);
    //printBranches(&allBranches);
    printMergedBranches(&allBranches, &branchesMergedInMaster, &currentBranchIndex);
    freeBranches(&branchesMergedInMaster);
    freeBranches(&allBranches);
    return 0;
}