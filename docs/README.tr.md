# opons-voxd

> Not: opons-voxd'nin jakovius/voxd ile hiçbir ilgisi yoktur. Ad bağımsız olarak seçildi — `opons` = O. Pons (yazar), `voxd` = vox + daemon (Latin/Unix geleneği).

> **⚠ Geriye dönük uyumluluk bilerek bozuldu.** Bu projenin eski adı `voice_in_linux` idi. Ad değişikliği ikili dosyanın adını, kaynak dosyanın adını, tüm ortam değişkenlerini (`VOICE_IN_*` → `OPONS_VOXD_*`) ve çalışma zamanındaki `xdotool` bağımlılığını etkiledi; bu bağımlılığın yerini `libxtst-dev` üzerinden doğrudan XTest sentezi aldı. Takma ad yok, geçiş süresi yok — başlatma betiklerinizi, otomatik başlatma girdilerinizi ve `apt install` satırlarınızı güncelleyin.

**Linux için yerel sesle dikte — sesiniz bilgisayarınızdan hiç çıkmaz.**

🌍 **Çeviriler:**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md) · [한국어](README.ko.md)

📖 **[İş Akışım: Kendi Dilinizde Konuşun, İngilizce Yazılsın](WORKFLOW.tr.md)** — gerçek bir kullanım örneği

---

## Neden?

Linux'ta sesimi üçüncü taraf bir sunucuya göndermeden herhangi bir uygulamaya metin dikte etmek istiyordum. Bulduğum çözümlerin hepsi — tarayıcı eklentileri, bulut API'leri, SaaS platformları — yazıya dökmek için sesinizi uzak sunuculara aktarıyor. Yani söylediğiniz her kelime internetten geçiyor: e-postalar, gizli belgeler, müşteri verileri, kişisel notlar, hepsi.

Bu proje hafif ve tamamen yerel bir alternatif. Bütünüyle sizin bilgisayarınızda çalışır ve OpenAI'ın açık kaynaklı konuşma tanıma modeli Whisper'ın bir uygulaması olan [whisper.cpp](https://github.com/ggerganov/whisper.cpp)'yi kullanır. İlk kurulumdan sonra ağ bağlantısı gerekmez. Hiçbir veri bilgisayarınızdan çıkmaz.

Yapay zekâ yardımıyla geliştirildi.

---

## Çalışma şekli

Aynı anda kullanılabilen iki dikte yöntemi var.

### Tepsi simgesi modu — aç/kapat, panoya kopyala

1. Sistem tepsisinde küçük bir simge belirir
2. **Sol tık** → kayıt başlar (simge kırmızıya döner)
3. **Tekrar sol tık** → kayıt durur, ses Whisper ile yerel olarak yazıya dökülür
4. Yazıya dökülen metin otomatik olarak panoya kopyalanır (X11'in iki seçimine de)
5. Bir masaüstü bildirimi metni 10 saniye boyunca gösterir
6. `Ctrl+Shift+V`, `Shift+Insert` ya da orta tıkla istediğiniz yere yapıştırın

Simgeye **sağ tıklayınca** Toggle / Unload models / Reload models / Quit seçeneklerini içeren bir menü açılır (bkz. [Yerel yapay zekâ için GPU'yu boşaltma](#yerel-yapay-zekâ-için-gpuyu-boşaltma-mcp-sunucusu)).

### Push-to-talk modu — kısayolu basılı tutun, metin imlece yazılsın

1. **`Ctrl+Shift+Space` tuşlarını basılı tutun** → kayıt başlar (simge kırmızıya döner)
2. **Tuşları bırakın** → kayıt durur, ses yazıya dökülür
3. Yazıya dökülen metin, odaktaki pencerede **doğrudan klavye imlecinin bulunduğu yere yazılır** (X11 XTest eklentisi aracılığıyla)

Kısayol `OPONS_VOXD_PTT_HOTKEY` ile ayarlanabilir. İkinci, bağımsız bir kısayol (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, varsayılan `ctrl+alt+t`) aynı şekilde çalışır ama bunun yerine İngilizce bir çeviri yazar — bkz. [ana README](../README.md#push-to-talk-and-translate-hotkey).

### Ekran görüntüleri

| Boşta | Kayıt sırasında |
|:---:|:---:|
| ![Boştaki simge](../screenshots/opons_voxd_inactive.png) | ![Kayıt sırasındaki simge](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **Kabul edilebilir yanıt süreleri için CUDA destekli bir NVIDIA GPU şart.**
> CUDA olmadan 15 saniyelik konuşmanın yazıya dökülmesi **10–15 saniye** sürer (yalnızca CPU).
> CUDA etkinken aynı işlem **1 saniyeden kısa** sürer.

---

## Özellikler

- **%100 yerel** — ses CPU'nuzda ya da GPU'nuzda işlenir, hiçbir yere gönderilmez
- **GPU hızlandırma** — neredeyse anlık yazıya dökme için NVIDIA CUDA desteği
- **99 dil** — OpenAI Whisper sayesinde
- **Hafiflik** — saf C, Python yok: daemon ve isteğe bağlı olarak yalnızca libc'ye bağlı 25 KB'lık bir MCP sunucusu
- **Sistem tepsisi entegrasyonu** — görev çubuğunda göze batmayan bir simge
- **Push-to-talk kısayolu** — kaydetmek için `Ctrl+Shift+Space` tuşlarını (ayarlanabilir) basılı tutun, dökümün klavye imlecine yazılması için bırakın
- **Push-to-talk ve çeviri kısayolu** — bağımsız olarak ayarlanabilen ikinci bir kısayol, konuşmanızın birebir dökümü yerine İngilizce çevirisini yazar
- **Çift pano** — metin X11'in hem PRIMARY hem CLIPBOARD seçimine gönderilir
- **Masaüstü bildirimleri** — yazıya dökülen metin bir bildirimde gösterilir
- **Sesli komutlar** — `commands/` içinde dil başına bir komut dosyası, varsayılan olarak kapalı (`OPONS_VOXD_COMMANDS=1` ile açılır). Yeni bir dil eklemek için `commands/xx.txt` dosyasını oluşturmanız yeterli, yeniden derleme gerekmez.
- **Geçici bildirimler** — varsayılan olarak bildirimler görünür, sonra tamamen kaybolur. `OPONS_VOXD_NOTIFY_PERSIST=1` onları geçmişte tutar.
- **Bildirim modu** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` başarı bildirimlerini denetler (varsayılan `quiet`: balon, ses yok). Hatalar her modda, istisnasız, her zaman sesli gösterilir. Tam tablo ana README'de.
- **Otomatik büyük harf** — cümleler otomatik olarak büyük harfle başlar
- **İstek üzerine GPU boşaltma** — Whisper modellerini VRAM'den kaldırın (tepsi menüsünden ya da yerleşik MCP sunucusu aracılığıyla herhangi bir yapay zekâ ajanıyla) ve işiniz bitince yeniden yükleyin
- **Otomatik başlatma** — oturum açılışında başlayacak şekilde ayarlanabilir

---

## Gereksinimler

### İşletim sistemi

| Gereksinim | En az | Kontrol |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Linux çekirdeği | 5.15+ | `uname -r` |
| Görüntü sunucusu | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland desteklenmez (GtkStatusIcon X11 gerektirir). Linux Mint varsayılan olarak X11 kullanır.

### Derleme araçları (zorunlu)

| Paket | En az | Kurulum |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### Geliştirme kütüphaneleri (zorunlu)

| Paket | En az | Kurulum |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> `libportaudio-dev` gerekmez: derleme `portaudio.h` dosyasını kendisi indirir (`curl` ile, ilk `make` sırasında) ve doğrudan `libportaudio.so.2` ile bağlanır. Ayrıntılar [ana README'de](../README.md#development-libraries-required).

### Çalışma zamanı araçları (zorunlu)

| Araç | Amaç | Kurulum |
|---|---|---|
| xclip | X11 panolarına kopyalama | `sudo apt install xclip` |
| notify-send | Masaüstü bildirimleri | `sudo apt install libnotify-bin` |

### NVIDIA GPU (isteğe bağlı ama performans için kritik)

CUDA olmadan 15 saniyelik konuşma için **10–15 saniye** işlem süresi bekleyin. CUDA ile **1 saniyeden kısa**. Bu, **10 ila 50 kat** hızlanma demek.

| Bileşen | En az | Kontrol |
|---|---|---|
| NVIDIA sürücüsü | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

`nvidia-smi` çalışıyor ama `nvcc` çalışmıyorsa CUDA Toolkit'i kurun:

```bash
sudo apt install nvidia-cuda-toolkit
```

`nvidia-smi` komutunun gösterdiği CUDA sürümü `nvcc` komutunun bildirdiğinden **düşükse** sürücüyü güncelleyin:

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## Kurulum

```bash
# 1. Sistem bağımlılıkları
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. Depoyu klonlama
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. whisper.cpp'yi derleme ve modeli indirme
make setup

# 4. İkili dosyaları derleme: opons-voxd (daemon) ve opons-voxd-mcp (isteğe bağlı MCP sunucusu)
make

# 5. Çalıştırma
./opons-voxd
```

---

## Yerel yapay zekâ için GPU'yu boşaltma (MCP sunucusu)

Whisper modelleri GPU belleğinde sürekli durur (`large-v3-turbo` + `medium` için yaklaşık 3,8 GB). Bu VRAM'e başka bir iş için ihtiyacınız olduğunda — yerel bir LLM, bir görüntü modeli, bir eğitim çalışması — opons-voxd kapanmadan modelleri bellekten kaldırıp sonra yeniden yükleyebilir. Kaldırma işlemi sürecin CUDA bağlamını da yok eder; böylece opons-voxd `nvidia-smi` çıktısından tamamen kaybolur (0 MB VRAM). Model dosyaları hâlâ sayfa önbelleğindeyse yeniden yükleme bir saniyeden kısa sürer, değilse birkaç saniye.

**Tepsi simgesinin durumları:**

| Simge | Durum |
|:---:|---|
| beyaz halka | modeller yüklü, hazır |
| kırmızı disk | kayıt |
| turuncu disk | yazıya dökme |
| gri, üstü çizili halka (⊘) | modeller kaldırıldı, GPU belleği boşaltıldı, dikte devre dışı |
| mavi, dörtte üçlük yay | modeller yeniden yükleniyor |

Modeller kaldırılmışken simgeye tıklamak ya da bir push-to-talk kısayoluna basmak kayıt başlatmaz, bunun yerine bir hata bildirimi gösterir.

**Elle:** tepsi simgesine sağ tıklayın > *Unload models (free GPU memory)* (modelleri kaldır), ardından *Reload models* (modelleri yeniden yükle).

**Bir yapay zekâ ajanından (MCP):** daemon'u `-m` / `--mcp` ile başlatın (varsayılan olarak kapalı). `make`, `opons-voxd-mcp` programını da derler: stdio üzerinden çalışan, yalnızca libc'ye bağlı (yaklaşık 1,5 MB RAM) bir [Model Context Protocol](https://modelcontextprotocol.io) sunucusu. Üç araç sunar: `unload_models`, `reload_models` ve `get_status`. Claude Code ile:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

Diğer istemcilerin çoğu (Claude Desktop, Cursor, LM Studio...) şu JSON'u kullanır:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Tüm ayrıntılar [ana README'de](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Push-to-talk kısayolu

Pakette gelen `launch.sh`, `ctrl+alt+w` (AZERTY için önerilir) ya da `ctrl+alt+z` (QWERTY için önerilir) kullanır. Bu tuş `A` tuşunun hemen yanındadır, bu yüzden yazma duruşunuzu bozmadan kombinasyonun tamamına tek sol elle ulaşabilirsiniz (serçe parmak `Ctrl` üzerinde, başparmak `Alt` üzerinde, yüzük ya da orta parmak `W`/`Z` üzerinde). Ayrıca çoğu masaüstü ortamında bu kombinasyon varsayılan olarak hiçbir işleve atanmamıştır. Kısayol `OPONS_VOXD_PTT_HOTKEY` ile ayarlanır. Tüm ayrıntılar [ana README'de](../README.md#push-to-talk-hotkey).

---

## Lisans

Her şey MIT lisansı altındadır: Whisper modeli (OpenAI), whisper.cpp (Georgi Gerganov) ve bu proje.

Belgelerin tamamı için [ana README'ye](../README.md) bakın.

---

## Kod stili

Bu proje, Olivier Pons'un uyarlamalarıyla **Linux çekirdeği kod stilini** (`Documentation/process/coding-style.rst`) izler: K&R süslü parantezleri, 4 boşluk girinti, typedef'siz `struct name`, önek yok (`s_`, `t_`, `e_`), parantezsiz `return value;`, kernel-doc yorumları, satır başına en fazla 80 karakter. Tüm ayrıntılar [ana README'de](../README.md).
