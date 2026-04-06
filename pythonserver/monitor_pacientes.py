import socket
import threading
import tkinter as tk
from tkinter import ttk, messagebox, filedialog
from datetime import datetime
import time
import csv

try:
    import winsound
    SOM_DISPONIVEL = True
except ImportError:
    SOM_DISPONIVEL = False


# CONFIGURAÇÕES DE REDE

UDP_IP = "0.0.0.0"
UDP_PORT = 5000

CONFIG_PEDIDOS = {
    "EMERGENCIA":       {"cor": "#c0392b", "icone": "🚨", "fg": "white", "prioridade": 1},
    "PRECISO DE AJUDA": {"cor": "#e67e22", "icone": "🆘", "fg": "white", "prioridade": 2},
    "SINTO DOR":        {"cor": "#f1c40f", "icone": "💊", "fg": "black", "prioridade": 3},
    "QUERO AGUA":       {"cor": "#3498db", "icone": "💧", "fg": "white", "prioridade": 4},
    "ESTOU CANSADO":    {"cor": "#2ecc71", "icone": "🛌", "fg": "black", "prioridade": 5}
}
CONFIG_PADRAO = {"cor": "#95a5a6", "icone": "🔔", "fg": "black", "prioridade": 99}

class CentralMonitoramentoGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("🩺 Central de Monitoramento Hospitalar Avançada v6")
        self.root.geometry("1100x750")
        self.root.configure(bg="#1e272e")
        self.root.state('zoomed') 

        self.pedidos_ativos = {} 
        self.historico_eventos = [] 
        self.contador_ids = 0
        self.alarme_mutado = False
        
        self.widgets_emergencia = []
        self.estado_pisco = False

        self.configurar_estilos()
        self.construir_interface()

        self.thread_rede = threading.Thread(target=self.escutar_rede, daemon=True)
        self.thread_rede.start()

        self.thread_audio = threading.Thread(target=self.gerenciar_audio, daemon=True)
        self.thread_audio.start()

        self.animar_emergencias()

    def configurar_estilos(self):
        style = ttk.Style()
        style.theme_use('clam')
        style.configure("TNotebook", background="#1e272e", borderwidth=0)
        style.configure("TNotebook.Tab", background="#2f3640", foreground="#dcdde1", padding=[15, 5], font=("Segoe UI", 12, "bold"))
        style.map("TNotebook.Tab", background=[("selected", "#3498db")], foreground=[("selected", "white")])
        
        style.configure("Treeview", background="#2f3640", foreground="#dcdde1", fieldbackground="#2f3640", rowheight=30, font=("Segoe UI", 11))
        style.configure("Treeview.Heading", background="#1e272e", foreground="#3498db", font=("Segoe UI", 12, "bold"))
        style.map("Treeview", background=[("selected", "#2980b9")])

    def construir_interface(self):
        self.frame_top = tk.Frame(self.root, bg="#2f3640", pady=10, padx=20)
        self.frame_top.pack(fill=tk.X)
        
        tk.Label(self.frame_top, text="🏥 PAINEL DE ENFERMAGEM", font=("Segoe UI", 20, "bold"), bg="#2f3640", fg="#dcdde1").pack(side=tk.LEFT)

        self.btn_mutar = tk.Button(self.frame_top, text="🔊 Mudo (Desativado)", font=("Segoe UI", 12, "bold"), 
                                   bg="#2ecc71", fg="black", cursor="hand2", command=self.alternar_mudo)
        self.btn_mutar.pack(side=tk.RIGHT)

        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        self.aba_painel = tk.Frame(self.notebook, bg="#1e272e")
        self.notebook.add(self.aba_painel, text="📊 Painel de Pacientes")
        
        self.aba_historico = tk.Frame(self.notebook, bg="#1e272e")
        self.notebook.add(self.aba_historico, text="📋 Histórico e Relatórios")

        self.construir_aba_painel()
        self.construir_aba_historico()

    def construir_aba_painel(self):
        self.canvas = tk.Canvas(self.aba_painel, bg="#1e272e", highlightthickness=0)
        self.scrollbar = tk.Scrollbar(self.aba_painel, orient="vertical", command=self.canvas.yview)
        self.frame_mosaico = tk.Frame(self.canvas, bg="#1e272e")

        self.frame_mosaico.bind("<Configure>", lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all")))
        self.canvas_window = self.canvas.create_window((0, 0), window=self.frame_mosaico, anchor="nw")
        
        # O Evento Configure do Canvas agora lida com o esticamento vertical e horizontal
        self.canvas.bind('<Configure>', self._on_canvas_configure)
        self.canvas.configure(yscrollcommand=self.scrollbar.set)
        
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        self.scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.aba_painel.bind_all("<MouseWheel>", self._on_mousewheel)
        self.atualizar_mosaico()

    def _on_canvas_configure(self, event):
        # Ajusta a largura sempre
        self.canvas.itemconfig(self.canvas_window, width=event.width)
        
        qtd = len(self.pedidos_ativos)
        # Se for 1, 2 ou 3 cards, estica a linha para ocupar 100% da altura da tela
        if 0 < qtd <= 3:
            self.frame_mosaico.rowconfigure(0, weight=1, uniform="row", minsize=event.height - 20)

    def _on_mousewheel(self, event):
        if self.notebook.index(self.notebook.select()) == 0:
            self.canvas.yview_scroll(int(-1*(event.delta/120)), "units")

    def construir_aba_historico(self):
        frame_acoes = tk.Frame(self.aba_historico, bg="#1e272e", pady=10)
        frame_acoes.pack(fill=tk.X)

        tk.Button(frame_acoes, text="💾 Exportar para Excel/CSV", font=("Segoe UI", 12), bg="#3498db", fg="white", 
                  cursor="hand2", command=self.exportar_csv).pack(side=tk.LEFT, padx=10)

        colunas = ("ID", "Paciente", "Pedido", "Hora Chamado", "Hora Atendido", "Tempo Espera", "Status")
        self.tabela = ttk.Treeview(self.aba_historico, columns=colunas, show="headings")
        
        for col in colunas:
            self.tabela.heading(col, text=col)
            self.tabela.column(col, anchor=tk.CENTER, width=120)
        self.tabela.column("Paciente", width=200)
        self.tabela.column("Pedido", width=200)

        scroll_tabela = ttk.Scrollbar(self.aba_historico, orient=tk.VERTICAL, command=self.tabela.yview)
        self.tabela.configure(yscroll=scroll_tabela.set)

        self.tabela.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(10,0), pady=10)
        scroll_tabela.pack(side=tk.RIGHT, fill=tk.Y, padx=(0,10), pady=10)

    def alternar_mudo(self):
        self.alarme_mutado = not self.alarme_mutado
        if self.alarme_mutado:
            self.btn_mutar.configure(bg="#e74c3c", fg="white", text="🔇 Mudo (Ativado)")
        else:
            self.btn_mutar.configure(bg="#2ecc71", fg="black", text="🔊 Mudo (Desativado)")

    def obter_hora_obj(self):
        return datetime.now()

    def formatar_hora(self, dt_obj):
        return dt_obj.strftime("%H:%M:%S")

    def obter_config_visual(self, mensagem):
        mensagem_upper = mensagem.upper()
        for chave, config in CONFIG_PEDIDOS.items():
            if chave in mensagem_upper:
                return config
        return CONFIG_PADRAO

    def escutar_rede(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((UDP_IP, UDP_PORT))
        
        while True:
            try:
                data, addr = sock.recvfrom(1024)
                pacote = data.decode('utf-8')
                
                if "|" in pacote:
                    paciente, mensagem = pacote.split("|", 1)
                else:
                    paciente = f"IP {addr[0]}"
                    mensagem = pacote

                self.root.after(0, self.adicionar_pedido, paciente, mensagem)
                self.root.after(0, lambda: self.notebook.select(self.aba_painel))
            except Exception as e:
                print(f"Erro na rede: {e}")

    def adicionar_pedido(self, paciente, mensagem):
        self.contador_ids += 1
        id_pedido = self.contador_ids
        agora = self.obter_hora_obj()
        config = self.obter_config_visual(mensagem)
        
        self.pedidos_ativos[id_pedido] = {
            "paciente": paciente,
            "mensagem": mensagem,
            "hora_obj": agora,
            "hora_str": self.formatar_hora(agora),
            "timestamp": time.time(),
            "config": config
        }

        registro_log = {
            "ID": id_pedido,
            "Paciente": paciente,
            "Pedido": mensagem,
            "Hora Chamado": self.formatar_hora(agora),
            "Hora Atendido": "---",
            "Tempo Espera": "---",
            "Status": "🚨 PENDENTE"
        }
        self.historico_eventos.append(registro_log)
        self.tabela.insert("", tk.END, iid=str(id_pedido), values=list(registro_log.values()))
        
        self.atualizar_mosaico()

    def atender_pedido(self, id_pedido):
        if id_pedido in self.pedidos_ativos:
            dados = self.pedidos_ativos[id_pedido]
            diferenca = self.obter_hora_obj() - dados["hora_obj"]
            minutos, segundos = divmod(diferenca.seconds, 60)
            tempo_espera = f"{minutos}m {segundos}s"

            for log in self.historico_eventos:
                if log["ID"] == id_pedido:
                    log["Hora Atendido"] = self.formatar_hora(self.obter_hora_obj())
                    log["Tempo Espera"] = tempo_espera
                    log["Status"] = "✅ ATENDIDO"
                    self.tabela.item(str(id_pedido), values=list(log.values()))
                    break

            del self.pedidos_ativos[id_pedido]
            self.atualizar_mosaico()

    def atualizar_mosaico(self):
        for widget in self.frame_mosaico.winfo_children():
            widget.destroy()
        
        self.widgets_emergencia.clear()
        qtd = len(self.pedidos_ativos)

        if qtd == 0:
            lbl_vazio = tk.Label(self.frame_mosaico, text="✅ Nenhum chamado ativo no momento.\nEquipe disponível.", 
                                 font=("Segoe UI", 20), bg="#1e272e", fg="#2ecc71", pady=150)
            lbl_vazio.pack(expand=True)
            return


        # LÓGICA DO DIAGRAMA (Adaptação de Colunas e Linhas)
    
        if qtd == 1:
            colunas = 1
        elif qtd == 2:
            colunas = 2
        elif qtd == 3:
            colunas = 3
        else:
            colunas = 2 # 4 ou mais cards travam em 2 colunas
            
   
        for c in range(colunas):
            self.frame_mosaico.columnconfigure(c, weight=1, uniform="col")

     
        linhas = (qtd + colunas - 1) // colunas
        self.root.update_idletasks()
        altura_disponivel = self.canvas.winfo_height()
        if altura_disponivel < 100: altura_disponivel = 600 

        for r in range(linhas):
            if qtd <= 3:
                # Ocupa 100% da tela (1, 2 ou 3 cards lado a lado)
                self.frame_mosaico.rowconfigure(r, weight=1, uniform="row", minsize=altura_disponivel - 20)
            else:
                # Quando passa de 3, ganha um tamanho fixo (350px) pra poder rolar a página pra baixo
                self.frame_mosaico.rowconfigure(r, weight=1, uniform="row", minsize=350)

        # Ordenação de prioridade (Emergência sempre fura a fila para o começo)
        pedidos_ordenados = []
        for id_pedido, dados in self.pedidos_ativos.items():
            pedidos_ordenados.append({
                'id': id_pedido,
                'dados': dados,
                'config': dados['config'],
                'prioridade': dados['config']['prioridade'],
                'hora_obj': dados['hora_obj']
            })

        pedidos_ordenados.sort(key=lambda x: (x['prioridade'], x['hora_obj']))

        row, col = 0, 0

        for pedido in pedidos_ordenados:
            id_pedido = pedido['id']
            dados = pedido['dados']
            config = pedido['config']
            cor_base = config['cor']
            fg_base = config['fg']
            
          
            card = tk.Frame(self.frame_mosaico, bg=cor_base, bd=0)
            card.grid(row=row, column=col, padx=15, pady=15, sticky="nsew")
            
            card_interno = tk.Frame(card, bg=cor_base)
            card_interno.pack(fill=tk.BOTH, expand=True)

            content_frame = tk.Frame(card_interno, bg=cor_base)
            content_frame.place(relx=0.5, rely=0.5, anchor=tk.CENTER)

            lbl_icone = tk.Label(content_frame, text=config['icone'], font=("Segoe UI Emoji", 45), bg=cor_base, fg=fg_base)
            lbl_icone.pack(pady=(0, 10))
            
            lbl_pac = tk.Label(content_frame, text=dados['paciente'].upper(), font=("Segoe UI", 16, "bold"), bg=cor_base, fg=fg_base, wraplength=350, justify="center")
            lbl_pac.pack()
            
            lbl_msg = tk.Label(content_frame, text=dados['mensagem'].upper(), font=("Segoe UI", 20, "bold"), bg=cor_base, fg=fg_base, wraplength=350, justify="center")
            lbl_msg.pack(pady=10)
            
            lbl_hora = tk.Label(content_frame, text=f"Chamou às: {dados['hora_str']}", font=("Segoe UI", 12), bg=cor_base, fg=fg_base)
            lbl_hora.pack(pady=(0, 15))

            btn_atender = tk.Button(content_frame, text="✅ ATENDER PACIENTE", font=("Segoe UI", 14, "bold"),
                                    bg="#ecf0f1", fg="#2c3e50", cursor="hand2", relief=tk.RAISED, bd=4,
                                    command=lambda id_p=id_pedido: self.atender_pedido(id_p))
            btn_atender.pack(ipadx=15, ipady=8)

            if config['prioridade'] == 1:
                self.widgets_emergencia.append((card, card_interno, content_frame, [lbl_icone, lbl_pac, lbl_msg, lbl_hora]))

            col += 1
            if col >= colunas:
                col = 0
                row += 1

    def animar_emergencias(self):
        self.estado_pisco = not self.estado_pisco
        cor_fundo = "#ff0000" if self.estado_pisco else "#8b0000"
        
        for w_card, w_interno, w_content, labels in self.widgets_emergencia:
            try:
                w_card.configure(bg=cor_fundo)
                w_interno.configure(bg=cor_fundo)
                w_content.configure(bg=cor_fundo)
                for lbl in labels:
                    lbl.configure(bg=cor_fundo)
            except:
                pass 

        self.root.after(500, self.animar_emergencias)

    def gerenciar_audio(self):
        while True:
            if not SOM_DISPONIVEL or self.alarme_mutado:
                time.sleep(0.5)
                continue

            tem_emergencia = any(d['config']['prioridade'] == 1 for d in self.pedidos_ativos.values())
            agora = time.time()
            tem_pedido_recente = any((agora - d['timestamp'] < 5) and (d['config']['prioridade'] > 1) 
                                     for d in self.pedidos_ativos.values())

            if tem_emergencia:
                winsound.Beep(1500, 300)
                winsound.Beep(2000, 300)
                time.sleep(0.2)
            elif tem_pedido_recente:
                winsound.Beep(1000, 400)
                time.sleep(1)
            else:
                time.sleep(0.5)

    def exportar_csv(self):
        if not self.historico_eventos:
            messagebox.showwarning("Aviso", "Não há eventos para exportar.")
            return

        arquivo = filedialog.asksaveasfilename(defaultextension=".csv", filetypes=[("Arquivo CSV", "*.csv")], title="Salvar Relatório")
        if arquivo:
            try:
                with open(arquivo, mode='w', newline='', encoding='utf-8') as f:
                    writer = csv.DictWriter(f, fieldnames=self.historico_eventos[0].keys())
                    writer.writeheader()
                    writer.writerows(self.historico_eventos)
                messagebox.showinfo("Sucesso", "Relatório exportado!")
            except Exception as e:
                messagebox.showerror("Erro", f"Falha ao exportar:\n{e}")

if __name__ == "__main__":
    janela = tk.Tk()
    app = CentralMonitoramentoGUI(janela)
    janela.mainloop()