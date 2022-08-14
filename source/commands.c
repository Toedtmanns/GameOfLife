#include "commands.h"
#include <stdlib.h>
#include <string.h>

int SearchCommand(const char* commandName, const struct CommandLUTentry* lut, const size_t lutLen)
{
    for (size_t com = 0; com < lutLen; com++)
    {
        if (strcmp(commandName, lut[com].commandName) == 0)
            return (int) com;
    }
    return -1;
}
int ExecCommand(const struct Command* command, const struct CommandLUTentry* lut, const size_t lutLen, struct World* world, struct Rules* rules)
{
    int cIndex = SearchCommand(command->command, lut, lutLen);

    if (cIndex < 0)
        return cIndex;

    lut[cIndex].callback(world, rules, command->args, command->argc);

    return cIndex;
}
bool RequiredArgc(const size_t argc, const size_t requiredArgc)
{
    if (argc != requiredArgc)
    {
        printf("This command requires %zu args but %zu were given!\n", requiredArgc, argc);
    }
    return argc == requiredArgc;
}
bool RequiredArgcRange(const size_t argc, const size_t minArgc, const size_t maxArgc)
{
    if (minArgc != -1 && argc < minArgc)
    {
        printf("This command requires at least %zu args but %zu were given!\n", minArgc, argc);
        return true;
    }
    else if (maxArgc != -1 && argc > maxArgc)
    {
        printf("This command takes a maximum of %zu args but %zu were given!\n", maxArgc, argc);
        return false;
    }
    return true;
}
int GetArg(const char* argName, const struct Arg* args, const size_t argc)
{
    for (size_t arg = 0; arg < argc; arg++)
    {
        if (strcmp(argName, args[arg].argName) == 0)
            return (int) arg;
    }
    return -1;
}
void RedrawWorld(const struct World* world)
{
    ClearScreen();
    DrawWorld(world);
}

// Commands

void CommandExit(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    printf("Exiting\n");
}
void CommandHelp(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    printf("Help command\n");
}
void CommandSetTile(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    if (!RequiredArgcRange(argc, 2, 3))
        return;

    size_t x = 0, y = 0;
    int state = -1;

    int xArg = GetArg("x", args, argc);
    int yArg = GetArg("x", args, argc);
    int stateArg = GetArg("s", args, argc);

    if (xArg == -1)
    {
        sscanf(args[0].argName, "%zu", &x);
        sscanf(args[1].argName, "%zu", &y);

        if (argc == 3)
            sscanf(args[2].argName, "%d", &state);
    }
    else
    {
        if (xArg >= 0)
            sscanf(args[xArg].argVal, "%zu", &x);
        if (yArg >= 0)
            sscanf(args[yArg].argVal, "%zu", &y);
        else
            y = x;
        if (stateArg >= 0)
            sscanf(args[stateArg].argVal, "%d", &state);
    }

    if (state > 1)
        state = 1;
    else if (state < -1)
        state = 0;

    bool newState;

    if (state == -1)
    {
        newState = !world->world[x][y];
        world->world[x][y] = newState;
    }
    else
    {
        world->world[x][y] = state;
        newState = state;
    }

    ClearScreen();
    DrawWorld(world);
    printf("Set position %zu, %zu to %d\n", x, y, newState);
}
void CommandInvert(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    for (size_t x = 0; x < world->width; x++)
    {
        for (size_t y = 0; y < world->height; y++)
        {
            world->world[x][y] = !world->world[x][y];
        }
    }
    RedrawWorld(world);
    puts("Inverted the world");
}
void CommandClear(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    for (size_t x = 0; x < world->width; x++)
    {
        memset(world->world[x], 0, world->height);
    }
    RedrawWorld(world);
    puts("Cleared the world");
}
void CommandSave(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    if (!RequiredArgc(argc, 1))
        return;

    char* filename = malloc(strlen(args[0].argName) + 5);
    sprintf(filename, "%s%s", args[0].argName, ".txt");

    WriteFile(filename, world, rules);
    printf("Saved world as %s\n", filename);
    free(filename);
}
void CommandLoad(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    if (!RequiredArgc(argc, 1))
        return;

    char* filename = malloc(strlen(args[0].argName) + 5);
    sprintf(filename, "%s%s", args[0].argName, ".txt");

    FILE* file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("File '%s' does not exist!\n", filename);
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

    // Delete old world

    for (size_t x = 0; x < world->width; x++)
    {
        free(world->world[x]);
    }
    free(world->world);

    // Allocate new world memory

    world->width = mapX;
    world->height = mapY;
    world->generation = 0;
    world->world = malloc(sizeof(bool*) * world->width);
    for (size_t c = 0; c < world->width; c++)
    {
        world->world[c] = calloc(world->height, sizeof(bool));
    }

    // Delete old rules

    free(rules->birth);
    free(rules->keep);

    // Allocate new Rules memory

    rules->birth = malloc(strlen(rulesBirth) + 1);
    memcpy(rules->birth, rulesBirth, strlen(rulesBirth) + 1);
    rules->keep = malloc(strlen(rulesKeep) + 1);
    memcpy(rules->keep, rulesKeep, strlen(rulesKeep) + 1);

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

        world->world[insertX][insertY] = c == '#';
        insertX++;
    }

    fclose(file);

    RedrawWorld(world);
    printf("Successfully loaded '%s' with dimensions %zux%zu and rules %s/%s!\n", filename, world->width, world->height, rules->keep, rules->birth);
    free(filename);
}
void CommandNew(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    if (!RequiredArgcRange(argc, -1, 4))
        return;

    size_t width = 10, height = 10;
    const char* keepRule = "23",* birthRule = "3";

    switch (argc)
    {
    case 4:
        birthRule = args[3].argName;
    case 3:
        keepRule = args[2].argName;
    case 2:
        sscanf(args[1].argName, "%zu", &height);
    case 1:
        sscanf(args[0].argName, "%zu", &width);
    case 0:
        break;
    }

    for (size_t x = 0; x < world->width; x++)
    {
        free(world->world[x]);
    }
    free(world->world);

    free(rules->birth);
    free(rules->keep);

    *world = *GenWorld(width, height);
    *rules = *GenRules(keepRule, birthRule);

    RedrawWorld(world);
    printf("Created new world with dimensions %zux%zu and rules %s/%s!\n", width, height, keepRule, birthRule);
}