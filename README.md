# BR_Linux_ManagementTool

WIP: Retrieve Chat Messages

## Building on Linux
Run:
- `meson setup --cross-file x86_64-w64-mingw32.txt build`
- `meson compile -C build`

## Tips
If running BR in a Proton container, use:
- `WINEFSYNC=1 WINEPREFIX=~/.steam/steam/steamapps/compatdata/552100/pfx/ WINEARCH=win64 ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/bin/wine64 brlmt.exe`
