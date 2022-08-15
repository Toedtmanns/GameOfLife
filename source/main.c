#include "gol.h"
#include "commands.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>

const static struct CommandLUTentry commandLUT[] = {
    {"exit", CommandExit},
    {"help", CommandHelp},
    {"set", CommandSetTile},
    {"invert", CommandInvert},
    {"clear", CommandClear},
    {"save", CommandSave},
    {"load", CommandLoad},
    {"new", CommandNew},
    {"rules", CommandRules},
    {"info", CommandInfo},
    {"play", CommandPlay},
    {"step", CommandStep},
    {"about", CommandAbout}
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
            ReadFromFile(filename, &world, &rules);
        }
        else
        {
            ReadFromFile(argv[1], &world, &rules);
        }
    }
    else
    {
        ReadFromFile("default.txt", &world, &rules);
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
        if (executed == -1)
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

        DestroyCommand(&com);
        if (executed == 0)
        {
            break;
        }
    }

    

    DestroyWorld(&world);
    DestroyRules(&rules);
}