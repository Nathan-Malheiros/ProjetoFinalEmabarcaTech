# 🏥 Sistema Inteligente de Triagem Hospitalar via IoT

**Projeto Final - Capacitação Embarcatech Expansão**

Este repositório contém o código-fonte integral (Hardware e Software) de um sistema de chamada de enfermagem inteligente. O projeto foi desenvolvido para mitigar o problema da "fadiga de alarmes" em ambientes clínicos, substituindo campainhas tradicionais por um ecossistema IoT capaz de categorizar necessidades, priorizar emergências e fornecer feedback visual aos pacientes em tempo real.

---

## 🏗️ Arquitetura e Estrutura do Repositório

O projeto adota uma arquitetura Cliente-Servidor e está dividido em dois módulos principais. Cada módulo possui o seu próprio `README.md` com instruções detalhadas de instalação e execução.

```text
📦 ProjetoFinalEmbarcaTech
 ┣ 📂 ProjetoFinalV1        # Firmware em C/C++ para o Terminal do Leito (Pico W)
 ┣ 📂 pythonserver          # Aplicação do Posto de Enfermagem (Servidor Python)
 ┗ 📜 README.md             # Documentação Geral (Este arquivo)
```

### 1. Nó de Borda: Terminal do Leito (C/C++)
Localizado na pasta `ProjetoFinalV1`.
É o firmware embarcado na placa didática **BitDogLab** (Raspberry Pi Pico W). Responsável por ler os acionamentos físicos do paciente via interrupções, tratar o *debounce* via software, acionar as matrizes de LEDs para feedback tátil/visual e transmitir os dados para a rede local utilizando datagramas UDP (via LwIP).
👉 **[Ver documentação do Hardware](./ProjetoFinalV1/README.md)**

### 2. Nó Central: Posto de Enfermagem (Python)
Localizado na pasta `pythonserver/monitor_pacientes`.
Um software assíncrono desenvolvido em Python puro (Tkinter, Socket, Threading). Ele escuta ativamente a rede Wi-Fi em busca de pacotes UDP. Ao receber um chamado, a interface gráfica recalcula a prioridade e desenha os alertas na tela, colocando eventos de "Emergência" em preempção (topo da fila com alertas sonoros e visuais pulsantes).
👉 **[Ver documentação do Software](./pythonserver/README.md)**

---

## 🛠️ Tecnologias Utilizadas

- **Microcontrolador:** Raspberry Pi Pico W (RP2040) / Placa BitDogLab
- **Linguagem de Hardware:** C/C++ (Raspberry Pi Pico SDK)
- **Linguagem de Software:** Python 3.9+
- **Protocolos de Rede:** Wi-Fi (802.11 b/g/n), UDP, IPv4
- **Conceitos Aplicados:** Interrupções (IRQ), Máquinas de Estado (PIO), Multithreading, Sockets, Programação Orientada a Eventos.

---

## 🎥 Demonstração Prática

O vídeo abaixo (máx. 3 minutos) apresenta o sistema completo operando na prática, demonstrando a comunicação ponta-a-ponta entre a placa BitDogLab e o computador central:

👉 **[Inserir Link para o Vídeo no YouTube]**

---

## ⚖️ Autoria e Direitos Autorais

**Copyright (c) 2026 Nathan Linhares Dias Malheiros.** Todos os direitos reservados.

Este sistema foi concebido, arquitetado e desenvolvido integralmente por Nathan Linhares Dias Malheiros como requisito para a conclusão do programa **Embarcatech**. 

A visualização do código-fonte é pública estritamente para fins de auditoria acadêmica e avaliação da banca examinadora. É expressamente **proibida** a cópia, clonagem, distribuição, modificação ou exploração comercial (parcial ou total) deste software, firmware ou arquitetura lógica sem a autorização prévia por escrito do autor.

*Contato e Portfólio:* [nathanldmalheiros@gmail.com]