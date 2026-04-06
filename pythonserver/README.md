# 🖥️ Central de Monitorização Hospitalar (Servidor Python)

Este diretório contém o software de servidor e a interface gráfica (Dashboard) desenvolvidos em Python. Este sistema atua como o **Nó Central** do projeto, responsável por receber, processar e exibir os alertas emitidos pelos terminais instalados nos leitos dos doentes (Raspberry Pi Pico W).

## 📌 Arquitetura do Software

Para garantir que a interface não congela enquanto aguarda pacotes da rede, a aplicação foi desenhada com uma arquitetura **Multithreading** assíncrona:

1. **Thread Principal (UI):** Utiliza o `Tkinter` para desenhar o mosaico dinâmico. A interface recalcula as proporções do ecrã e reordena os "cards" dos doentes automaticamente, dando prioridade absoluta a alertas de Emergência.
2. **Thread de Rede (UDP Listener):** Executa um loop de escuta não-bloqueante na porta definida (`5000`), aguardando os datagramas enviados pela rede local.
3. **Thread de Áudio:** Avalia a gravidade da fila de chamadas em tempo real e emite bipes correspondentes à urgência (contínuos para emergências, temporários para alertas padrão).

---

## 🛠️ Requisitos e Dependências

A aplicação foi otimizada para correr sem a necessidade de bibliotecas externas pesadas, garantindo fácil implementação em computadores hospitalares padrão.

- **Python 3.9 ou superior** instalado no computador.
- Sistema Operativo: Windows (recomendado para suporte total aos bipes da biblioteca nativa `winsound`) ou Linux/macOS (a interface gráfica funcionará, mas o áudio poderá requerer adaptações no módulo `winsound`).

**Bibliotecas Nativas Utilizadas (Não requerem `pip install`):**
- `socket` e `threading` (Comunicações e Paralelismo).
- `tkinter` e `ttk` (Interface Gráfica).
- `csv` (Exportação de relatórios).
- `datetime` e `time` (Métricas de SLA).

---

## 🚀 Como Executar o Servidor

1. Certifique-se de que o computador está ligado à mesma rede Wi-Fi que os microcontroladores.
2. Abra o terminal ou linha de comandos (CMD/PowerShell).
3. Navegue até a pasta do script Python:
   ```bash
   cd pythonserver/monitor_pacientes
   ```
4. Execute o programa principal:
   ```bash
   python monitor_pacientes.py
   ```
5. A interface abrirá em ecrã inteiro. O sistema estará imediatamente à escuta dos nós da rede.

---

## 🛜 Importante: Sincronização de IP

O protocolo UDP necessita de um destino fixo. É **estritamente necessário** que o Endereço IPv4 do computador onde este script está a correr seja exatamente o mesmo IP configurado no firmware C (`SERVER_IP`) da Raspberry Pi Pico W.

Para verificar o seu IP atual no Windows, utilize o comando `ipconfig` no terminal.

---

## 📊 Relatórios e Auditoria

Além da monitorização em tempo real, o software calcula o tempo de resposta (SLA) entre a emissão do alerta e o momento em que a equipa clica em "ATENDER". Todo o histórico de eventos pode ser exportado para um ficheiro `.csv` no final do turno através do botão "Exportar para Excel/CSV" no painel de Relatórios.