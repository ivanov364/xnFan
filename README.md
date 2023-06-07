# xnFan
xnFan - is an linux module which can help you to change your computer fan speed and color

## Disclaimer

I don't take any risk of your pc done by this program.

## Requirements

Kernel: 6.2.x or higher

## How to build

```
make

```

## How to use

### Help command

```
xnFan -h
```

### To change speed:
> There's no warnings about fan speed limit. Use it on your own risk.
> 
> Don't put it too high though.
```
xnFan -s <speed>
```

<hr>

### To change color:
> Use it on your own risk.
```
xnFan -c <color>
```
color list:
- orange
- green
- yellow
- white
- red
- purple

## How it works

### Short summary

setting up GPIO and PWM; creating driver and sysfs file.

### Long answer

It works by PWM (Pulse Width Modulation) it uses a rectangular signal to vary power delivered to a fan to control its speed and by GPIO (General Purpose Input/Output) it helps you interface physical devices with your Raspberry Pi's digital pins to control their behavior. Then we create linux driver and Sysfs. Sysfs is a virtual file system in Linux that allows the kernel to export information about hardware to user space.

## Tests

Tested on:
- RGB-FANS: RF120
- OS: Artix Linux x86_64
- Kernel: 6.2.6-artix1-1
