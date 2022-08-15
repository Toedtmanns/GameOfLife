# Commands

A list of all implemented commands

## help

Syntax: `help (<command>)`

Displays help about a given command or, if no command is given, displays this text.

## list

Syntax: `list`

Lists all commands.

## load

Syntax: `load <filename>`

Loads a world from disk.

## save

Syntax: `save <filename>`

Saves a world to disk with the given filename.

## exit

Syntax: `exit`

Exits the application.

## set

Syntax: `set <x-pos> <y-pos> (<state>)`

Sets the specified cell to the given state or toggles it if no state was given.  
Example:
```
set 5 7     ; Toggles the cell at position x:5 y:7
set 3 4 1   ; Sets the cell at x:3 y:4 to alive
```

## new

Syntax `new <width> <height> <aliveRule> <birthRule>`

Creates a new world with given parameters.  
Example:
```
set 25 10 23 3  ; Creates a new world with dimensions 25x10 and rules 23/3
```

## clear

Syntax: `clear`

Clears the world.

## invert

Syntax: `invert`

Inverts all cells in the world.

## rules

Syntax: `rules <aliveRule> <keepRule>`

Changes the rules. If no new rules are specified, the current rules get printed.

## info

Syntax: `info`

Prints information about the world such as world dimensions and rules.

## play

Syntax: `play (<iterations>) (<waitTime>)` or  
Syntax: `play wait=<waitTime>`

Lets the simulation run. If no arguments are given it runs indefinitely with a step approx. every 125 ms. This can be interrupted with Ctrl+C. Wait time is in milliseconds.

## step

Syntax: `step <generations>`

Simulates the specified amount of generations as fast as possible.

## about

Syntax: `about`

Print information about the software