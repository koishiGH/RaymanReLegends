# RaymanReLegends

## About

RaymanReLegends is my attempt to make a reimplementation of Rayman Legends's v1.3.140380 (Steam release) executable.
My goal with this is to AT LEAST get scenes loading and rendering properly.
I want this to be a simple drag and drop replacement for the original RaymanLegends.exe file.

You WILL need the original files from Rayman Legends's Steam Release (No idea if the Ubisoft Connect release works but it should...).
Without it this is basically useless.

This is A HEAVY WORK IN PROGRESS, NOT MUCH IS DONE YET.

## SMALL NOTICE

This probably won't be updated as much unless I genuinely have freetime. School's starting on the 25th so I won't have a lot of time to work on this. I WILL try my best to get small progress done throughout the year but I cannot make no promises.

## HELP WANTED

I would honestly like ANY sort of help with this, this is my SECOND time doing something related to reverse engineering (first time was making a cheat for an old Zenless Zone Zero build). If you'd like to help contact me on discord (_flixel).

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

## Credits / shoutouts

- [koi (me!)](https://twitter.com/maybekoi_) - the dude thats doing the reimplementation

- [RayCarrot](https://github.com/RayCarrot) - A push into the right direction, [Rayman Control Panel](https://github.com/RayCarrot/RayCarrot.RCP.Metro), and [BinarySerializer.UbiArt](https://github.com/BinarySerializer/BinarySerializer.UbiArt)