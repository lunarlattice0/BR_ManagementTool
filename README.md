# BR_Linux_ManagementTool

A tool for assisting 24/7 server OPs of Brick Rigs. Currently not restricted to Linux systems, contrary to original plan.

Working Features:
- End / Restart matches
- Reset all player positions

TODO:
- Restart individual rounds
- Destroy vehicles
- Kill players
- Send Chat Messages
- Kick Players
- Ban Players
- Retrieve Chat Messages
- Start the server automatically

## Building on Linux
Run:
- `meson setup --cross-file x86_64-w64-mingw32.txt build`
- `meson compile -C build`

## Tips
If running BR in a Proton container, use:
- `WINEFSYNC=1 WINEPREFIX=~/.steam/steam/steamapps/compatdata/552100/pfx/ WINEARCH=win64 ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/bin/wine64 brlmt.exe`
