# ft_irc

A lightweight Internet Relay Chat (IRC) server written in C++, developed as part of the 42 curriculum.

## 📌 Overview

**ft_irc** is a custom implementation of an IRC server based on the specifications of the IRC protocol defined in RFC standards such as RFC 1459 and RFC 2812.

The goal of this project is to understand how real-world network servers work by building one from scratch, handling multiple clients, and implementing core IRC features.

## ⚙️ Features

* TCP server using IPv4
* Non-blocking I/O with `poll()`
* Multi-client handling
* User authentication (`PASS`, `NICK`, `USER`)
* Channel management (`JOIN`, `PART`, `TOPIC`)
* Messaging (`PRIVMSG`, `NOTICE`)
* Operator commands (`KICK`, `INVITE`, `MODE`)
* Proper disconnection handling
* RFC-compliant message parsing

## 🧠 Concepts Covered

* Socket programming
* Event-driven architecture
* Network protocols (IRC)
* Multiplexing (`poll`)
* Parsing and buffering
* Client-server communication

## 🛠️ Technologies

* Language: C++
* Standard: C++98 (as required by 42)
* System calls: `socket`, `bind`, `listen`, `accept`, `poll`, `recv`, `send`, `close`

## 🚀 Getting Started

### Prerequisites

* Unix-based system (Linux / macOS)
* C++ compiler (`g++`)
* Make

### Installation

```bash
git clone https://github.com/yourusername/ft_irc.git
cd ft_irc
make
```

### Usage

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 mypassword
```

## 🔌 Connecting to the Server

You can connect using:

* Netcat:

```bash
nc localhost 6667
```

* IRC client (recommended):

  * HexChat
  * WeeChat
  * irssi

Then authenticate:

```
PASS mypassword
NICK yournick
USER youruser 0 * :Real Name
```

## 💬 Example Commands

```
JOIN #channel
PRIVMSG #channel :Hello everyone!
PART #channel
```

## 🧪 Testing

* Test multiple simultaneous clients
* Try edge cases (invalid commands, disconnects)
* Use a real IRC client for better validation

## 📁 Project Structure

```
.
├── src/
├── include/
├── Makefile
└── README.md
```

## ⚠️ Challenges

* Strict adherence to IRC protocol format
* Handling partial and fragmented messages
* Managing multiple clients efficiently
* Avoiding blocking operations
* Debugging network behavior

## 📚 References

* RFC 1459 - Internet Relay Chat Protocol
* RFC 2812 - IRC Client Protocol

