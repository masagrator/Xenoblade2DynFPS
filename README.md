# Xenoblade2DynFPS
exlaunch plugin that is taking naive approach at adjusting game speed dynamically.

Main points of using this plugin:
- Gameplay speed is adjusted to framerate on frame-to-frame basis without any smoothing implemented
- Cutscenes are automatically locked to 30 FPS
- UI speed is adjusted to framerate
- Foliage movement is adjusted to framerate
- Gameplay dynamic resolution GPU rendering frametime was replaced by CPU frametime with some tweaks to match better dynamic resolution expectations 

Known issues that probably will never be fixed:
- Game has big issues with fluid asset streaming, so it can affect stability of speed calculation at higher framerates
- When using FPSLocker with Window Sync set to Semi, game slows down up to 20% depending on FPS
- Billboards speed like mist and fire is still tied to framerate, this is probably calculated on shader side

It should work with yuzu and ryujinx, but it was not tested. It has some skeleton to support 120 FPS game speed adjustments, but I guess they would need refining from somebody who knows how to achieve 120 FPS in Xenoblade Chronicles 2 without breaking game.
