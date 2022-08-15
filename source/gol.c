#include "gol.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void WriteToFile(const char* filename, const struct World* world, const struct Rules* rules)
{
    FILE* file = fopen(filename, "w");

    fprintf(file, "%zux%zu %s/%s\n", world->width, world->height, rules->keep, rules->birth);
    for (size_t y = 0; y < world->height; y++)
    {
        for (size_t x = 0; x < world->width; x++)
        {
            fputc(world->world[x][y] ? '#' : '.', file);
            fputc(' ', file);
        }
        fputc('\n', file);
    }
    fclose(file);
}
void ReadFromFile(const char* filename, struct World** world, struct Rules** rules)
{
    FILE* file = fopen(filename, "r");

    if (file == NULL)
    {
        *world = NULL;
        *rules = NULL;
        return;
    }

    char line[256];
    char c;
    size_t insert = 0;

    while ((c = getc(file)) != '\n' && c != EOF)
    {
        line[insert] = c;
        insert++;
    }
    line[insert] = 0;

    size_t mapX, mapY, rulesKeepNum, rulesBirthNum;
    sscanf(line, "%zux%zu %zu/%zu", &mapX, &mapY, &rulesKeepNum, &rulesBirthNum);

    char rulesKeep[8], rulesBirth[8];
    sprintf(rulesKeep, "%zu", rulesKeepNum);
    sprintf(rulesBirth, "%zu", rulesBirthNum);

    *world = GenWorld(mapX, mapY);
    *rules = GenRules(rulesKeep, rulesBirth);

    size_t insertX = 0, insertY = 0;
    while ((c = getc(file)) != EOF)
    {
        if (c == ' ')
            continue;
        if (c == '\n')
        {
            insertX = 0;
            insertY++;
            continue;
        }

        (*world)->world[insertX][insertY] = c == '#';
        insertX++;
    }

    fclose(file);
}
size_t CountOccurences(const char* string, const char character)
{
    size_t count = 0;
    for (; *string != 0; string++)
    {
        if (*string == character)
            count++;
    }
    return count;
}
bool StrIsOneOf(const char* string, const char** strArr, const size_t strArrLen)
{
    for (size_t s = 0; s < strArrLen; s++)
    {
        if (strcmp(string, strArr[s]) == 0)
            return true;
    }
    return false;
}
void LowerCase(char* string)
{
    for (; *string != 0; string++)
        tolower(*string);
}

struct Rules* GenRules(const char* keepRule, const char* birthRule)
{
    struct Rules* rules = malloc(sizeof(struct Rules));
    rules->birth = malloc(strlen(birthRule) + 1);
    memcpy(rules->birth, birthRule, strlen(birthRule) + 1);
    rules->keep = malloc(strlen(keepRule) + 1);
    memcpy(rules->keep, keepRule, strlen(keepRule) + 1);
    return rules;
}
void DestroyRules(struct Rules** rules)
{
    free((*rules)->birth);
    free((*rules)->keep);
    free(*rules);
    *rules = NULL;
}

struct World* GenWorld(const size_t width, const size_t height)
{
    struct World* retWorld = malloc(sizeof(struct World));
    retWorld->width = width;
    retWorld->height = height;
    retWorld->generation = 0;
    retWorld->world = malloc(sizeof(bool*) * retWorld->width);
    for (size_t c = 0; c < retWorld->width; c++)
    {
        retWorld->world[c] = calloc(retWorld->height, sizeof(bool));
    }
    return retWorld;
}
void DestroyWorld(struct World** world)
{
    for (size_t c = 0; c < (*world)->width; c++)
    {
        free((*world)->world[c]);
    }
    free((*world)->world);
    free(*world);
    *world = NULL;
}
struct World* CopyWorld(const struct World* restrict world)
{
    struct World* retWorld = GenWorld(world->width, world->height);
    retWorld->generation = world->generation;
    for (size_t c = 0; c < world->width; c++)
    {
        memcpy(retWorld->world[c], world->world[c], world->height);
    }
    return retWorld;
}

void DrawWorld(const struct World* world)
{
    printf("Generation %zu:\n", world->generation);
    char* row = calloc(world->width * 2 + 1, sizeof(char*));

    for (size_t i = 0; i < world->width; i++)
    {
        row[i * 2 + 1] = ' ';
    }
    row[world->width * 2] = 0;

    for (size_t r = 0; r < world->height; r++)
    {
        for (size_t c = 0; c < world->width; c++)
        {
            row[c * 2] = world->world[c][r] ? '#' : '.';
        }
        printf("%s\n", row);
    }
    free(row);
}
void ClearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif // _WIN32
}

unsigned char GetNeighborCount(struct World* world, const int xPos, const int yPos)
{
    unsigned char nCount = 0;
    for (int x = xPos - 1; x < xPos + 2; x++)
    {
        if (x < 0 || x >= world->width)
            continue;
        for (int y = yPos - 1; y < yPos + 2; y++)
        {
            if (y < 0 || y >= world->height)
                continue;
            if (x == xPos && y == yPos)
                continue;
            if (world->world[x][y])
                nCount++;
        }
    }
    return nCount;
}
bool DoesRuleApply(const char* rule, const size_t neighbors)
{
    size_t ruleLen = strlen(rule);
    for (size_t r = 0; r < ruleLen; r++)
    {
        char ruleNum = rule[r] - '0';
        if (neighbors == ruleNum)
            return true;
    }
    return false;
}
void WorldStep(struct World* world, const struct Rules* rules)
{
    struct World* wCpy = CopyWorld(world);
    world->generation++;
    for (size_t x = 0; x < world->width; x++)
    {
        for (size_t y = 0; y < world->height; y++)
        {
            unsigned char nCount = GetNeighborCount(wCpy, (int) x, (int) y);
            if (!world->world[x][y] && DoesRuleApply(rules->birth, nCount))
            {
                world->world[x][y] = true;
            }
            else if (world->world[x][y] && !DoesRuleApply(rules->keep, nCount))
            {
                world->world[x][y] = false;
            }
        }
    }
    DestroyWorld(&wCpy);
}

struct Command* ReadCommand(char* string)
{
    struct Command* restrict command = calloc(1, sizeof(struct Command));
    command->argc = CountOccurences(string, ' ');
    if (command->argc == 0)
    {
        command->args = NULL;
    }
    else
    {
        command->args = calloc(command->argc, sizeof(struct Arg));
    }

    char* part = strtok(string, " ");
    size_t cLen = strlen(part);
    command->command = malloc(cLen + 1);
    memcpy(command->command, part, cLen);
    command->command[cLen] = 0;

    part = strtok(NULL, " ");
    size_t arg = 0;
    while (part != NULL)
    {
        size_t sep = strcspn(part, "=");
        command->args[arg].argName = malloc(sep + 1);
        memcpy(command->args[arg].argName, part, sep);
        command->args[arg].argName[sep] = 0;

        if (part[sep] != 0)
        {
            size_t pLen = strlen(part);
            command->args[arg].argVal = malloc(pLen - sep);
            memcpy(command->args[arg].argVal, part + sep + 1, pLen - sep - 1);
            command->args[arg].argVal[pLen - 1] = 0;
        }

        arg++;
        part = strtok(NULL, " ");
    }

    return command;
}
void DestroyCommand(struct Command** command)
{
    free((*command)->command);
    for (size_t arg = 0; arg < (*command)->argc; arg++)
    {
        DestroyArg(&(*command)->args[arg]);
    }
    free(*command);
    *command = NULL;
}
void DestroyArg(struct Arg* arg)
{
    free((arg)->argName);
    free((arg)->argVal);
}