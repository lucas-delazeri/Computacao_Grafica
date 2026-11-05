# OpenGL 3.3 Projects

## ⚙️ Prerequisites

To compile and run this project on Windows, you will need the MSYS2 (UCRT64) environment with the `g++` compiler and the GLFW3 library installed on your system.

If the GLFW library is not yet installed in your MSYS2, open the MSYS2 UCRT64 terminal and run:

```bash
pacman -S mingw-w64-ucrt-x86_64-glfw
```

## 🚀 How to Compile and Run

Open the MSYS2 UCRT64 terminal and navigate to the project's root folder.

### 1. Rendering

Compilation:

```bash
g++ src/file_name.cpp src/glad.c -Iinclude -o main.exe -lglfw3 -lopengl32 -lgdi32
```

Execution:

```bash
./main.exe
```

## 💻 VS Code Configuration (IntelliSense)

The repository already includes the necessary configuration files inside the `.vscode/` folder. The `c_cpp_properties.json` file ensures autocomplete works correctly and removes include error warnings (such as `cannot open source file "GLFW/glfw3.h"`).

It is pre-configured with the default MSYS2 installation path:

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/include",
                "C:/msys64/ucrt64/include"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "compilerPath": "C:/msys64/ucrt64/bin/g++.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x64"
        }
    ],
    "version": 4
}
```

📌 **Note on the MSYS2 path:**
If your MSYS2 is installed in a different folder or drive (for example, `D:/msys64` or `C:/Tools/msys64`), simply update the `"includePath"` and `"compilerPath"` properties in the `.vscode/c_cpp_properties.json` file to point to your corresponding directory.

## 🛠️ Compiler Flags Breakdown

- `src/*.cpp src/glad.c`: Compiles the application's source code along with the GLAD loader.
- `-Iinclude`: Adds the local `include/` folder to the header search path (`glad/glad.h` and `KHR/khrplatform.h`).
- `-o <name>.exe`: Sets the name of the output executable.
- `-lglfw3`: Links against the GLFW3 dynamic library managed by MSYS2.
- `-lopengl32`: Links against the native Windows OpenGL driver.
- `-lgdi32`: Windows Graphics Device Interface library required for GLFW to function properly.