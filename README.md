# Lunar Monitor

**NOTE:** Currently this tool only works with Lunar Magic 3.30 and 3.31 (there are two separate versions available).

**DISCLAIMER:** I cannot guarantee that using this program will not corrupt your ROM data or export corrupted data. I have tested all functionality and not had any issues but considering I am hijacking a program whose internals I do not fully understand (Lunar Magic) there is always potential for something to go wrong so keep that in mind if you want to use this tool.

Allows for automatic exporting of resources (levels, map16, shared palettes, overworld/title screen/credits) while using Lunar Magic to edit Super Mario World ROMs.

NOTE: Only tested on Windows, may not work on other operating systems.

For setup see [Setup](https://github.com/Underrout/lunar-monitor#setup).

## Why is this a thing?

Projects using git in combination with the convenient [Lunar Helper](https://github.com/MaddyThorson/LunarHelper) build system will likely experience the issue that git will sometimes spot "changes" in files generated via Lunar Helper's "Save" function that didn't really occur. Most often this seems to happen with level files, which appear as "modified" after each "Save" operation (at least for me).

In order to get around this issue I decided to create Lunar Monitor, which uses DLL injection in combination with Microsoft's [Detours](https://github.com/microsoft/Detours) package to directly modify Lunar Magic's code while it's running. The modifications Lunar Monitor makes result in Lunar Magic exporting resources from the ROM as soon as you save them. As a result, Lunar Helper's "Save" function is no longer needed, since everything is "saved" as soon as it happens in Lunar Magic.

## Why is this a *good* thing?

Only exporting resources that (very likely) just changed results in:

- Clearer commit histories that only include relevant changes
- Fewer merge conflicts
- Smaller repository sizes due to fewer binary files changing
- Less time spent cleaning up commits/manually exporting resources
- Smaller chance of work being lost due to forgetting to export changes
- The ROM and its environment staying in sync (somewhat)

## Limitations

The only limitation I am currently aware of is that global exanimation is **not** automatically exported as soon as it is saved, because as far as I know it's saved via the same save button as levels. Since I don't want the `global_data.bps` patch to "change" on every level save I have decided not to output a new patch whenever a level is saved. Global exanimation will still be saved whenever the overworld, title screen or credits are saved though, so if you change the global exanimations and save one of those three right after your changes will be saved as expected.

Currently not planning on fixing this because I have no easy way to tell whether global exanimation was changed and should be re-exported.

Some errors messages Lunar Magic would usually pop up when something went wrong while trying to save data to the ROM (i.e. ROM was renamed, moved, etc.) will not pop up.

It's possible operating systems and/or antivirus software may flag this as malware since DLL injection is sometimes used by malicious software. If you are concerned about this, feel free to look through the source code, which is completely public, and build the binaries from it yourself instead of using binaries I provide directly (which, to be clear, are built from the exact same source code and only provided for convenience).

Lunar Monitor will very likely **not** work with older or future Lunar Magic versions, since addresses may move around. The Lunar Magic versions I worked off of and tested with are 3.30 and 3.31.

## Building

Build this project and [lunar-monitor-injector](https://github.com/Underrout/lunar-monitor-injector) in Visual Studio (I used VS2019) in **x86** Release mode.

The relevant generated binary files will be `lunar-monitor-injector.exe` and `lunar-monitor.dll`. The injector is a very simple DLL injection executable that simply injects the DLL as soon as it runs and waits until Lunar Magic closes.

## Setup

NOTE: If you want to use this with Lunar Helper, ideally you should use [this fork](https://github.com/Underrout/LunarHelper/releases/tag/v1.0.0-lmc) which removes the "Save" function, since it's not needed if you're using Lunar Monitor.

Also please be aware of the current [limitations](https://github.com/Underrout/lunar-monitor#limitations).

To set this up you will need the `lunar-monitor-injector.exe`, `lunar-monitor.dll`, `usertoolbar.txt` and `lunar-monitor-config.txt` files. On the right side of this page there is a "Releases" tab where you can get the latest up-to-date zip containing all the files you need.

Alternatively you can build this project and [lunar-monitor-injector](https://github.com/Underrout/lunar-monitor-injector) yourself as described above and gather the relevant files from the build results.

To actually get set up, put `lunar-monitor-injector.exe`, `lunar-monitor.dll` and `usertoolbar.txt` into the same directory as your Lunar Magic executable.

`lunar-monitor-config.txt` is an example of a configuration file for Lunar Monitor. You should put a file with this precise name in the same directory as the ROM(s) you want to automatically extract resources from. 

The contents of the example configuration file `lunar-monitor-config.txt` are:

```
level_directory: "Levels"
mwl_file_format: "level #.mwl"
flips_path: "../Tools/FLIPS/flips.exe"
map16_path: "Other/all.map16"
clean_rom_path: "../SMW_clean.smc"
global_data_path: "Other/global_data.bps"
shared_palettes_path: "Other/shared.pal"
human_readable_map16_cli_path: "../Tools/human-readable-map16-cli.exe"
log_level: Log
log_path: "Other/lunar-monitor-log.txt"
```

These should be relatively self explanatory, especially if you have used Lunar Helper before.

All paths are relative to the directory the configuration file and your ROM are in.

`level_directory` specifies the directory levels will be exported to, this should point to the same directory as the `levels` variable from Lunar Helper.

`mwl_file_format` specifies the naming for exported level files. The '#' character is a placeholder for the level number. For example, if the `mwl_file_format` is "level #.mwl" and we save level 10A it will be exported as "level 10A.mwl". You should probably leave this unchanged as it's the exact format Lunar Magic will use when exporting levels by default.

`flips_path` specifies the path to your [FLIPS](https://github.com/Alcaro/Flips) executable. This should be the same as the `flips_path` variable from Lunar Helper. NOTE: I think older FLIPS versions have a bug that sometimes results in them returning error codes despite creating/applying .bps patches correctly so be sure to get an up-to-date version if possible.

`map16_path` specifies the path the map16 file should be exported to. This should be the same as the `map16` variable from Lunar Helper.

`clean_rom_path` specifies the path to a clean Super Mario World ROM. This should be the same as the `clean` variable from Lunar Helper.

`global_data_path` specifies the path the global data .bps patch should be exported to. This patch will contain the overworld, title screen, credits and global exanimation data from your ROM. The path should be the same as the `global_data` variable from Lunar Helper.

`shared_palettes_path` specifies the path the shared palette .pal should be exported to. This should be the same as the `shared_palette` variable from Lunar Helper.

`human_readable_map16_cli_path` specifies the path to a [Human Readable Map16 Conversion Executable](https://github.com/Underrout/human-readable-map16-cli). This value is optional, if it is present, this executable will be used to convert the map16 file exported to `map16_path` into a human readable text format. These text files will be output into a directory with the same name as the .map16 file, but with the .map16 extension stripped. The directory will be in the same location as the .map16 file. The .map16 file will **not** automatically be deleted after conversion.

`log_level` specifies the type of logging that's gonna be done by the tool. There are 3 possible logging levels: Warn, Log and Silent. Warn is the noisiest one, which will pop up a message box when a warning/error is issued and log everything else to file, Log will just log everything else to a file while Silent will ignore everything.

`log_path` specifies the path where the logging messages will be written to.

Make sure you specify all the configuration variables correctly or Lunar Monitor won't export resources reliably. 

The last 2 configuration variables (`log_path` and `log_level`) can be omitted. If omitted `log_level` will default to Log and `log_path` will default to a file named `lunar_monitor_log.txt` in the same folder as your ROM.

If you're using git, excluding the log file from version control via your `.gitignore` is probably a good idea.

That should be it. Open your Lunar Magic executable, save a level, map16, shared palettes, etc. and you should see your resources automatically be extracted to the specified directories. You can use this Lunar Magic executable as you would any other program, you can make shortcuts to it, pin it to your taskbar, create a file association for it, etc. and Lunar Monitor should still work correctly.

If it doesn't seem to be working, please double check that:
- all the paths in your `lunar-monitor-config.txt` are correct
- you're actually using the Lunar Magic executable that's in the same folder as `lunar-monitor-injector.exe`, `lunar-monitor.dll` and `usertoolbar.txt`
- you're using Lunar Magic 3.30 or 3.31 and have the correct version of the tool for your Lunar Magic version

If you're certain everything's set up correctly and it's still not working please feel free to [open an issue](https://github.com/Underrout/lunar-monitor/issues/new) and let me know what isn't working as expected.

# Additional thoughts

I'd recommend a git hook that builds ROMs as soon as a commit/branch is checked out to further ensure that ROMs stay in sync with their version controlled environment.

**Never** add ROMs to your version control system. If you ever push a repo containing a ROM to a public repo you could potentially get in trouble so probably don't do that. I highly recommend adding `*.smc`, `*.sfc` and any other file extensions that could potentially contain ROMs to your `.gitignore` as soon as you start a new hacking project so you're on the safe side. You also don't need to version control the ROM since everything relevant is extracted anyway and you can always re-build your ROM using Lunar Helper.

You might be wondering if this idea of DLL injecting Lunar Magic could be used to add more complex functionality. The answer is yes, since it theoretically gives you full control over the whole application, but it may involve a lot of reverse engineering and require (advanced?) knowledge of x86 assembly. I am neither a reverse engineer nor an x86 programmer (yet?) so I probably won't attempt anything too crazy. I am still interested in the idea though and think a sort of plugin system where the injected DLL loads other DLLs could be pretty cool. Might look into this more in the future.
