# Cuddly Terminal Emulator

A modern terminal emulator for Linux inspired by Windows Terminal. Built with C++ and GTK3, Cuddly provides a fast, customizable, and feature-rich terminal experience.

## Features

- **Tabbed Interface**: Multiple terminal tabs in a single window with full tab management
- **Multiple Color Schemes**: Built-in support for Campbell, One Half Dark, Solarized Dark, Tango Dark, Dracula, and custom schemes
- **JSON Configuration**: Windows Terminal-style configuration with profiles and schemes
- **Keybindings**: 
  - `Ctrl+Shift+T` - New tab
  - `Ctrl+Shift+W` - Close tab
  - `Ctrl+Tab` - Next tab
  - `Ctrl+Shift+Tab` - Previous tab
  - `Ctrl+Shift+C` - Copy
  - `Ctrl+Shift+V` - Paste
  - `Ctrl+Shift+H` - Split pane horizontally (planned)
  - `Ctrl+Shift+V` - Split pane vertically (planned)
  - `Ctrl+Shift+F` - Find in terminal (planned)
- **Profile Support**: Customizable profiles with shell, font, colors, and cursor settings
- **Modern UI**: Dark theme with Windows Terminal-inspired styling and CSS theming
- **Tab Reordering**: Drag and drop tabs to reorder them
- **Auto-close**: Tabs close automatically when shell exits
- **Cursor Customization**: Support for bar, underline, and block cursor shapes
- **Scrollback**: Configurable scrollback buffer (default 10,000 lines)

## Requirements

- CMake 3.15 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+)
- GTK+ 3.0
- VTE 2.91 (libvte)
- json-glib-1.0

### Installing Dependencies

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libgtk-3-dev libvte-2.91-dev libjson-glib-dev
```

#### Fedora
```bash
sudo dnf install cmake gcc-c++ gtk3-devel vte291-devel json-glib-devel
```

#### Arch Linux
```bash
sudo pacman -S cmake gcc gtk3 vte3 json-glib
```

## Building

1. Clone the repository:
```bash
git clone https://github.com/Polabiel/cuddly.git
cd cuddly
```

2. Create a build directory and compile:
```bash
mkdir build
cd build
cmake ..
make
```

3. (Optional) Install system-wide:
```bash
sudo make install
```

## Running

After building, you can run the terminal emulator:

```bash
# From the build directory
./cuddly

# Or if installed system-wide
cuddly
```

## Configuration

Configuration is stored in `resources/defaults.json`. You can customize:

- **Profiles**: Shell command, color scheme, font settings
- **Color Schemes**: Foreground, background, and ANSI colors
- **Keybindings**: Custom keyboard shortcuts
- **Appearance**: Theme, opacity, tab behavior

Example configuration:
```json
{
  "profiles": {
    "defaults": {
      "name": "Default",
      "commandline": "/bin/bash",
      "colorScheme": "Campbell",
      "fontSize": 12
    }
  }
}
```

## Project Structure

```
cuddly/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── LICENSE                 # MIT License
├── resources/
│   └── defaults.json       # Terminal configuration
├── include/                # Header files
└── src/                    # Source files
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by [Windows Terminal](https://github.com/microsoft/terminal)
- Built with [GTK+](https://www.gtk.org/) and [VTE](https://wiki.gnome.org/Apps/Terminal/VTE)

## Windows Terminal Feature Parity

Cuddly Terminal aims to bring Windows Terminal's best features to Linux:

### Implemented
- ✅ JSON-based configuration (similar to settings.json)
- ✅ Multiple color schemes with full ANSI color support
- ✅ Tabbed interface with drag-to-reorder
- ✅ Profile system for different shell configurations
- ✅ Customizable keybindings
- ✅ Modern, dark-themed UI
- ✅ Font customization (family and size)
- ✅ Cursor shape customization (bar, block, underline)
- ✅ Configurable scrollback buffer

### Planned
- 🔄 Split panes (horizontal and vertical)
- 🔄 Command palette
- 🔄 Search in terminal
- 🔄 Background image support
- 🔄 Opacity/transparency settings
- 🔄 Multiple profile support
- 🔄 Profile switching via dropdown
