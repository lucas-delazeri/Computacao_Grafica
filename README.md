# Projetos OpenGL 3.3

## ⚙️ Pré-requisitos

Para compilar e executar este projeto no Windows, você precisará do ambiente MSYS2 (UCRT64) com o compilador `g++` e a biblioteca GLFW3 instalados no sistema.

Caso a biblioteca GLFW ainda não esteja instalada no seu MSYS2, abra o terminal MSYS2 UCRT64 e execute:

```bash
pacman -S mingw-w64-ucrt-x86_64-glfw
```
## 🚀 Como Compilar e Executar

Abra o terminal do MSYS2 UCRT64 e navegue até a pasta raiz do projeto.

### 1. Renderizar

Compilação:

```bash
g++ src/nome_arquivo.cpp src/glad.c -Iinclude -o main.exe -lglfw3 -lopengl32 -lgdi32
```

Execução:

```bash
./main.exe
```
## 💻 Configuração do VS Code (IntelliSense)

O repositório já inclui os arquivos de configuração necessários dentro da pasta `.vscode/`. O arquivo `c_cpp_properties.json` garante o autocomplete e remove avisos de erro de inclusão (como `cannot open source file "GLFW/glfw3.h"`).

Ele está pré-configurado com o caminho padrão de instalação do MSYS2:

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

📌 **Observação sobre o caminho do MSYS2:**
Se o seu MSYS2 estiver instalado em uma pasta ou unidade diferente (por exemplo, `D:/msys64` ou `C:/Ferramentas/msys64`), basta atualizar as propriedades `"includePath"` e `"compilerPath"` no arquivo `.vscode/c_cpp_properties.json` apontando para o seu diretório correspondente.

## 🛠️ Detalhamento das Flags do Compilador

- `src/*.cpp src/glad.c`: Compila o código-fonte da aplicação em conjunto com o carregador GLAD.
- `-Iinclude`: Adiciona a pasta local `include/` ao caminho de busca dos cabeçalhos (`glad/glad.h` e `KHR/khrplatform.h`).
- `-o <nome>.exe`: Define o nome do executável de saída.
- `-lglfw3`: Realiza o link com a biblioteca dinâmica GLFW3 gerenciada pelo MSYS2.
- `-lopengl32`: Realiza o link com o driver OpenGL nativo do Windows.
- `-lgdi32`: Biblioteca de interface gráfica do Windows necessária para o funcionamento da GLFW.
