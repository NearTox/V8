## Current V8 branch / tag:
 + 7.5-lkgr / 7.5.288.22

## Prerequisites
See [v8 documentation](https://v8.dev/docs/build)

## Pre-Build

```bash
gn gen out/x64 --root=v8
ninja -C out/x64 d8 v8_shell
```