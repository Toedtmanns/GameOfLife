# Game Of Life

This is an implementation of [Conway's Game Of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) in C which runs exclusively in the console and should be compilable on both,
Windows and Linux.  

## How it's used
---
You can use it like this: you load a text file which contains a small header which specifies the dimensions and rules of the world in the format: `width`x`height` `aliveRule`/`birthRule`.  
This looks something like this:  
`10x10 23/3`  

The world consists of `.` and `#` where `.` is dead and `#` is alive. Spaces are skipped.
You also don't have to fill the whole world in the file. It's enough if you have only a part of it, which will then be inserted into the world from the upper left corner.  
An example file could look like this:  
```
10x10 23/3
. # .
. . #
# # #
```

This will put a glider into the upper left corner.

## The Console
---

When the application is started, it generates a world with dimensions 10x10 and rules 23/3 (default rules). You can then either [load](commands.md#load) a world file,
create a new world with [new](commands.md#new) or set single cells to a specified state with [set](commands.md#set). You can step one generation forwards if you just hit enter.
If you're done you can [exit](commands.md#exit) the application.