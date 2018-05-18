# USB Pen Injection

Uses the Windows API to slave the input of a pressure sensitive stylus from a Microsoft Surface and pipe it
over USB (or some other protocol) for input on a separate computer without a touch display. (Work-In-Progress)

The initial implementation uses the sockets API (with Winsock). USB or Bluetooth to come later.

## Contents

**PenClient.c** and **PenServer.c** contain the sources of the program to recieve and inject input; and to capture and send input over a designated protocol. Currently, these send cursor movements over TCP/IP stream sockets as "boilerplate" -- they will be modified to use pen input and send the input over other means later.

**IOTest.c** is for any scratchwork towards capturing and injecting pen input properly.

## Building

The client and server are built similarly to each other:

```bash
build.bat PenClient.c
build.bat PenServer.c
```

The test file has different dependencies that I've seperated into a seperate build script:

```bash
build1.bat IOTest.c
```