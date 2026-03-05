# Traffic Monitoring System

This project implements a comprehensive traffic monitoring system that captures, processes, and stores network traffic data. It features a C++ backend for high-performance data handling and a Python-based graphical user interface (GUI) for real-time visualization and management.

## Features

- **Real-time Traffic Monitoring:** Captures and analyzes network traffic in real-time.
- **Database Integration:** Utilizes a database (SQLite) for persistent storage of traffic data.
- **JSON Data Simulation:** Simulates JSON files for efficient data exchange and configuration.
- **Robust Network Communication:** Employs TCP/IP for reliable client-server communication.
- **Multi-threaded Server Architecture:** Handles multiple client connections concurrently for high scalability.
- **Python GUI:** Provides an intuitive graphical interface for users to interact with the system, display traffic statistics, and manage configurations.
- **Cross-platform Compatibility:** Designed to run on various operating systems.

## Technologies Used

- **C++:** For the high-performance backend server, responsible for traffic capture, processing, and database interaction.
- **Python:** For the client-side application, including the GUI and network communication with the server.
- **Databases:** Integrated with a database system for efficient data storage and retrieval.
- **JSON:** Used for data serialization and deserialization, particularly for configuration and simulated data.
- **TCP/IP Sockets:** For establishing reliable communication channels between the client and server.

## Project Structure

The project is organized into two main components:

- **`server_C++/`**: Contains the C++ server implementation.
  - `include/`: Header files for various server modules (e.g., `DatabaseManager.hpp`, `TCPServer.hpp`, `TrafficManager.hpp`, `json.hpp`).
  - `src/`: Source files for server modules (e.g., `DatabaseManager.cpp`, `TCPServer.cpp`, `TrafficManager.cpp`, `main.cpp`).
  - `Makefile`: Build script for compiling the C++ server.
  - `traffic_server`: Compiled executable of the C++ server.

- **`client_python/`**: Contains the Python client application.
  - `gui_manager.py`: Manages the graphical user interface.
  - `main.py`: Main entry point for the Python client.
  - `models.py`: Defines data models used in the client application.
  - `network_manager.py`: Handles network communication between the Python client and the C++ server.

## Getting Started

### Prerequisites

- C++ Compiler (e.g., g++)
- Python 3.x
- Required Python packages (install using `pip install -r requirements.txt` in `client_python/`)

### Building and Running the Server (C++)

1.  Navigate to the `server_C++/` directory:
    ```bash
    cd server_C++
    ```
2.  Compile the server:
    ```bash
    make
    ```
3.  Run the server:
    ```bash
    ./traffic_server
    ```

### Running the Client (Python)

1.  Navigate to the `client_python/` directory:
    ```bash
    cd client_python
    ```
2.  Install Python dependencies (if any, a `requirements.txt` file might be needed):
    ```bash
    pip install -r requirements.txt
    ```
3.  Run the client application:
    ```bash
    python main.py
    ```

## Configuration

Configuration details for both the client and server can be found in their respective modules. JSON files are used for simulating data and may also be used for configuration purposes.

## Contribution

Feel free to contribute to this project by submitting bug reports or feature requests.

If you'd like, I can also:

- add a `requirements.txt` for the Python client,
- generate a SQL schema file for `traffic.db`, or
- run the server build and report any build errors.
