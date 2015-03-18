#!/bin/sh

# Disable leak checking in Address Sanitizer.
# This is done because Qt causes a lot of useless leaks to be caught.
# Unfortunately, I haven't found a way to ignore those particular warnings.
export ASAN_OPTIONS=detect_leaks=0

# Enable Mesa debugging (catches OpenGL errors, etc.)
export LIBGL_DEBUG=1
export MESA_DEBUG=1

while getopts "soi" OPTION ; do
    case $OPTION in
        s)
            # Use software rendering (needed if running without a screen).
            # For automated testing.
            export LIBGL_ALWAYS_SOFTWARE=1
            export SOFTPIPE_USE_LLVM=1
            ;;
        o)
            # Override GL/GLSL version so that software rendering supports 3.2.
            # This is iffy and I'm not positive that it will be robust.
            # However, when running _with_ a screen, software claims to provide
            # 3.3. So hopefully this will be okay.
            export MESA_GL_VERSION_OVERRIDE=3.2
            export MESA_GLSL_VERSION_OVERRIDE=150
            ;;
        i)
            # Add an argument which tells the program to render one frame into
            # an image and then exit. For automated testing.
            export CIS277_AUTOTESTING=1
            ;;
    esac
done

exec build-asan/277
