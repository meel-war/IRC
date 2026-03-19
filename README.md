*This project has been created as part of the 42 curriculum by [pribolzi](https://github.com/pribolzi), [meel-war](https://github.com/meel-war), [mubersan](https://github.com/mubersan).*

## Description

**ft_irc** is a fully functional Internet Relay Chat (IRC) server implementation written in C++98. The project demonstrates advanced network programming concepts, including socket management, multiplexing with `poll()`, and the IRC protocol.

The server supports multiple simultaneous client connections, implements core IRC commands, and features channel management with various modes. It includes a custom bot that broadcasts messages to active channels at regular intervals.

### Key Features
- Multi-client support using `poll()` for I/O multiplexing
- Non-blocking socket operations with `fcntl()`
- Full IRC authentication flow (PASS/NICK/USER)
- Channel management with operators and modes (i, t, k, l, o)
- Private messaging between users and channels
- Built-in bot with periodic automated messages
- Proper handling of partial messages with buffering system

---

## Instructions

### Prerequisites
- C++ compiler supporting C++98 standard (g++ or clang++)
- Make
- A UNIX-like operating system (Linux, macOS)

### Compilation
```bash
make
```

This will produce an executable named `ircserv`.

### Execution
```bash
./ircserv <port> <password>
```

**Parameters:**
- `port`: The port number on which the server will listen (must be between 1024 and 65535)
- `password`: The connection password required for clients to authenticate

**Example:**
```bash
./ircserv 6667 secret_password
```

### Connecting with a Client

You can connect using any IRC client (irssi, hexchat, WeeChat, etc.) or even netcat for testing:

**With irssi:**
```bash
irssi
/connect localhost 6667 secret_password
/nick YourNickname
/join #general
```

**With netcat (for testing):**
```bash
nc -C localhost 6667
PASS secret_password
NICK Alice
USER alice 0 * :Alice Wonderland
JOIN #general
PRIVMSG #general :Hello everyone!
```

### Supported Commands

| Command | Description |
|---------|-------------|
| `PASS <password>` | Authenticate with the server |
| `NICK <nickname>` | Set your nickname |
| `USER <username> 0 * :<realname>` | Complete registration |
| `JOIN <#channel>` | Join a channel |
| `PART <#channel> [reason]` | Leave a channel |
| `PRIVMSG <target> :<message>` | Send a message to user or channel |
| `KICK <#channel> <user> [reason]` | Kick a user from channel (operators only) |
| `INVITE <user> <#channel>` | Invite a user to a channel |
| `TOPIC <#channel> [new topic]` | View or set channel topic |
| `MODE <#channel> [+/-][i\|t\|k\|l\|o]` | Manage channel modes |
| `PING <token>` | Test connection |
| `QUIT [reason]` | Disconnect from server |

### Channel Modes

- `i` - Invite-only channel
- `t` - Topic restricted to operators
- `k` - Channel requires a key (password)
- `l` - User limit on channel
- `o` - Give/take channel operator privileges

---

## Project Structure
```
ft_irc/
├── server/
│   ├── Server.hpp          # Server class declaration
│   ├── Server.cpp          # Server implementation
│   └── Commands.cpp        # IRC commands implementation
├── client/
│   ├── Client.hpp          # Client class declaration
│   └── Client.cpp          # Client implementation
├── channel/
│   ├── Channel.hpp         # Channel class declaration
│   └── Channel.cpp         # Channel implementation
├── main.cpp                # Entry point
├── Makefile
└── README.md
```

---

## Technical Choices

### Non-Blocking I/O
The server uses `fcntl()` to set all sockets to non-blocking mode (`O_NONBLOCK`). This ensures that a slow client never blocks the entire server and allows handling hundreds of concurrent connections efficiently.

### poll() for Multiplexing
We chose `poll()` over `select()` for better performance with large numbers of connections and no limit on file descriptors.

### Message Buffering
Each client maintains a buffer to handle partial IRC messages. Since TCP is a stream protocol, messages ending with `\r\n` may arrive fragmented across multiple `recv()` calls.

### Bot Implementation
The bot is implemented as a virtual client (fd = -1) without an actual socket connection. It automatically joins all channels and broadcasts messages every 60 seconds.

---

## Resources

### IRC Protocol Documentation
- [RFC 1459 - Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459) - Original IRC specification
- [RFC 2812 - Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812) - Updated IRC client protocol
- [Modern IRC Documentation](https://modern.ircdocs.horse/) - Comprehensive modern IRC reference

### Network Programming
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) - Comprehensive socket programming guide
- [poll() man page](https://man7.org/linux/man-pages/man2/poll.2.html) - Linux manual for poll()
- [fcntl() man page](https://man7.org/linux/man-pages/man2/fcntl.2.html) - File control operations

### C++ References
- [C++98 Standard](https://www.lirmm.fr/~ducour/Doc-objets/ISO+IEC+14882-1998.pdf) - Official C++98 specification
- [CPP Reference](https://en.cppreference.com/) - C++ language reference

### Testing Tools
- [irssi](https://irssi.org/) - Terminal-based IRC client
- [HexChat](https://hexchat.github.io/) - Graphical IRC client
- [nc (netcat)](https://man7.org/linux/man-pages/man1/ncat.1.html) - Network debugging tool

### AI Usage
AI assistance (Claude by Anthropic) was used throughout the project as a debugging and review tool: identifying segfaults and valgrind warnings (uninitialized `pollfd` fields, missing `return` after error checks), understanding IRC compliance requirements (numeric replies such as `001 RPL_WELCOME` needed for irssi to establish a connection), spotting logic bugs in command implementations, and advising on socket configuration (`SO_REUSEADDR`, non-blocking I/O). All architecture decisions, feature implementation, and final code were written and validated by the project authors.

---

## Authors

- **pribolzi** - Server infrastructure, socket management, poll() implementation, bot system
- **meel-war** - IRC command implementation, protocol handling
- **mubersan** - Client and Channel class design and implementation

---

## License

This project is part of the 42 School curriculum and is intended for educational purposes only.
