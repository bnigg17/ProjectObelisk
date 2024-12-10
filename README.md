# ECE395
## Parts and Documentation
ARM M7 Cortex Processor: STM32H7A3ZIT6Q (https://www.st.com/resource/en/datasheet/stm32h7a3ai.pdf)

Camera: OV7670 (https://web.mit.edu/6.111/www/f2016/tools/OV7670_2006.pdf, https://www.electronicscomp.com/datasheet/ov7670-sensor-datasheet.pdf)

## Helpful Links
Connecting USB to WSL (https://learn.microsoft.com/en-us/windows/wsl/connect-usb)

How HAL DCMI Works (https://www.st.com/resource/en/product_training/STM32F7_Peripheral_DCMI.pdf)

Use this command to find out what linux serial port the UART device is connected to.
```bash
dmesg | grep tty
```
Turn on Raspbeery PI python program on startup
```bash
sudo nvim /etc/systemd/system/myscript.service
```
Modify file
```bash
[Unit]
Description=My Python Script
After=network.target

[Service]
ExecStart=/usr/bin/python3 /path/to/your_script.py
WorkingDirectory=/path/to/
StandardOutput=inherit
StandardError=inherit
Restart=always
User=pi

[Install]
WantedBy=multi-user.target

```
```bash
sudo systemctl enable myscript.service
sudo systemctl start myscript.service
sudo systemctl status myscript.service
```

To Reload and Restart
```bash
sudo systemctl daemon-reload
sudo systemctl restart myscript.service
```

Turn off Raspberry PI python program on startup
```bash
sudo systemctl stop myscript.service
sudo systemctl disable myscript.service
sudo rm /etc/systemd/system/myscript.service
sudo systemctl daemon-reload
systemctl list-unit-files | grep myscript
```

