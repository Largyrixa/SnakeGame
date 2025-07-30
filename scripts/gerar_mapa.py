import graphviz

def visualizar_rede_neural(caminho_arquivo, range_n = range(24)):
    """
    Gera uma visualização final e ajustada de uma rede neural,
    garantindo que a legenda caiba na largura da imagem.
    """
    camadas = []
    ligacoes = []

    # 1. Processar o arquivo
    with open (caminho_arquivo, 'r') as f:
        for linha in f:
            linha = linha.strip()
            if not linha or linha.startswith('#'): continue
            
            partes = linha.split()
            comando = partes[0]
            
            if comando == "CAMADA_ENTRADA":
                index = int(partes[1])
                while len(camadas) < index + 1:
                    camadas.append([])
                
                camadas[index] = [[None] * int(partes[2])]
            
            elif comando in ("CAMADA_OCULTA", "CAMADA_SAIDA"):
                index = int(partes[1])
                while len(camadas) < index + 1:
                    camadas.append([])
    
    with open(caminho_arquivo, 'r') as f:
        for linha in f:
            linha = linha.strip()
            if not linha or linha.startswith('#'): continue
            partes = linha.split()
            comando = partes[0]
            if comando == "BIAS":
                camadas[int(partes[1])] = [float(bias) for bias in partes[2:]]
            elif comando == "LIGACAO":
                if int(partes[1]) == 0 and int(partes[2]) not in range_n:
                    continue
                ligacoes.append({
                    "de_camada": int(partes[1]), "de_neuronio": int(partes[2]),
                    "para_camada": int(partes[3]), "para_neuronio": int(partes[4]),
                    "peso": float(partes[5])
                })

    camadas[0] = [[None]*len(range_n)]

    # 2. Configurar o grafo principal
    dot = graphviz.Digraph('RedeNeural')
    dot.attr(rankdir='TB', splines='line', overlap='false', nodesep='1', ranksep='2', newrank='true')
    dot.attr('node', shape='circle', style='filled', fillcolor='lightgray', width='0.4', fixedsize='true', label='')
    
    # 3. Adicionar os neurônios
    for i, camada in enumerate(camadas):
        with dot.subgraph(name=f'cluster_{i}') as c:
            c.attr(style='invis', rank='same')
            for j, bias in enumerate(camada):
                c.node(f'L{i}N{j}')

    # 3.5: ADICIONAR ÂNCORAS PARA ALARGAR O GRÁFICO
    num_inputs = len(camadas[0]) if camadas else 0
    if num_inputs > 0:
        with dot.subgraph(name='cluster_anchors') as c:
            c.attr(rank='same')
            c.node('anchor_left', style='invis', width='10')
            c.node('anchor_right', style='invis', width='10')
            c.edge('anchor_left', 'L0N0', style='invis')
            c.edge(f'L0N{num_inputs-1}', 'anchor_right', style='invis')

    # 4. Adicionar as conexões
    for ligacao in ligacoes:
        peso = ligacao["peso"]
        cor = 'royalblue' if peso >= 0 else 'firebrick'
        espessura = 0.5 + abs(peso) * 4.5
        id_origem = f'L{ligacao["de_camada"]}N{ligacao["de_neuronio"]}'
        id_destino = f'L{ligacao["para_camada"]}N{ligacao["para_neuronio"]}'
        dot.edge(id_origem, id_destino, color=cor, penwidth=str(espessura))

    # 5. LEGENDA FINAL COM TABELA HTML
    with dot.subgraph(name='cluster_legend') as legend:
        legend.attr(rank='sink', style='invis')
        legend.node('legend_table', shape='plaintext', label=r'''<
            <TABLE BORDER="1" CELLBORDER="0" CELLSPACING="8" BGCOLOR="lightyellow">
              <TR>
                <TD COLSPAN="8" ALIGN="CENTER"><B>Legenda</B></TD>
              </TR>
              <TR>
                <TD><FONT COLOR="royalblue"><B>━━━</B></FONT></TD>
                <TD ALIGN="LEFT">Peso Positivo</TD>
                <TD WIDTH="20"></TD>
                <TD><FONT COLOR="firebrick"><B>━━━</B></FONT></TD>
                <TD ALIGN="LEFT">Peso Negativo</TD>
                <TD WIDTH="20"></TD>
                <TD><B>━━━</B></TD>
                <TD ALIGN="LEFT">Conexão Forte</TD>
                <TD WIDTH="20"></TD>
                <TD>─</TD>
                <TD ALIGN="LEFT">Conexão Fraca</TD>
              </TR>
            </TABLE>
        >''')

    # 6. Renderizar o arquivo final
    nome_saida = 'images/mapa_neural'
    dot.render(nome_saida, format='png', view=True, cleanup=True)
    print(f"Visualização salva como '{nome_saida}.png'")

# Execute com seu arquivo
visualizar_rede_neural('data/melhor_cerebro.txt', range(24))