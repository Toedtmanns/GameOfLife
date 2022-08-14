# Commands

A list of all implemented commands

## load

Syntax: `load <filename>`

Loads a world from disk.

## save

Syntax: `save <filename>`

Saves a world to disk with the given filename.

## exit

Syntax: `exit`

Exits the application.

## Set

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

Syntax: `Invert`

Inverts all cells in the world.