# 🖥️ Firmware C/C++ - Terminal de Leito (Raspberry Pi Pico W)

Este diretório contém o código-fonte do firmware embarcado desenvolvido para o microcontrolador **Raspberry Pi Pico W** (utilizando a placa BitDogLab). Este módulo atua como o **Nó de Borda (Edge Node)** do Sistema Inteligente de Triagem Hospitalar.

## 📌 Escopo deste Módulo

A função deste firmware é monitorar fisicamente as interações do paciente (botões), fornecer feedback visual local (Matriz de LEDs e LED RGB) e empacotar essas ações em datagramas UDP, transmitindo-os pela rede Wi-Fi local para o servidor Python.

### Principais Características do Código:
- **Interrupções de Hardware (IRQ):** Leitura de botões orientada a eventos (borda de descida), substituindo o uso ineficiente de *polling*.
- **Filtro Debounce em Software:** Algoritmo de temporização (ex: 250ms) implementado para ignorar o repique mecânico dos botões, evitando envios duplicados.
- **Protocolo UDP (LwIP):** Envio assíncrono ("fire-and-forget") de *payloads* no formato `ID_LEITO|COMANDO` diretamente para a central.
- **Controle de Periféricos (PIO):** Gerenciamento eficiente da matriz de LEDs WS2812B através de máquinas de estado (PIO) para não sobrecarregar a CPU principal.

---

## 🛠️ Dependências e Pré-requisitos

Para compilar e modificar este código, o seu ambiente de desenvolvimento deve conter:
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk) instalado e configurado.
- Compilador **ARM GCC** (`arm-none-eabi-gcc`).
- **CMake** e **Make/Ninja** (Build tools).
- Extensão do Raspberry Pi Pico no **Visual Studio Code** (Recomendado).

---

## 🛜 Configuração de Rede

Antes de compilar, você **precisa** informar as credenciais da sua rede Wi-Fi e o endereço IP da máquina que executará o servidor Python. 

Verifique o arquivo `CMakeLists.txt` (ou o arquivo de cabeçalho principal `.c`/`.h`) e altere as seguintes definições com os seus dados:

```cmake
# Exemplo de configuração (NÃO envie suas senhas reais para o GitHub)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    WIFI_SSID="NOME_DA_SUA_REDE"
    WIFI_PASSWORD="SUA_SENHA_WIFI"
    SERVER_IP="192.168.1.100" # IP do Computador rodando o Python
    SERVER_PORT=5000
)
```

---

## 🚀 Como Compilar e Fazer o Flash (Upload)

1. Clone o repositório raiz e navegue até esta pasta:
   ```bash
   git clone [https://github.com/Nathan-Malheiros/ProjetoFinalEmbarcaTech.git](https://github.com/Nathan-Malheiros/ProjetoFinalEmbarcaTech.git)
   cd ProjetoFinalEmbarcaTech/ProjetoFinalV1
   ```
2. Abra a pasta no **VS Code** (certifique-se de que a extensão do Pico SDK esteja ativa).
3. Na barra inferior do VS Code, clique em **Compile** (ou use a interface do CMake).
4. O processo gerará um arquivo com a extensão `.uf2` na pasta `build/`.
5. Conecte a sua Raspberry Pi Pico W ao computador via USB enquanto mantém pressionado o botão **BOOTSEL**.
6. A placa será reconhecida como um pendrive (unidade `RPI-RP2`).
7. Arraste o arquivo `.uf2` para dentro desta unidade. O dispositivo irá reiniciar automaticamente e rodar o firmware.