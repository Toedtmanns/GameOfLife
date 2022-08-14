#pragma once
#include "gol.h"

typedef void (*CommandCBptr)(struct World*, struct Rules*, struct Arg* args, const size_t argc);

struct CommandLUTentry
{
    const char* commandName;
    CommandCBptr callback;
};

// Command execution functions

int SearchCommand(const char* commandName, const struct CommandLUTentry* lut, const size_t lutLen);
int ExecCommand(const struct Command* command, const struct CommandLUTentry* lut, const size_t lutLen, struct World* world, struct Rules* rules);

// Command helper functions

bool RequiredArgc(const size_t argc, const size_t requiredArgc);
bool RequiredArgcRange(const size_t argc, const size_t minArgc, const size_t maxArgc);
int GetArg(const char* argName, const struct Arg* args, const size_t argc);
void RedrawWorld(const struct World* world);

// Commands

void CommandExit(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc);
void CommandHelp(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc);
void CommandSetTile(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc);
void CommandInvert(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc);
void CommandClear(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc);
void CommandSave(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc);
void CommandLoad(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc);
void CommandNew(struct World* world, struct Rules* rules, struct Arg* args, const size_t argc);