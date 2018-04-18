# USB Pen Injection

Used the Windows API to slave the input of a pressure sensitive stylus from a Microsoft Surface and pipe it
over USB (or some other protocol) for input on a separate computer without a touch display.

The initial implementation uses the sockets API (with Winsock).