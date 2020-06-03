# Fake IMU Simulator for Autoware

## Instructions

```

1. Make build this project.

```

cd fake_imu_simulator
make

```

2. Create virtual serial ports

```

socat -d -d pty,raw,echo=0 pty,raw,echo=0

```

The output will be like this.
One of PTYs is used for [tamagawa_imu_driver](https://github.com/tier4/AutowareArchitectureProposal/tree/master/src/sensing/driver/tamagawa_serial_driver).<br>
Another is used for Fake IMU Simulator.

```

2020/05/22 17:29:10 socat[42184] N PTY is /dev/pts/0
2020/05/22 17:29:10 socat[42184] N PTY is /dev/pts/2
2020/05/22 17:29:10 socat[42184] N starting data transfer loop with FDs [5,5] and [7,7]

```

3. Launch tamagawa_imu_driver.

   Change serial port to use PTY before running a launch file.

4. Run Fake IMU Simulator.

```

./fake_imu_simulator

```

Enter device name, and choose a log file.
![window](docs/images/window.png)

Then, turn on the switch of `Serial Port` to open PTY serial port and transmit data.

### <u>Checksum error</u>

If you intend to generate checksum error, turn on the switch of `Checksum error`.<br>
The checksum value will be `??`.

### <u>Debug output</u>

If you want to see transmission data, turn on the switch of `Debug output`.
```
