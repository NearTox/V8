## Current V8 branch / tag:
 + 7.3-lkgr / 7.3.492.19

## Prerequisites
See [v8 documentation](https://v8.dev/docs/build)

## Pre-Build

```bash
gn gen out/x64 --root=v8
ninja -C out/x64 v8_monolith
```