# Changelog

## Version 1.1.0
- Added Commands
  - [Rules](commands.md#rules): change world rules
  - [Info](commands.md#info): print world information
  - [Play](commands.md#play): let the simulation run
  - [List](commands.md#list): lists all available commands
  - [Help](commands.md#list): prints help for a specified command
  - [Step](commands.md#step): simulates a specified amount of generations as fast as possible
- If a command is unrecognized, the program will no longer step to the next generation but will warn the user instead
- Added command function macro
- Added file mapping functions

## Version 1.0.0
- Creation of the repository
- Added Game Of Life
- Added GameOfLife console
  - has various commands like `load`, `save`, `set`, `clear`, `exit` etc.