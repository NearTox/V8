## Current V8 branch:
 + 10.4-lkgr

## Prerequisites
See [v8 documentation](https://v8.dev/docs/build)

## Pre-Build

```bash
gn gen out/x64 --root=v8
ninja -C out/x64 d8 v8_shell
```