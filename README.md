Note (to myself and NYU students):
I had to add -I/usr/include/ffmpeg to the CXX and C flags, in ccmake . in build, after running luarocks make, to compile with the version it links to.