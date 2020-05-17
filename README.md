## Current V8 branch / tag:
 + 8.4-lkgr / 8.4.371.3

## Prerequisites
See [v8 documentation](https://v8.dev/docs/build)

## Pre-Build

```bash
set GYP_MSVS_VERSION=2019
gn gen out/x64 --root=v8
ninja -C out/x64 d8 v8_shell
```