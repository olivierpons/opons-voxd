# opons-voxd

> Observação: o opons-voxd não tem nenhuma relação com jakovius/voxd. O nome foi escolhido de forma independente — `opons` = O. Pons (autor), `voxd` = vox + daemon (tradição latina/Unix).

> **⚠ Compatibilidade com versões anteriores quebrada de propósito.** Este projeto se chamava `voice_in_linux`. A mudança de nome afetou o nome do binário, o nome do arquivo-fonte, todas as variáveis de ambiente (`VOICE_IN_*` → `OPONS_VOXD_*`) e a dependência de `xdotool` em tempo de execução (substituída pela síntese direta via XTest com `libxtst-dev`). Nenhum alias, nenhum período de transição: atualize seus scripts de inicialização, as entradas de início automático e as linhas `apt install`.

**Ditado por voz local para Linux — sua voz nunca sai da sua máquina.**

🌍 **Traduções:**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md) · [한국어](README.ko.md)

📖 **[Meu fluxo de trabalho: dite no seu idioma, digite em inglês](WORKFLOW.pt.md)** — um caso de uso real

---

## Por quê?

Eu queria ditar texto em qualquer aplicativo no Linux sem enviar minha voz para um servidor de terceiros. Todas as soluções que encontrei — extensões de navegador, APIs na nuvem, plataformas SaaS — enviam o áudio para servidores remotos para fazer a transcrição. Ou seja, cada palavra que você diz passa pela internet: e-mails, documentos confidenciais, dados de clientes, anotações pessoais, tudo.

Este projeto é uma alternativa leve e totalmente local. Ele roda inteiramente na sua máquina com o [whisper.cpp](https://github.com/ggerganov/whisper.cpp), uma implementação do Whisper, o modelo de reconhecimento de fala de código aberto da OpenAI. Depois da configuração inicial, nenhuma conexão de rede é necessária. Nenhum dado sai do seu computador.

Feito com a ajuda de IA.

---

## Como funciona

Duas formas de ditar, disponíveis ao mesmo tempo.

### Modo ícone na bandeja — alternar, copiar para a área de transferência

1. Um pequeno ícone aparece na bandeja do sistema
2. **Clique esquerdo** → a gravação começa (o ícone fica vermelho)
3. **Novo clique esquerdo** → a gravação para e o áudio é transcrito localmente pelo Whisper
4. O texto transcrito é copiado automaticamente para a área de transferência (as duas seleções do X11)
5. Uma notificação na área de trabalho mostra o texto por 10 segundos
6. Cole onde quiser com `Ctrl+Shift+V`, `Shift+Insert` ou clique do meio

**Clique com o botão direito** no ícone para abrir um menu com Toggle / Unload models / Reload models / Quit (veja [Liberar a GPU para IA local](#liberar-a-gpu-para-ia-local-servidor-mcp)).

### Modo push-to-talk — segure um atalho, o texto é digitado no cursor

1. **Segure `Ctrl+Shift+Space`** → a gravação começa (o ícone fica vermelho)
2. **Solte as teclas** → a gravação para e o áudio é transcrito
3. O texto transcrito é **digitado diretamente no cursor do teclado** (pela extensão XTest do X11), na janela que estiver em foco

O atalho pode ser configurado com `OPONS_VOXD_PTT_HOTKEY`. Um segundo atalho, independente (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, padrão `ctrl+alt+t`), funciona da mesma forma, mas digita uma tradução em inglês — veja o [README principal](../README.md#push-to-talk-and-translate-hotkey).

### Capturas de tela

| Inativo | Gravando |
|:---:|:---:|
| ![Ícone inativo](../screenshots/opons_voxd_inactive.png) | ![Ícone durante a gravação](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **Uma GPU NVIDIA com CUDA é essencial para ter tempos de resposta aceitáveis.**
> Sem CUDA, a transcrição leva **de 10 a 15 segundos** para 15 segundos de fala (só CPU).
> Com CUDA ativado, a mesma transcrição leva **menos de 1 segundo**.

---

## Recursos

- **100% local** — o áudio é processado na CPU ou na GPU e nunca é enviado a lugar nenhum
- **Aceleração por GPU** — suporte a NVIDIA CUDA para uma transcrição quase instantânea
- **99 idiomas** — graças ao OpenAI Whisper
- **Leveza** — C puro, sem Python: o daemon e, opcionalmente, um servidor MCP de 25 KB vinculado apenas à libc
- **Integração com a bandeja do sistema** — um ícone discreto na barra de tarefas
- **Atalho push-to-talk** — segure `Ctrl+Shift+Space` (configurável) para gravar e solte para digitar a transcrição no cursor do teclado
- **Atalho push-to-talk com tradução** — um segundo atalho, configurável de forma independente, digita uma tradução da sua fala para o inglês em vez da transcrição literal
- **Área de transferência dupla** — o texto vai para as duas seleções do X11, PRIMARY e CLIPBOARD
- **Notificações na área de trabalho** — o texto transcrito aparece em uma notificação
- **Comandos de voz** — um arquivo de comandos por idioma em `commands/`, desativados por padrão (ative com `OPONS_VOXD_COMMANDS=1`). Para adicionar um idioma, basta criar `commands/xx.txt`, sem recompilar.
- **Notificações temporárias** — por padrão, as notificações aparecem e depois somem por completo. `OPONS_VOXD_NOTIFY_PERSIST=1` as mantém no histórico.
- **Modo de notificação** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` controla as notificações de sucesso (padrão `quiet`: balão, sem som). Os erros sempre aparecem com som, em todos os modos, sem exceção. A tabela completa está no README principal.
- **Maiúsculas automáticas** — as frases começam automaticamente com letra maiúscula
- **Liberação da GPU sob demanda** — descarregue os modelos Whisper da VRAM (pelo menu da bandeja ou por qualquer agente de IA, através do servidor MCP integrado) e recarregue-os quando terminar
- **Início automático** — pode ser configurado para iniciar junto com a sessão

---

## Pré-requisitos

### Sistema operacional

| Requisito | Mínimo | Verificação |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Kernel Linux | 5.15+ | `uname -r` |
| Servidor gráfico | X11 | `echo $XDG_SESSION_TYPE` |

> O Wayland não é suportado (o GtkStatusIcon exige X11). O Linux Mint usa X11 por padrão.

### Ferramentas de compilação (obrigatórias)

| Pacote | Mínimo | Instalação |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### Bibliotecas de desenvolvimento (obrigatórias)

| Pacote | Mínimo | Instalação |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> `libportaudio-dev` não é necessário: a compilação baixa `portaudio.h` (via `curl`, no primeiro `make`) e se vincula diretamente a `libportaudio.so.2`. Detalhes no [README principal](../README.md#development-libraries-required).

### Ferramentas de execução (obrigatórias)

| Ferramenta | Função | Instalação |
|---|---|---|
| xclip | Copiar para as áreas de transferência do X11 | `sudo apt install xclip` |
| notify-send | Notificações na área de trabalho | `sudo apt install libnotify-bin` |

### GPU NVIDIA (opcional, mas essencial para o desempenho)

Sem CUDA, conte com **10 a 15 segundos** de processamento para 15 segundos de fala. Com CUDA, **menos de 1 segundo**. Isso representa uma aceleração de **10 a 50 vezes**.

| Componente | Mínimo | Verificação |
|---|---|---|
| Driver NVIDIA | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

Se o `nvidia-smi` funciona mas o `nvcc` não, instale o CUDA Toolkit:

```bash
sudo apt install nvidia-cuda-toolkit
```

Se a versão do CUDA mostrada pelo `nvidia-smi` for **inferior** à informada pelo `nvcc`, atualize o driver:

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## Instalação

```bash
# 1. Dependências do sistema
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. Clonar o repositório
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. Compilar o whisper.cpp e baixar o modelo
make setup

# 4. Compilar os binários: opons-voxd (o daemon) e opons-voxd-mcp (servidor MCP opcional)
make

# 5. Executar
./opons-voxd
```

---

## Liberar a GPU para IA local (servidor MCP)

Os modelos Whisper ficam na memória da GPU o tempo todo (cerca de 3,8 GB para `large-v3-turbo` + `medium`). Quando você precisa dessa VRAM para outra coisa — um LLM local, um modelo de imagem, um treinamento —, o opons-voxd pode descarregá-los e recarregá-los depois, sem ser encerrado. O descarregamento também destrói o contexto CUDA do processo, de modo que o opons-voxd desaparece completamente do `nvidia-smi` (0 MB de VRAM). O recarregamento leva menos de um segundo se os arquivos dos modelos ainda estiverem no cache de páginas; caso contrário, alguns segundos.

**Estados do ícone na bandeja:**

| Ícone | Estado |
|:---:|---|
| anel branco | modelos carregados, pronto |
| disco vermelho | gravando |
| disco laranja | transcrevendo |
| anel cinza cortado por uma barra (⊘) | modelos descarregados, memória da GPU liberada, ditado desativado |
| arco azul de três quartos | modelos sendo recarregados |

Com os modelos descarregados, clicar no ícone ou pressionar um atalho push-to-talk mostra uma notificação de erro em vez de gravar.

**Manualmente:** clique com o botão direito no ícone da bandeja > *Unload models (free GPU memory)* (descarregar os modelos), depois em *Reload models* (recarregar os modelos).

**A partir de um agente de IA (MCP):** inicie o daemon com `-m` / `--mcp` (desativado por padrão). O `make` também compila o `opons-voxd-mcp`, um servidor [Model Context Protocol](https://modelcontextprotocol.io) via stdio, vinculado apenas à libc (cerca de 1,5 MB de RAM). Ele expõe três ferramentas: `unload_models`, `reload_models` e `get_status`. Com o Claude Code:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

A maioria dos outros clientes (Claude Desktop, Cursor, LM Studio...) usa este JSON:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Todos os detalhes no [README principal](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Atalho push-to-talk

O `launch.sh` incluído usa `ctrl+alt+w` (recomendado em AZERTY) ou `ctrl+alt+z` (recomendado em QWERTY). Essa tecla fica ao lado do `A`, então a combinação inteira pode ser feita só com a mão esquerda (mindinho no `Ctrl`, polegar no `Alt`, anelar ou médio no `W`/`Z`) sem desfazer a postura de digitação, e ela não está associada a nada por padrão na maioria dos ambientes de desktop. O atalho é configurado com `OPONS_VOXD_PTT_HOTKEY`. Todos os detalhes no [README principal](../README.md#push-to-talk-hotkey).

---

## Licença

Tudo está sob a licença MIT: o modelo Whisper (OpenAI), o whisper.cpp (Georgi Gerganov) e este projeto.

Documentação completa: veja o [README principal](../README.md).

---

## Estilo de código

Este projeto segue o **estilo de código do kernel Linux** (`Documentation/process/coding-style.rst`), com adaptações de Olivier Pons: chaves K&R, indentação de 4 espaços, `struct name` sem typedef, sem prefixos (`s_`, `t_`, `e_`), `return value;` sem parênteses, comentários kernel-doc, no máximo 80 caracteres por linha. Todos os detalhes no [README principal](../README.md).
