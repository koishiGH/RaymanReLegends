# RaymanReLegends

## About

RaymanReLegends is my attempt to make a reimplementation of Rayman Legends's v1.3.140380 (Steam release) executable.
My goal with this is to AT LEAST get scenes loading and rendering properly.

You WILL need the original files from Rayman Legends's Steam Release (No idea if the Ubisoft Connect release works but it should...).
Without it this is basically useless.

This is A HEAVY WORK IN PROGRESS, NOT MUCH IS DONE YET.

## HELP WANTED

I would honestly like ANY sort of help with this, this is my SECOND time doing something related to reverse engineering (first time was making a cheat for an old Zenless Zone Zero build). If you'd like to help contact me on discord (_koisprite).

## Launch Arguments

### Changing what renderer to use

```bash
"Rayman Legends.exe" -renderer=auto # for the platform default
"Rayman Legends.exe" -renderer=dx9 # for directX 9
"Rayman Legends.exe" -renderer=sdl2 # for sdl2
```

## Links
* [How to build](Building.md)
* [Notes that I made](Notes.md)
* [Commit History (From before I messed up)](Commits.md)