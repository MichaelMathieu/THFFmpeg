# THFFmpeg

Some (quick and not so dirty) bindings of ffmpeg for torch.
Big thanks to https://github.com/mpenkov/ffmpeg-tutorial for their tutorials.

## Install

Clone this git, then type
```
luarocks make
```

### Troubleshooting

When installing, I had a problem with the version of ffmpeg (the headers version didn't match the loaded library).
There is now a check at initialization. If it fails, make sure that you compile with the same headers.
A way to force them is
```
luarocks make
cd build
ccmake .
# set the correct paths
cd ..
luarocks make
```
After that, as long as you don't remove the `build` folder, it should compile fine with the future `luarocks make` calls.

The only way I found to set the correct paths were to add `-I/usr/include/ffmpeg` to the CXX and C flags, in `ccmake .`.
This should work on NYU servers, but other paths may be necessary depending on where you installed `ffmpeg`.