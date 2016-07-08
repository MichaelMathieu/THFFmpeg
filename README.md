# THFFmpeg

Some (quick and not so dirty) bindings of ffmpeg for torch.
Big thanks to https://github.com/mpenkov/ffmpeg-tutorial for their tutorials.

Under BSD License.

## Install

It requires ffmpeg 2.x (tested with 2.8.5) for now.

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

### THFFmpeg:open(filename)

Opens a video file. If another was opened in the same THFFmpeg object, it is closed.
It sets the variables `h` and `w` of the THFFmpeg object to the size of the video stream.

### THFFmpeg:close()

Closes a video file. It is not usually strictly necessary, as the decoder will
automatically close the opened video when another video is opened,
or when the decoder is garbage collected.

### [res] THFFmpeg:next_frame([dst])

Decodes and returns the next frame. If `dst` is provided, it is used as a destination buffer,
and must have the correct size.
This function returns nil when there is no more frames to read.

### [res] THFFmpeg:skip_frame()

Skips a frame. Returns true if successful and false otherwise. Because the frame is not processed, it
is faster than next_frame.

### [res] THFFmpeg:seek(idx)

Seeks the `idx`-th frame. Because of limitations of video format, this function decodes
every frame from the beginning, so it might be slow on large videos. However, it will
always be faster than calling `idx` times the function `next_frame` because the
images are not processed.
This function returns `true` if the seek was a success, and `false` otherwise.

### [res] THFFmpeg:length()

Returns the length of the video. As for `seek`, this function decodes all frames of the
video and therefore might be slow on large videos.
