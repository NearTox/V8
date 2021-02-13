## Current V8 branch / tag:
 + 8.9-lkgr / 8.9.255.18

## Prerequisites
See [v8 documentation](https://v8.dev/docs/build)

## Pre-Build

```bash
gn gen out/x64 --root=v8
ninja -C out/x64 d8 v8_shell
```