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

+ `list`, `ls`: List all known serial devices.
+ `get`, `show`: Get information about device.
+ `set`: Set, modify properties of device.
+ `unset`, `rm`: Remove properties of device.
+ `[--]help`: Show usage information.
+ `[--]version`: Show current release version.

## Options

Options are command-specific and can be found via `sbctl <COMMAND> [-h|--help]`.

## Examples

#### List all known USB devices.

`sbctl ls --usb`

```
 01    usb    usb    ---    029   000       ---    0            480            ---             000          Apple    Root Hub Simulation
 02    usb    usb    ---    026   000       ---    0            480            ---             000          Apple    Root Hub Simulation
 03    usb    usb    ---    029   001       01     250          480            ---             000          ---      ---
 04    usb    usb    ---    026   001       01     250          480            ---             000          ---      ---
 05    usb    usb    ---    029   002       08     250          480            ---             000          ---      ---
 06    usb    usb    ---    029   004       01     250          12             ---             016          Apple    BRCM20702 Hub
 07    usb    usb    ---    029   005       03     250          12             ---             048          Apple    Apple Internal Keyb
 08    usb    usb    ---    029   007       03     250          12             ---             019          Apple    Bluetooth USB Host
 09    usb    usb    ---    026   002       02     250          480            ---             000          ---      ---
 10    usb    usb    ---    026   003       01     250          480            ---             016          ---      ---
 11    usb    usb    ---    026   004       03     250          480            ---             048          ---      ---
 12    usb    usb    ---    026   005       02     250          480            ---             032          ---      ---
 13    usb    usb    ---    026   006       04     250          480            ---             064          ---      ---
 14    usb    usb    ---    026   007       02     250          480            000000000000    018          Apple    Keyboard Hub
 15    usb    usb    ---    026   008       03     250          12             KFV49920        019          Matrox   DualHead2Go SE
 16    usb    usb    ---    026   009       04     250          12             ---             052          Logite   USB Receiver
 17    usb    usb    ---    026   010       02     250          1.5            ---             018          Apple    Apple Keyboard
 18    usb    usb    ---    029   003       02     250          1.5            ---             032          Apple    IR Receiver
```

#### List all known devices with Thunderbolt support.

`sbctl ls --thun`

```
 19    pci    thun   port   006   ---       06     ---          10000          ---             021          ---      Thunderbolt Native
 20    pci    thun   port   006   ---       01     ---          10000          ---             021          ---      Thunderbolt Port
 21    pci    thun   port   006   ---       03     ---          10000          ---             021          ---      Thunderbolt Port
 22    pci    thun   port   006   ---       01     ---          10000          ---             021          ---      Thunderbolt Port
 23    pci    thun   port   006   ---       02     ---          10000          ---             021          ---      Thunderbolt Port
 24    pci    thun   port   006   ---       04     ---          10000          ---             021          ---      Thunderbolt Port
 25    pci    thun   port   006   ---       06     ---          10000          ---             021          ---      PCIe Adapter
 26    pci    thun   port   006   ---       07     ---          10000          ---             021          ---      PCIe Adapter
 27    pci    thun   port   006   ---       08     ---          10000          ---             021          ---      PCIe Adapter
 28    pci    thun   port   006   ---       10     ---          10000          ---             021          ---      DP or HDMI Adapter
 29    pci    thun   port   006   ---       02     ---          10000          ---             021          ---      Thunderbolt Port
 30    pci    thun   port   006   ---       07     ---          10000          ---             021          ---      PCIe Adapter
 31    pci    thun   port   006   ---       08     ---          10000          ---             021          ---      PCIe Adapter
 32    pci    thun   port   006   ---       09     ---          10000          ---             021          ---      PCIe Adapter
 33    pci    thun   port   006   ---       10     ---          10000          ---             021          ---      PCIe Adapter
 34    pci    thun   port   006   ---       12     ---          10000          ---             021          ---      DP or HDMI Adapter
 35    pci    thun   port   006   ---       13     ---          10000          ---             021          ---      DP or HDMI Adapter
 36    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci8086,154
 37    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 38    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      display
 39    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci8086,1e31
 40    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci8086,1e3a
 41    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci8086,1e2d
 42    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci8086,1e20
 43    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 44    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 45    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 46    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci8086,1e26
 47    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci8086,1e03
 48    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci8086,1e22
 49    pci    thun   brid   001   ---       ---    ---          ---            ---             ---          ---      ethernet
 50    pci    thun   brid   000   ---       ---    ---          ---            ---             ---          ---      pci8086,1e57
 51    pci    thun   brid   002   ---       ---    ---          ---            ---             ---          ---      pci14e4,4331
 52    pci    thun   brid   003   ---       ---    ---          ---            ---             ---          ---      pci11c1,5901
 53    pci    thun   brid   001   ---       ---    ---          ---            ---             ---          ---      pci14e4,16bc
 54    pci    thun   brid   004   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 55    pci    thun   brid   005   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 56    pci    thun   brid   005   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 57    pci    thun   brid   005   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 58    pci    thun   brid   005   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 59    pci    thun   brid   005   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 60    pci    thun   brid   006   ---       ---    ---          ---            ---             ---          ---      pci8086,1513
 61    pci    thun   brid   191   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 62    pci    thun   brid   192   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 63    pci    thun   brid   192   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 64    pci    thun   brid   192   ---       ---    ---          ---            ---             ---          ---      pci-bridge
 65    pci    thun   brid   193   ---       ---    ---          ---            ---             ---          ---      pci10de,fb9
 66    pci    thun   brid   193   ---       ---    ---          ---            ---             ---          ---      display
 67    pci    thun   swit   006   ---       06     ---          10000          ---             021          Apple    MacBook Pro
 68    pci    thun   swit   006   ---       03     ---          20000          ---             021          inXtr    AKiTiO Thunder 2 Bo
```

#### Get device at index 15

`sbctl get %15`

```
 DualHead2Go SE

 Spec: usb
 Mode: usb
 Type: ---

 Bus: 026
 Address: 008
 Port: 03
 Power: 250mA
 Speed: 12Mbps [Full]

 Serial: KFV49920
 Device ID: 19
 Vendor: Matrox
```
