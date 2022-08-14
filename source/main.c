#include "gol.h"
#include "commands.h"
#include <string.h>
#include <stdlib.h>
#define STRINGIFY(x) #x
#define MAX_INPUT 50

const char* PROG_VERSION_STR = STRINGIFY(PROG_VERSION);

const static struct CommandLUTentry commandLUT[] = {
    {"exit", CommandExit},
    {"help", CommandHelp},
    {"set", CommandSetTile},
    {"invert", CommandInvert},
    {"clear", CommandClear},
    {"save", CommandSave},
    {"load", CommandLoad},
    {"new", CommandNew}
};

int main(int argc, char* argv[])
{
    struct World* world;
    struct Rules* rules;

    if (argc == 2)
    {
        if (argv[1][strcspn(argv[1], ".")] == 0)
        {
            char* filename = malloc(strlen(argv[1]) + 5);
            sprintf(filename, "%s%s", argv[1], ".txt");
            ReadFile(filename, &world, &rules);
        }
        else
        {
            ReadFile(argv[1], &world, &rules);
        }
    }
    else
    {
        ReadFile("default.txt", &world, &rules);
    }


    if (world == NULL)
    {
        world = GenWorld(10, 10);
        rules = GenRules("23", "3");
    }

    char inBuffer[MAX_INPUT];
    struct Command* com = NULL;
    int executed = -1;

    while (true)
    {
        if (executed < 0)
        {
            ClearScreen();
            DrawWorld(world);
        }
        printf("> ");

        fflush(stdin);
        fgets(inBuffer, MAX_INPUT, stdin);

        if (*inBuffer == '\n')
        {
            WorldStep(world, rules);
            executed = -1;
            continue;
        }

        inBuffer[strcspn(inBuffer, "\n")] = 0;
        LowerCase(inBuffer);
        com = ReadCommand(inBuffer);

        executed = ExecCommand(com, commandLUT, sizeof(commandLUT) / sizeof(*commandLUT), world, rules);

        if (executed == 0)
        {
            break;
        }

        DestroyCommand(&com);
    }

    DestroyWorld(&world);
    DestroyRules(&rules);
}