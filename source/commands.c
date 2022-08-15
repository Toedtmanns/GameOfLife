#include "commands.h"
#include "osFunc.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <signal.h>
#include <limits.h>


#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #define SLEEP(x) Sleep(x)
#else
    #include <unistd.h>
    #define SLEEP(x) usleep(x * 1000)
#endif // _WIN32

bool interrupt = false;

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
    {
        if (strcmp(command->command, "list") == 0)
        {
            puts("list");
            for (size_t c = 0; c < lutLen; c++)
                puts(lut[c].commandName);
            return INT_MAX;
        }

        printf("Command '%s' not found!\n", command->command);
        return -2;
    }

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
void IntHandler(int signum)
{
    interrupt = true;
    signal(SIGINT, SIG_DFL);
}

// Commands

void CommandExit(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    printf("Exiting\n");
}
void CommandHelp(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    char* helpStr;

    switch (argc)
    {
    case 1:
        helpStr = malloc(strlen(args[0].argName) + 4);
        sprintf(helpStr, "## %s", args[0].argName);
        break;
    case 0:
        helpStr = malloc(strlen("## help") + 1);
        sprintf(helpStr, "%s", "## help");
        break;
    }

    MappedMemory helpMem = MapFile("commands.md", ACS_READ);

    if (helpMem.memory == NULL)
    {
        puts("Help file 'commands.md' couldn't be found!");
        return;
    }

    //puts(helpMem.memory);
    
    char* helpLoc = strstr(helpMem.memory, helpStr);

    if (helpLoc == NULL)
    {
        if (argc == 1)
            printf("Help for command '%s' not found!\n", args[0].argName);
        else
            puts("Help for 'help' not found!");
        UnmapFile(&helpMem);
        return;
    }

    char* helpEnd = strstr(helpLoc + 3, "## ");
    if (helpEnd == NULL)
        helpEnd = strstr(helpLoc, "\0");

    printf("Command: %.*s\n", (int) (helpEnd - helpLoc - 4), helpLoc + 3);

    UnmapFile(&helpMem);
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

    WriteToFile(filename, world, rules);
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
void CommandRules(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    if (!RequiredArgcRange(argc, -1, 2))
        return;

    switch (argc)
    {
    case 2:
        free(rules->birth);
        rules->birth = malloc(strlen(args[1].argName) + 1);
        sprintf(rules->birth, "%s", args[1].argName);
    case 1:
        free(rules->keep);
        rules->keep = malloc(strlen(args[0].argName) + 1);
        sprintf(rules->keep, "%s", args[0].argName);
    case 0:
        break;
    }
    if (argc > 0)
        printf("Rules changed to %s/%s\n", rules->keep, rules->birth);
    else
        printf("Rules are: %s/%s\n", rules->keep, rules->birth);
}
void CommandInfo(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    printf("Dimensions of the world are: %zux%zu\n", world->width, world->height);
    printf("Rules are: %s/%s\n", rules->keep, rules->birth);
}
void CommandPerformance(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{

}
void CommandPlay(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    if (!RequiredArgcRange(argc, -1, 2))
        return;

    size_t wait = 125;
    int iterations = -1;

    switch (argc)
    {
    case 2:
        sscanf(args[1].argName, "%zu", &wait);
    case 1:
    {
        if (args[0].argVal == NULL)
            sscanf(args[0].argName, "%d", &iterations);
        else if (strcmp(args[0].argName, "wait") == 0)
            sscanf(args[0].argVal, "%zu", &wait);
        else
            printf("%s is not a valid argument!\n", args[0].argName);
    }
    case 0:
        break;
    }

    int startIter = iterations;

    signal(SIGINT, IntHandler);

    while (iterations != 0 && !interrupt)
    {
        SLEEP(wait);
        WorldStep(world, rules);
        RedrawWorld(world);
        if (iterations < 0)
            printf("Playing indefinitely, stop with Ctrl+C\n");
        iterations--;
        fflush(stdout);
    }
    interrupt = false;

    printf("Simulated %d generations\n", startIter - iterations);
}
void CommandStep(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    if (!RequiredArgcRange(argc, -1, 1))
        return;

    int steps = 1;

    if (argc == 1)
        sscanf(args[0].argName, "%i", &steps);

    printf("Simulating %d generations...\n", steps);
    for (int s = 0; s < steps; s++)
        WorldStep(world, rules);

    RedrawWorld(world);
    printf("Simulated %d generations\n", steps);
}
void CommandAbout(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc)
{
    printf("Conway's Game Of Life implementation v%s\n", PROG_VERSION_STR);
}