# Xenoblade2DynFPS
exlaunch plugin for Xenoblade Chronicles 2 that is taking naive approach at adjusting game speed dynamically.

It's adjusted for Western release, version 2.1.0

Main points of using this plugin:
- Gameplay speed is adjusted to framerate on frame-to-frame basis without any smoothing implemented
- Cutscenes are automatically locked to 30 FPS
- UI speed is adjusted to framerate
- Foliage movement is adjusted to framerate
- Gameplay dynamic resolution GPU rendering frametime was replaced by CPU frametime with some tweaks to match better dynamic resolution expectations. Cutscenes still rely on GPU frametime because at 30 FPS it gives better results. 

Known issues that probably will never be fixed:
- Game has big issues with fluid asset streaming, so it can affect stability of speed calculation at higher framerates (> 50 FPS)
- Billboards speed like mist and fire is still tied to framerate, this is probably calculated on shader side

For **EMULATORS** it is recommended to use this release:<br>
https://github.com/masagrator/Xenoblade2DynFPS/releases/tag/1.0.0-CLEAN <br>
It has some skeleton to support 120 FPS game speed adjustments, but I guess they would need refining from somebody who knows how to achieve 120 FPS in Xenoblade Chronicles 2 without breaking game.

Plugin also ties internal vSync with nvn interval, so at default game runs at 60 FPS with this plugin. You can use FPSLocker or emulator settings to lock it back to 30 FPS. Don't lower res in lib_nx.ini, it will make your experience worse since whenever CPU will have an issue with maintaining stable framepacing, it will drop res to lower possible until framepacing will stabilize. And in some sceneries it's not possible without big bump in RAM clocks. Lowering res in most cases doesn't help enough to justify doing this.

If you are compiling this plugin, use `main.npdm` from root of repo instead from `deploy` folder.

# How to install
Copy folder `0100E95004038000` to `atmosphere/contents/` on sdcard. Run game
