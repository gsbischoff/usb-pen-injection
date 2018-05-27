# USB Pen Injection

Uses the Windows API to slave the input of a pressure sensitive stylus from a Microsoft Surface and pipe it
over USB (or some other protocol) for input on a separate computer without a touch display. (Work-In-Progress)

The initial implementation uses the sockets API (with Winsock). UDP sockets, USB, or Bluetooth to come later.

## Contents

**PenClient.c** and **PenServer.c** contain the sources of the program to recieve and inject input; and to capture and send input over a designated protocol. Currently, these send cursor movements over TCP/IP stream sockets as "boilerplate" -- they will be modified to use pen input and send the input over other means later.

**Point.c** is for capturing Pen input using Pointer APIs. This is to be joined with **PenServer.c**.

### Testbeds

**IOTest.c** is for any scratchwork towards capturing and injecting pen input properly.

**IOFinal.c** contains only the code necessary for capturing the raw pen input using RawInput. *No longer used for testing*


## Building

The client and server are built similarly to each other:

```bash
build.bat PenClient.c
build.bat PenServer.c
```

The test files have different dependencies that I've seperated into a seperate build script placed in their own directory:

```bash
build.bat IOTest.c
build.bat IOFinal.c
```

### Notes/Current Issues

* **WM_INPUT** messages can be recieved from a pen arbitrarily using the *INPUTSINK* attribute while registering the RI device -- however, once the RAWINPUT structure is returned from **GetRawInputData()**, we cannot place it back into a handle (using **GlobalAlloc()** or **GlobalHandle()**) to reinject into another context. Ideas:

 * ~~Try examining the *HRAWINPUT* of *LPARAM* to see if we can figure out how it is different from those we can make with the Global memory management functions.~~ *Said handles are not manageable with GlobalLock(), etc.*

 * See if WinTab has APIs for injection

* Switch to using Pointer messages (can we capture all messages still as with *INPUTSINK*? Register pointers?)

 * **RegisterPointerDeviceNotifications()** allows *message-windows* to recieve inputs telling when a pointer in put in range; however, the *pointerId* that is returned from **GET_POINTERID_WPARAM()** on those messages is invalid. **ONLY** when retrieving it through a **WM_POINTERUPDATE** message can you call **GetPointerPenInfo()**. Additionally, *pointerId*s are unique, each time the pen leaves and comes back in range, a new *pointerId* is returned.

 * Must use a visible window.

 * Can we get pressure and raw pen info using RAWINPUT (for which we have INPUTSINK attribute) and reinject it using the touch injection APIs?


|*Summary*| Broadcast Globally | Inject?       | Input sink? |
|--------:|:------------------:|:-------------:|:-----------:|
|RawInput | Yes                | **No**        | Yes         |
|Pointers | *Inject touch*     | *Only touch*  | No          |
|WinTab   | ?                  | ?             | ?           |

*There exist PenInjection APIs in C++/RT, but the only thing lost with TouchInjection is the eraser's functionality.*