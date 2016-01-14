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

### Testing

To test the install, you can run
```
th test/test.th
```

## Usage

In torch:
```
require 'thffmpeg'

decoder = THFFmpeg() --creates a decoder object
decoder:open('myvideo.avi') --opens the video myvideo.avi
frame1 = decoder:next_frame() --allocates a new tensor and puts the first frame in it
frame2 = torch.Tensor(42,42)
decoder:next_frame(frame2) --resizes frame2 if necessary and puts the second frame in it
decoder:close() --closes the video
decoder:open('myvideo2.avi') --opens the video myvideo2.avi
```

`next_frame` returns nil when there is no more frames to read. Unfortunately, there
is no way to know the number of frames beforehand, apparently the best we can do
is an estimate (due to limitations of the video codecs).

Note that closing the video is not strictly necessary, as the decoder will automatically close
it when another video is opened, or when the decoder is garbage collected.
