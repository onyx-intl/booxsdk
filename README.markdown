

# The Onyx Boox SDK

## Introduction

The Onyx Boox SDK is used for developing applications on the Boox
ebook readers produced by Onyx International.

## Development environment

The toolchain used for cross-compiling can be obtained at
`https://github.com/onyx-intl/toolchain`. This repository contains toolchains
for various processors. For the Onyx Boox M92 use `imx508`. Extract the two
archives `gcc-4.4.4-glibc-2.11.1-multilib-1.0.tar.gz` and `sdk_imx508.tar.gz` to
`/opt`.

## Building the SDK

Grab the source:

    git clone git@github.com:onyx-intl/booxsdk.git
    git submodule update --init

To build the SDK, install CMake and Rake, then run

    rake build:arm:default

CCache and DistCC will be used if they are available. If you do not
want to use DistCC, append `DISABLE_DISTCC=1` to the build command.

The SDK is frequently updated and sometimes may break compatibility
with earlier versions. If you want your application to run no matter
which version of the libraries are installed on the device, you can
link your application against the static libraries. You can build the
static libraries by running

    rake build:arm:static

## Notes for 64-bit hosts

If you are running a 64-bit environment, please make sure you have
32-bit runtime libraries installed. (For example, on Arch Linux, it is `multilib/lib32-gcc-libs`.)


# Open source packages

Please visit http://opensource.onyx-international.com/ to download all source packages.

