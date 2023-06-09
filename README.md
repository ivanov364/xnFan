# xnFan
xnFan - is an linux module which can help you to change your computer fan speed and color

## Disclaimer

I am not responsible for any damages this kernel module may cause after being acquired.

## Requirements

Kernel: 6.2.x or higher

## How to build

On Arch Linux:
```
sudo pacman -S gcc kmod linux-headers
```

Then:
```
make
```

## Tests

Tested on:
- RGB-FANS: RF120
- OS: Artix Linux x86_64
- Kernel: 6.2.6-artix1-1
