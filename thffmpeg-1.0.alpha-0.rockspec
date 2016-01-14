package = "thffmpeg"
version = "1.0.alpha-0"

source = {
   url = "https://github.com/MichaelMathieu/thffmpeg",
   tag = "master"
}

description = {
   summary = "FFMPEG bingings for Torch (reading only)",
   homepage = "https://github.com/MichaelMathieu/thffmpeg",
   license = "BSD"
}

dependencies = {
   "torch >= 7.0",
   "paths",
   "dok"
}

build = {
   type = "command",
   build_command = [[
cmake -E make_directory build && cd build && cmake .. -DLUALIB=$(LUALIB) -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(LUA_BINDIR)/.." -DCMAKE_INSTALL_PREFIX="$(PREFIX)" && $(MAKE)
   ]],
   install_command = "cd build && $(MAKE) install"
}