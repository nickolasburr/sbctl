# sbctl

`sbctl` is a macOS serial bus controller.

## Installation

There are two installation methods: Homebrew and manual.

### Homebrew

```
brew tap nickolasburr/pfa
brew install sbctl
```

### Manual

```
make
make install
```

## Commands

+ `list`, `ls`: List all serial devices.
+ `get`: Get information for specific bus, device[s].
+ `set`: Update properties of specific bus, device.
