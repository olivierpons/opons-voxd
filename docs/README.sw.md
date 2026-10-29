# opons-voxd

> Kumbuka: opons-voxd haihusiani na jakovius/voxd. Jina hili lilichaguliwa kwa kujitegemea — `opons` = O. Pons (mwandishi), `voxd` = vox + daemon (desturi ya Kilatini na Unix).

> **⚠ Uoanifu na matoleo ya awali umevunjwa kwa makusudi.** Mradi huu hapo awali uliitwa `voice_in_linux`. Kubadilisha jina kuligusa jina la programu (binary), jina la faili la msimbo chanzo, kila kigezo cha mazingira (`VOICE_IN_*` → `OPONS_VOXD_*`), na utegemezi wa `xdotool` wakati wa kuendesha (sasa vitufe vinazalishwa moja kwa moja kupitia XTest, kwa `libxtst-dev`). Hakuna majina mbadala, hakuna kipindi cha mpito — sasisha skripti zako za kuanzisha, maingizo ya kuanzisha kiotomatiki, na mistari ya `apt install`.

**Uandishi wa sauti kwa Linux unaofanyika wote kwenye kompyuta yako — sauti yako haiondoki kamwe kwenye mashine yako.**

🌍 **Tafsiri:**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md) · [한국어](README.ko.md)

📖 **[Mtiririko wangu wa kazi: ongea kwa lugha yako, andika kwa Kiingereza](WORKFLOW.sw.md)** — mfano halisi wa matumizi

---

## Kwa nini?

Nilitaka kuandika kwa sauti katika programu yoyote kwenye Linux bila kutuma sauti yangu kwenye seva ya mtu mwingine. Kila suluhisho nililopata — viendelezi vya kivinjari, API za wingu, majukwaa ya SaaS — hutuma sauti yako kwenye seva za mbali ili inukuliwe. Maana yake ni kwamba kila neno unalotamka hupita kwenye intaneti: barua pepe, hati za siri, data za wateja, madokezo binafsi — vyote.

Mradi huu ni mbadala mwepesi unaofanya kazi wote kwenye kompyuta yako. Unaendeshwa kikamilifu kwenye mashine yako kwa kutumia [whisper.cpp](https://github.com/ggerganov/whisper.cpp), injini ya ndani inayoendesha Whisper — modeli huria ya OpenAI ya kutambua matamshi. Hakuna muunganisho wa mtandao unaohitajika baada ya usanidi wa kwanza. Hakuna data yoyote inayotoka kwenye kompyuta yako.

Umetengenezwa kwa msaada wa akili bandia (AI).

---

## Jinsi unavyofanya kazi

Njia mbili za kuandika kwa sauti, zinazopatikana kwa wakati mmoja.

### Hali ya aikoni ya trei — kuwasha na kuzima, kunakili kwenye ubao wa kunakili

1. Aikoni ndogo inaonekana kwenye trei ya mfumo
2. **Bofya kushoto** → kurekodi kunaanza (aikoni inakuwa nyekundu)
3. **Bofya kushoto tena** → kurekodi kunasimama, sauti inanukuliwa kwenye kompyuta yako kupitia Whisper
4. Maandishi yaliyonukuliwa yananakiliwa kiotomatiki kwenye ubao wa kunakili (kwenye uteuzi wote wawili wa X11)
5. Arifa ya eneo-kazi inaonyesha maandishi kwa sekunde 10
6. Bandika popote kwa `Ctrl+Shift+V`, `Shift+Insert`, au kwa kubofya kitufe cha kati cha kipanya

**Bofya kulia** aikoni ili kupata menyu yenye Toggle / Unload models / Reload models / Quit (angalia [Kuachilia GPU kwa ajili ya AI ya ndani](#kuachilia-gpu-kwa-ajili-ya-ai-ya-ndani-seva-ya-mcp)).

### Hali ya push-to-talk — kushikilia kitufe cha mkato, kuandika mahali pa kishale

1. **Shikilia `Ctrl+Shift+Space`** → kurekodi kunaanza (aikoni inakuwa nyekundu)
2. **Achia vitufe** → kurekodi kunasimama, sauti inanukuliwa
3. Maandishi yaliyonukuliwa **yanaandikwa moja kwa moja mahali pa kishale cha kibodi** (kupitia kiendelezi cha X11 XTest) kwenye dirisha lolote lililo amilifu

Kitufe cha mkato kinaweza kubadilishwa kupitia `OPONS_VOXD_PTT_HOTKEY`. Kitufe cha pili cha mkato, kinachojitegemea (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, chaguo-msingi `ctrl+alt+t`), hufanya kazi vivyo hivyo lakini huandika tafsiri ya Kiingereza badala yake — angalia [README kuu](../README.md#push-to-talk-and-translate-hotkey).

### Picha za skrini

| Imetulia | Inarekodi |
|:---:|:---:|
| ![Aikoni ikiwa imetulia](../screenshots/opons_voxd_inactive.png) | ![Aikoni ikirekodi](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **GPU ya NVIDIA yenye CUDA ni muhimu sana ili muda wa kujibu ukubalike.**
> Bila CUDA, unukuzi huchukua **sekunde 10–15** kwa sekunde 15 za mazungumzo (CPU pekee).
> CUDA ikiwa imewashwa, unukuzi uleule huchukua **chini ya sekunde 1**.

---

## Vipengele

- **100% kwenye kompyuta yako** — sauti inachakatwa kwenye CPU au GPU yako, haitumwi popote
- **Kuongeza kasi kwa GPU** — usaidizi wa NVIDIA CUDA kwa unukuzi wa karibu papo hapo
- **Lugha 99** — kupitia OpenAI Whisper
- **Nyepesi** — C tupu, bila Python: daemon, pamoja na seva ya hiari ya MCP ya KB 25 iliyounganishwa na libc pekee
- **Kuunganishwa na trei ya mfumo** — aikoni isiyosumbua kwenye upau wa kazi
- **Kitufe cha mkato cha push-to-talk** — shikilia `Ctrl+Shift+Space` (kinaweza kubadilishwa) ili kurekodi, kisha achia ili maandishi yaandikwe mahali pa kishale cha kibodi
- **Kitufe cha mkato cha push-to-talk na tafsiri** — kitufe cha pili cha mkato, kinachowekwa kivyake, huandika tafsiri ya Kiingereza ya maneno yako badala ya unukuzi wa neno kwa neno
- **Ubao wa kunakili maradufu** — maandishi yanatumwa kwenye uteuzi wote wawili wa X11, PRIMARY na CLIPBOARD
- **Arifa za eneo-kazi** — maandishi yaliyonukuliwa huonyeshwa kama arifa
- **Amri za sauti** — faili moja la amri kwa kila lugha ndani ya `commands/`, zimezimwa kwa chaguo-msingi (`OPONS_VOXD_COMMANDS=1`). Ongeza lugha kwa kuunda `commands/xx.txt`, bila kukusanya (compile) upya.
- **Arifa za muda mfupi** — kwa chaguo-msingi arifa huonekana kisha hupotea kabisa. `OPONS_VOXD_NOTIFY_PERSIST=1` huzihifadhi kwenye historia.
- **Hali ya arifa** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` hudhibiti arifa za mafanikio (chaguo-msingi `quiet`: kiputo, bila sauti). Hitilafu huonyeshwa kila mara pamoja na sauti, katika hali zote, bila kighairi. Angalia README kuu kwa jedwali kamili.
- **Herufi kubwa kiotomatiki** — sentensi huanza kwa herufi kubwa kiotomatiki
- **Kuachilia GPU unapohitaji** — ondoa modeli za Whisper kwenye VRAM (kupitia menyu ya trei, au kwa wakala yeyote wa AI kupitia seva ya MCP iliyojengwa ndani) kisha uzipakie upya ukimaliza
- **Kuanza kiotomatiki** — inaweza kusanidiwa ianze unapoingia kwenye mfumo

---

## Mahitaji ya awali

### Mfumo wa uendeshaji

| Hitaji | Kiwango cha chini | Kukagua |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Kerneli ya Linux | 5.15+ | `uname -r` |
| Seva ya maonyesho | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland haiungwi mkono (GtkStatusIcon inahitaji X11). Linux Mint hutumia X11 kwa chaguo-msingi.

### Zana za kujenga (zinahitajika)

| Kifurushi | Kiwango cha chini | Kusakinisha |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### Maktaba za uundaji programu (zinahitajika)

| Kifurushi | Kiwango cha chini | Kusakinisha |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> `libportaudio-dev` haihitajiki: mchakato wa kujenga huleta `portaudio.h` (kupitia `curl`, kwenye `make` ya kwanza) na huunganisha moja kwa moja na `libportaudio.so.2`. Maelezo zaidi kwenye [README kuu](../README.md#development-libraries-required).

### Zana za wakati wa kuendesha (zinahitajika)

| Zana | Kazi yake | Kusakinisha |
|---|---|---|
| xclip | Kunakili kwenye ubao wa kunakili wa X11 | `sudo apt install xclip` |
| notify-send | Arifa za eneo-kazi | `sudo apt install libnotify-bin` |

### GPU ya NVIDIA (hiari, lakini muhimu sana kwa utendaji)

Bila CUDA, tarajia **sekunde 10–15** za uchakataji kwa sekunde 15 za mazungumzo. Ukiwa na CUDA, **chini ya sekunde 1**. Hiyo ni kasi zaidi kwa **mara 10–50**.

| Kipengele | Kiwango cha chini | Kukagua |
|---|---|---|
| Kiendeshi cha NVIDIA | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

Ikiwa `nvidia-smi` inafanya kazi lakini `nvcc` haifanyi, sakinisha CUDA toolkit:

```bash
sudo apt install nvidia-cuda-toolkit
```

Ikiwa toleo la CUDA linaloonyeshwa na `nvidia-smi` ni **la chini** kuliko lile linaloripotiwa na `nvcc`, sasisha kiendeshi:

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## Usakinishaji

```bash
# 1. Vitegemezi vya mfumo
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. Nakili hazina ya msimbo
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. Jenga whisper.cpp na upakue modeli
make setup

# 4. Jenga programu mbili: opons-voxd (daemon) na opons-voxd-mcp (seva ya hiari ya MCP)
make

# 5. Endesha
./opons-voxd
```

---

## Kuachilia GPU kwa ajili ya AI ya ndani (seva ya MCP)

Modeli za Whisper hukaa kwenye kumbukumbu ya GPU muda wote (takriban GB 3.8 kwa `large-v3-turbo` + `medium`). Unapohitaji VRAM hiyo kwa kazi nyingine — LLM ya ndani, modeli ya picha, mzunguko wa mafunzo — opons-voxd inaweza kuziondoa kwenye kumbukumbu na kuzipakia upya baadaye, bila kufunga programu. Kuziondoa pia huharibu muktadha wa CUDA wa mchakato huo, hivyo opons-voxd hutoweka kabisa kwenye `nvidia-smi` (MB 0 za VRAM). Kupakia upya huchukua chini ya sekunde moja ikiwa faili za modeli bado ziko kwenye page cache, na sekunde chache vinginevyo.

**Hali za aikoni ya trei:**

| Aikoni | Hali |
|:---:|---|
| pete nyeupe | modeli zimepakiwa, tayari |
| diski nyekundu | inarekodi |
| diski ya rangi ya chungwa | inanukuu |
| pete ya kijivu yenye mstari wa mshazari (⊘) | modeli zimeondolewa, kumbukumbu ya GPU imeachiliwa, uandishi wa sauti umezimwa |
| tao la buluu la robo tatu | modeli zinapakiwa upya |

Wakati modeli zimeondolewa, kubofya aikoni au kubonyeza kitufe cha mkato cha push-to-talk huonyesha arifa ya hitilafu badala ya kurekodi.

**Kwa mkono:** bofya kulia aikoni ya trei > *Unload models (free GPU memory)* (ondoa modeli, achilia kumbukumbu ya GPU), kisha *Reload models* (pakia modeli upya).

**Kutoka kwa wakala wa AI (MCP):** anzisha daemon kwa `-m` / `--mcp` (imezimwa kwa chaguo-msingi). `make` pia hujenga `opons-voxd-mcp`, seva ya [Model Context Protocol](https://modelcontextprotocol.io) inayowasiliana kupitia stdio, iliyounganishwa na libc pekee (takriban MB 1.5 za RAM). Inatoa zana tatu: `unload_models`, `reload_models` na `get_status`. Kwa Claude Code:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

Programu nyingine nyingi za mteja (Claude Desktop, Cursor, LM Studio...) hutumia JSON hii:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Maelezo kamili kwenye [README kuu](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Kitufe cha mkato cha push-to-talk

`launch.sh` inayokuja na mradi hutumia `ctrl+alt+w` (inapendekezwa kwa kibodi za AZERTY) au `ctrl+alt+z` (inapendekezwa kwa kibodi za QWERTY). Kitufe hicho kiko karibu na `A`, kwa hiyo mchanganyiko wote unafikiwa kwa mkono wa kushoto peke yake (kidole kidogo kwenye `Ctrl`, kidole gumba kwenye `Alt`, kidole cha pete au cha kati kwenye `W`/`Z`) bila kuvuruga mkao wako wa kuandika, na mazingira mengi ya eneo-kazi hayautumii kwa chaguo-msingi. Unaweza kuubadilisha kupitia `OPONS_VOXD_PTT_HOTKEY`. Maelezo kamili kwenye [README kuu](../README.md#push-to-talk-hotkey).

---

## Leseni

Kila kitu kiko chini ya leseni ya MIT: modeli ya Whisper (OpenAI), whisper.cpp (Georgi Gerganov), na mradi huu.

Hati kamili: angalia [README kuu](../README.md).

---

## Mtindo wa uandishi wa msimbo

Mradi huu unafuata **mtindo wa uandishi wa msimbo wa kerneli ya Linux** (`Documentation/process/coding-style.rst`) pamoja na marekebisho ya Olivier Pons: mabano ya mawimbi kwa mtindo wa K&R, ujongezaji wa nafasi 4, `struct name` bila typedef, bila viambishi awali (`s_`, `t_`, `e_`), `return value;` bila mabano ya duara, maoni ya kernel-doc, na herufi 80 kwa kila mstari kwa kiwango cha juu. Maelezo kamili kwenye [README kuu](../README.md).
