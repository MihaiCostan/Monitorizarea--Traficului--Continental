# Monitorizarea Traficului

Unified repository for a traffic monitoring system with a C++ server and a Python client.

## Overview

- **Purpose:** Collect, process and store traffic data on the server; provide a Python GUI client for visualization and interaction.
- **Components:** a C++ server (network listener, database manager, traffic manager) and a Python client GUI.

## Repository Structure

- **Server (C++):** `server_C++/`
  - [server_C++/src](server_C++/src) — implementation (.cpp files)
  - [server_C++/include](server_C++/include) — headers like `DatabaseManager.hpp`, `TCPServer.hpp`, `TrafficManager.hpp`
  - [server_C++/Makefile](server_C++/Makefile) — build rules
- **Client (Python):** `client_python/`
  - [client_python/main.py](client_python/main.py) — application entry
  - [client_python/gui_manager.py](client_python/gui_manager.py) — GUI code
  - [client_python/network_manager.py](client_python/network_manager.py) — networking helpers
  - [client_python/models.py](client_python/models.py) — data models

## Prerequisites

- C++ toolchain: `g++` / `clang++` and `make` (for the server)
- Python 3.8+ and `pip` (for the client)
- `sqlite3` (used by the server for persistent storage)

## Server: Build & Run

1. Build from the `server_C++` directory:

```bash
cd server_C++
make
```

2. After a successful build, run the produced server binary. The binary name depends on the `Makefile`; a common pattern is:

```bash
./server   # or the executable produced by the Makefile
```

3. Database: the server uses an SQLite database (e.g. `traffic.db`). If one does not exist create it:

```bash
cd server_C++
sqlite3 traffic.db
```

If your project includes SQL schema or migration scripts add them to `server_C++/` and apply before running the server.

## Client: Setup & Run

1. Create and activate a Python virtual environment (recommended):

```bash
cd client_python
python3 -m venv venv
source venv/bin/activate
```

2. Install dependencies. If a `requirements.txt` exists, use it; otherwise install required packages manually (example):

```bash
pip install -r requirements.txt  # if present
# or example: pip install pyqt5 requests
```

3. Run the client GUI:

```bash
python main.py
```

The client expects to connect to the server TCP endpoint; configure host/port in [client_python/network_manager.py](client_python/network_manager.py) or via the GUI settings if provided.

## Development Notes

- Server code entrypoint: [server_C++/src/main.cpp](server_C++/src/main.cpp)
- Key headers: [server_C++/include/DatabaseManager.hpp](server_C++/include/DatabaseManager.hpp), [server_C++/include/TCPServer.hpp](server_C++/include/TCPServer.hpp), [server_C++/include/TrafficManager.hpp](server_C++/include/TrafficManager.hpp)
- Client GUI: [client_python/gui_manager.py](client_python/gui_manager.py)

## Testing / Debugging

- Use `sqlite3` to inspect the database contents: `sqlite3 traffic.db` then run queries.
- Server logs / stdout will show connection attempts and processing details; run the server from a terminal to view them.
- Use multiple client instances to simulate traffic sources.

## Contributing

- Open an issue with a description of the change/bug.
- Create feature branches, run and verify both server and client locally, and open a pull request.

## Next Steps / Suggestions

- Add a `requirements.txt` for the client listing exact Python dependencies.
- Add a `README` or `.sql` schema file in `server_C++/` if the DB schema is required for setup.

---

If you'd like, I can also:

- add a `requirements.txt` for the Python client,
- generate a SQL schema file for `traffic.db`, or
- run the server build and report any build errors.

Please tell me which of the above you'd like me to do next.
