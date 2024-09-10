# xlog

## Overview

**XLog** is a desktop streaming and keylogging utility designed for security research and experimentation. It uses the X11 protocol to capture the desktop screen and streams it over a network socket. Additionally, it logs keystrokes for auditing purposes.

**DISCLAIMER:** This project is for educational and research purposes ONLY. Unauthorized use may result in legal consequences. Please use responsibly and only in environments where you have explicit permission.

## Features
- Capture desktop screen and stream over a socket using X11.
- Keylogger functionality for logging keystrokes.
- Lightweight, modern C implementation.

## Requirements
- **Operating System:** Linux (X11 required)
- **Libraries:**
  - X11 development libraries (`libX11-dev`)
  - POSIX sockets
  - Threads (`pthread`)

## Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/Hqnnqh/xlog.git
    cd xlog
    ```

2. Install dependencies:

    ```bash
    sudo apt update
    sudo apt install libx11-dev
    ```

3. Build the project:

    ```bash
    make
    ```

4. Run the project:

    ```bash
    ./bin/xlog [target_ip] [target_port]
    ```
