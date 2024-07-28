# f/term F256e

## Overview

f/term is a terminal emulator for the [Foenix F256jr and F256K retro computers](http://wiki.f256foenix.com/index.php?title=Main_Page). This version is for the "expanded memory" version of the F256, and is not compatible with the classic MMU-based configuration and the Microkernel. If you do not own a Foenix computer, or if you don't know what one is, this software should have limited interest for you. Go back now before you are sucked into the world of programming and using 8-bit and 16-bit computers as if it was the 1980s. You have been warned. 

If you are a brand new F256 owner, congratulations! It's a great piece of hardware, and ton of fun to program. If you haven't already, jump on the Discord and share what you are working on.

#### Features

- text-only terminal communications via serial port and WIFI 232. 
- support for much of the ANSI protocol. No support for blinking, 8-bit color, or other features not compatible with the Foenix hardware. 

#### Coming Soon
- YMODEM download capability

## Using f/term

### Starting f/term

Until the F256K2 "toolbox" / MCP is ready, f/term is most easily started by blasting it into RAM over the USB debug port. For example:
`fnxmgr.py --run-pgz bin/fterm.pgz`

Note: f/term starts up in communications mode. That is to say: it is ready to talk to your modem immediately.

### Requirements

- f/term is a native 65816 application, it will not run on an F256 equipped with a 65C02. You can swap in a 65816, however. 
- f/term requires an F256K or Jr configured with the new "extended memory" (e) FPGA load. This includes F256K2s, if you have configured it for the e mode. Note: f/term relies extensively on interrupts to handles the data coming into the serial port. F256s with the "classic" (c) FPGA configuration are generally configured to run the Microkernel. The Microkernel has its own interrupt routines and does not take kindly to interlopers. In theory, f/term could be forked to have a version that is compatible with the classic MMU model but not run under the Microkernel. In practice, however, no one would probably configure their machine with that configuration. 
- You must have an F256Jr or K with a serial port (all Ks come with a dedicated serial port). 
- You must have a Wifi RS-232 modem. It shouldn't matter what the "brand" is, but obviously you need to have the right cabling//interface for the F256 serial port. One example:
  - WiModem232 Pro - https://www.cbmstuff.com


### Understanding the f/term screen

<pre>                                                                                      
  ┌────────────────────────────────────────────────────────────────┐                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │  <- Terminal Area
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  ├─f/term────────────────────────────9600───────2024-07-28─11:52──┤  <- Status Line  
  │                                                                │                  
  │                                                                │                  
  │                                                                │                  
  │                                                                │  <- Message Area 
  │                                                                │                  
  └────────────────────────────────────────────────────────────────┘                  
</pre>

#### The Terminal Area

The top 24 rows of the screen are dedicated to displaying the information received over the serial port. This provides 80x24 for ANSI terminals. 

#### The Status Line

The Status Line sits between the T;erminal Area and the message area, near the bottom of the screen. 

The Status Line mainly provides a visual break between information coming from the external source (the terminal area) and information coming from your computer (the Message Area). It also displays the time/date and the current serial port speed. 

#### The Message Area

The Message Area is a scrolling area at the bottom of the screen, containing messages from the f/term application, to you. The area above the status line contains communications from the BBS or remote service you are connected to, but below the status line it is only messages from f/term itself. Typically, these will be feedback about actions you have taken, such as changing serial port speed, or error messages. 


### Connecting to a BBS

#### One-time Setup

Before you can go online with your Foenix, you need to configure your modem to talk to your WiFi network. The process for that is beyond the scope of this document. Please check the instructions that came with your modem. 

After you have the modem connected to your WiFi network, you may wish to configure some quick dial #s. This is also beyond the scope of this document, and will vary depending on your modem manufacturer. What won't vary is how nice it is to have shortcut names for long BBS "numbers"!

#### Understanding Modem and Port Speed

Modems (and serial ports) communicate a number of fixed speeds, measured in "bauds", which is an Americanization of the old English term "bawds", referring to the density of plague-carrying rats in a given neighborhood. Higher numbers are faster, but your computer will need to work harder to achieve them.

The modem has a speed setting. Some actual modems only had 1 fixed speed. Some could switch between say 300 and 1200 baud with a physical switch on the device. Modern WiFi 232 modems will accept a command over the serial port to set the speed. But the modem can't take the command if the serial port isn't already talking at the speed the modem is currently using. And once you tell the modem to operate a different speed, voilá! It can no longer communicate with the computer, because now they are talking at different speeds. Each modem will have a reset speed (for example, 300 baud), so if you get really stuck, you can always pull the plug and try again. 

Here is the process for changing the modem speed:
1. Find out what speed the modem is currently on. Some WiFi modems come with a screen that shows the speed. Look for "300", "1200", "19200", etc. 
1. With the modem connected to your Foenix, run f/term
1. Use the Alt-1 through Alt-0 keys to select a matching port speed. In other words, tell the Foenix what speed the modem is communicating at. 
1. Test the speed setting by typing "at" and hitting ENTER. The modem should send back the message "OK" to the screen. If it doesn't, double check the modem and port speeds. 
1. Type in the new speed on the Foenix and hit ENTER. The exact command will vary by manufacturer, so check your manual. For example, my modem is configured with the "AT*Bnnnn" command, where "nnnn" is a baud (speed). e.g, 'at&b1200" tells the modem to switch to 1200 baud. Make sure you get an "OK" from the modem after you enter the command. 
1. Very important: now use the ALT-# key on your Foenix to match the new speed. Confirm the Status Bar shows the same speed as the modem. Confirm by typing "at" and ENTER again, checking that you get back "OK". If you don't get an "OK", double-check the modem and port speeds again. 


#### Selecting a BBS

It may not be the heydey of the BBS in the 80s and early 90s, but there are a lot of BBSes out there, and more coming online every month. The great thing is, each will be different. Some run the same software, but configure it differently. But all, in general, have a different set of regular users. Some have deep history going back to the 80s, some are brand new. Many are in English, but many are also in French, Polish, German, Japanese, etc. Some are organized loosely around one type of machine or software. 

Try a Google search, an AI search, or browse this interactive list of BBSes: [Telnet BBS Guide](https://www.telnetbbsguide.com)

Do not despair if the one you want to connect to doesn't answer. It may be temporarily offline, or it may just have exceeded the number of "calls" it can take, etc. Try again later. 

#### Going Online

Assuming you have a modem connected to your WiFi network, and you have the modem and Foenix communicating at the same speed, you are ready to go online!

To connect to a BBS, you need to know the URL of the BBS. It will generally be in the format the-bbs-url:port. For example, here is the official Foenix BBS: `bbs.foenixnet.com:256`

Let's try connecting:
1. Type "atdt bbs.foenixnet.com:256" and hit ENTER. You should see a few odd characters, then a message about connecting. 
2. If you are prompted about whether your computer supports ASCII or ANSI, you can select either one. ASCII will be faster, but you won't get colors and text graphics. ANSI will be a little slower, but many BBSes support pretty dynamic interfaces powered by ANSI, so it's definitely worth at least trying it out. 
3. You will generally be asked for a username and password. Many BBSes are setup to allow for guests, so try GUEST/guest as username and password if you don't want to register immediately. Also, never enter any of your real usernames or passwords. The BBS systems around today are either the exact same thing they were in the 1980s, or a lightly upgraded version compatible with more modern computers: security standards will be practically non-existant. 

### Controlling the Online Experience

#### Setting the Baud Rate

We already touched on how to change the baud (speed) rate, but here are the specific keys:
- ALT-1: Set baud 300
- ALT-2: Set baud 1200
- ALT-3: Set baud 2400
- ALT-4: Set baud 3600
- ALT-5: Set baud 4800
- ALT-6: Set baud 9600. This speed should be reliable.
- ALT-7: Set baud 19200. This speed may produce some intermittent serial errors which will cause a few dropped characters. Go down to 9600 if the drops are a problem.
- ALT-8: Set baud 38400
- ALT-9: Set baud 57600
- ALT-0: Set baud 115200. There is no way this will work. :)

#### Changing the Text Color

If you are connected to an ANSI BBS, it will be controlling the color of text. When connected to an ASCII-only BBS, however, you may wish to override the default light gray text. You can cycle through the available colors using the ALT-C key. Note that if you subsequently connect to an ANSI BBS, the chances are close to 100% that it will pick its own colors. 

#### Switching Fonts

f/term comes with 4 built-in fonts that you can use to customize your BBS experience. The fonts differ not only in the shape of the letters, but in the what letters are in which positions. Unfortunately, as an 8-bit computer, there is no support for Unicode's 65K+ characters: we are limited to 256 unique characters. Some shapes that exist in one "charset" may not have an equivalent in other characters sets. For example, ANSI supports musical note symbols, while the standard Foenix character sets do not. Conversely, the Foenix fonts support a much richer set of progressively shaded, or dithered, characters. The arrangement for ANSI compatibility is different from that for standard Foenix fonts, and the Japanese (JIS X 0201) arrangement is different again. You may find a use for only 1, or for all. To switch between fonts, use one of the commands below:

- ALT-F: Standard Foenix font (but bolded).
- ALT-J: Japanese kana font, non-ANSI.
- ALT-A: ANSI characters drawn in the Foenix style
- ALT-I: ANSI characters drawn in the original IBM PC style.

Note: to type in Japanese, you will also need to switch to the Japanese key layout, which you can do with FOENIX-F7. 

