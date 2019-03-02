## Current V8 branch / tag:
 + 7.3-lkgr / 7.3.492.19

## Prerequisites
See [v8 documentation](https://v8.dev/docs/build)

## Build

```bash
gn gen out --root=v8
cd out
ninja -C . v8_monolith
```