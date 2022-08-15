#pragma once
#include <stdio.h>
#include <stdbool.h>

struct World
{
    bool** world;
    size_t width, height, generation;
};

struct Rules
{
    char* keep,* birth;
};

struct Arg
{
    char* argName,* argVal;
};

struct Command
{
    char* command;
    struct Arg* args;
    size_t argc;
};

void WriteToFile(const char* filename, const struct World* world, const struct Rules* rules);
void CreateDefault();
void ReadFromFile(const char* filename, struct World** world, struct Rules** rules);
size_t CountOccurences(const char* string, const char character);
bool StrIsOneOf(const char* string, const char** strArr, const size_t strArrLen);
void LowerCase(char* string);

struct Rules* GenRules(const char* keepRule, const char* birthRule);
void DestroyRules(struct Rules** rules);

struct World* GenWorld(const size_t width, const size_t height);
void DestroyWorld(struct World** world);
struct World* CopyWorld(const struct World* restrict world);
void DrawWorld(const struct World* world);
void ClearScreen();

unsigned char GetNeighborCount(struct World* world, const int xPos, const int yPos);
bool DoesRuleApply(const char* rule, const size_t neighbors);
void WorldStep(struct World* world, const struct Rules* rules);

struct Command* ReadCommand(char* string);
void DestroyCommand(struct Command** command);
void DestroyArg(struct Arg* arg);