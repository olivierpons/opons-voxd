# opons-voxd

> Lưu ý: opons-voxd không liên quan gì đến jakovius/voxd. Cái tên được chọn hoàn toàn độc lập — `opons` = O. Pons (tác giả), `voxd` = vox + daemon (theo truyền thống La-tinh/Unix).

> **⚠ Chủ động phá vỡ khả năng tương thích ngược.** Dự án này trước đây có tên `voice_in_linux`. Việc đổi tên ảnh hưởng đến tên tệp nhị phân, tên tệp mã nguồn, mọi biến môi trường (`VOICE_IN_*` → `OPONS_VOXD_*`) và phụ thuộc lúc chạy vào `xdotool` (nay được thay bằng việc tổng hợp phím trực tiếp qua XTest với `libxtst-dev`). Không có bí danh, không có giai đoạn chuyển tiếp — hãy cập nhật các script khởi chạy, mục tự khởi động và các dòng `apt install` của bạn.

**Đọc chính tả cục bộ cho Linux — giọng nói của bạn không bao giờ rời khỏi máy.**

🌍 **Bản dịch:**
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [한국어](README.ko.md)

📖 **[Quy trình làm việc của tôi: nói tiếng mẹ đẻ, ra chữ tiếng Anh](WORKFLOW.vi.md)** — một trường hợp sử dụng thực tế

---

## Vì sao có dự án này?

Tôi muốn đọc chính tả văn bản vào bất kỳ ứng dụng nào trên Linux mà không phải gửi giọng nói của mình lên máy chủ của bên thứ ba. Mọi giải pháp có sẵn mà tôi tìm được — tiện ích mở rộng trình duyệt, API đám mây, nền tảng SaaS — đều truyền âm thanh của bạn lên máy chủ từ xa để chuyển thành văn bản. Nghĩa là mọi lời bạn nói đều đi qua Internet: email, tài liệu mật, dữ liệu khách hàng, ghi chú cá nhân — tất cả.

Dự án này là một giải pháp thay thế gọn nhẹ và hoàn toàn cục bộ. Nó chạy trọn vẹn trên máy của bạn nhờ [whisper.cpp](https://github.com/ggerganov/whisper.cpp), công cụ chạy mô hình nhận dạng giọng nói mã nguồn mở của OpenAI. Sau lần cài đặt ban đầu, không cần kết nối mạng. Không có dữ liệu nào rời khỏi máy tính của bạn.

Được xây dựng với sự hỗ trợ của AI.

---

## Cách hoạt động

Có hai cách đọc chính tả, dùng được cùng lúc.

### Chế độ biểu tượng khay hệ thống — nhấp để bật/tắt, sao chép vào bộ nhớ tạm

1. Một biểu tượng nhỏ xuất hiện trong khay hệ thống
2. **Nhấp chuột trái** → bắt đầu ghi âm (biểu tượng chuyển sang màu đỏ)
3. **Nhấp chuột trái lần nữa** → dừng ghi âm, âm thanh được Whisper chuyển thành văn bản ngay trên máy
4. Văn bản được tự động sao chép vào bộ nhớ tạm (cả hai vùng chọn X11)
5. Một thông báo trên màn hình hiển thị văn bản trong 10 giây
6. Dán vào bất kỳ đâu bằng `Ctrl+Shift+V`, `Shift+Insert` hoặc nhấp chuột giữa

**Nhấp chuột phải** vào biểu tượng để mở menu Toggle / Unload models / Reload models / Quit (xem [Giải phóng GPU cho AI cục bộ](#giải-phóng-gpu-cho-ai-cục-bộ-máy-chủ-mcp)).

### Chế độ push-to-talk — giữ phím tắt, gõ tại con trỏ

1. **Giữ `Ctrl+Shift+Space`** → bắt đầu ghi âm (biểu tượng chuyển sang màu đỏ)
2. **Thả phím** → dừng ghi âm, âm thanh được chuyển thành văn bản
3. Văn bản được **gõ thẳng tại vị trí con trỏ bàn phím** (qua phần mở rộng XTest của X11), vào cửa sổ đang có tiêu điểm

Phím tắt có thể cấu hình qua `OPONS_VOXD_PTT_HOTKEY`. Một phím tắt thứ hai, độc lập (`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, mặc định `ctrl+alt+t`), hoạt động y hệt nhưng gõ ra bản dịch tiếng Anh — xem [README chính](../README.md#push-to-talk-and-translate-hotkey).

### Ảnh chụp màn hình

| Đang chờ | Đang ghi âm |
|:---:|:---:|
| ![opons-voxd đang chờ](../screenshots/opons_voxd_inactive.png) | ![opons-voxd đang ghi âm](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **GPU NVIDIA có CUDA là yếu tố then chốt để có thời gian phản hồi dùng được.**
> Không có CUDA, việc chuyển 15 giây giọng nói thành văn bản mất **10–15 giây** (chỉ dùng CPU).
> Khi bật CUDA, việc chuyển đổi tương tự chỉ mất **chưa đến 1 giây**.

---

## Tính năng

- **Cục bộ 100%** — âm thanh được xử lý trên CPU hoặc GPU của bạn, không bao giờ gửi đi đâu
- **Tăng tốc bằng GPU** — hỗ trợ NVIDIA CUDA, chuyển thành văn bản gần như tức thì
- **99 ngôn ngữ** — nhờ OpenAI Whisper
- **Gọn nhẹ** — C thuần, không cần Python: chỉ có daemon, cộng thêm một máy chủ MCP tùy chọn nặng 25 KB, chỉ liên kết với libc
- **Tích hợp khay hệ thống** — biểu tượng kín đáo trên thanh tác vụ
- **Phím tắt push-to-talk** — giữ `Ctrl+Shift+Space` (có thể cấu hình) để ghi âm, thả ra để văn bản được gõ tại con trỏ bàn phím
- **Phím tắt push-to-talk kèm dịch** — phím tắt thứ hai, cấu hình độc lập, gõ ra bản dịch tiếng Anh của lời bạn nói thay vì bản ghi nguyên văn
- **Bộ nhớ tạm kép** — văn bản được đưa vào cả hai vùng chọn X11 PRIMARY và CLIPBOARD
- **Thông báo trên màn hình** — văn bản đã chuyển đổi được hiển thị dưới dạng thông báo
- **Lệnh bằng giọng nói** — mỗi ngôn ngữ một tệp lệnh trong `commands/`, tắt theo mặc định (`OPONS_VOXD_COMMANDS=1`). Thêm một ngôn ngữ chỉ bằng cách tạo `commands/xx.txt`, không cần biên dịch lại.
- **Thông báo tạm thời** — theo mặc định, thông báo hiện lên rồi biến mất hoàn toàn. `OPONS_VOXD_NOTIFY_PERSIST=1` giữ chúng lại trong lịch sử.
- **Chế độ thông báo** — `OPONS_VOXD_NOTIFY=normal|quiet|silent|off` điều khiển thông báo khi thành công (mặc định `quiet`: có bong bóng, không có âm thanh). Lỗi luôn được hiển thị kèm âm thanh, ở mọi chế độ, không có ngoại lệ. Xem bảng đầy đủ trong README chính.
- **Tự động viết hoa** — chữ đầu câu được viết hoa tự động
- **Giải phóng GPU khi cần** — gỡ các mô hình Whisper khỏi VRAM (từ menu khay, hoặc từ bất kỳ tác tử AI nào qua máy chủ MCP tích hợp) rồi nạp lại khi xong việc
- **Tự khởi động** — có thể cấu hình để chạy khi đăng nhập

---

## Yêu cầu

### Hệ điều hành

| Yêu cầu | Tối thiểu | Kiểm tra |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Nhân Linux | 5.15+ | `uname -r` |
| Máy chủ hiển thị | X11 | `echo $XDG_SESSION_TYPE` |

> Không hỗ trợ Wayland (GtkStatusIcon cần X11). Linux Mint dùng X11 theo mặc định.

### Công cụ biên dịch (bắt buộc)

| Gói | Tối thiểu | Cài đặt |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### Thư viện phát triển (bắt buộc)

| Gói | Tối thiểu | Cài đặt |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> Không cần `libportaudio-dev`: quá trình biên dịch tự tải `portaudio.h` (bằng `curl`, ở lần `make` đầu tiên) và liên kết trực tiếp với `libportaudio.so.2`. Chi tiết trong [README chính](../README.md#development-libraries-required).

### Công cụ lúc chạy (bắt buộc)

| Công cụ | Mục đích | Cài đặt |
|---|---|---|
| xclip | Sao chép vào bộ nhớ tạm X11 | `sudo apt install xclip` |
| notify-send | Thông báo trên màn hình | `sudo apt install libnotify-bin` |

### GPU NVIDIA (không bắt buộc nhưng rất quan trọng cho hiệu năng)

Không có CUDA, bạn sẽ phải chờ **10–15 giây** để xử lý 15 giây giọng nói. Có CUDA, **chưa đến 1 giây**. Tức là nhanh hơn **10–50 lần**.

| Thành phần | Tối thiểu | Kiểm tra |
|---|---|---|
| Trình điều khiển NVIDIA | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

Nếu `nvidia-smi` chạy được nhưng `nvcc` thì không, hãy cài CUDA Toolkit:

```bash
sudo apt install nvidia-cuda-toolkit
```

Nếu phiên bản CUDA mà `nvidia-smi` hiển thị **thấp hơn** phiên bản `nvcc` báo cáo, hãy cập nhật trình điều khiển:

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## Cài đặt

```bash
# 1. Các gói phụ thuộc của hệ thống
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. Clone kho mã nguồn
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. Biên dịch whisper.cpp và tải mô hình
make setup

# 4. Biên dịch các tệp thực thi: opons-voxd (daemon) và opons-voxd-mcp (máy chủ MCP tùy chọn)
make

# 5. Chạy
./opons-voxd
```

---

## Giải phóng GPU cho AI cục bộ (máy chủ MCP)

Các mô hình Whisper luôn nằm trong bộ nhớ GPU (khoảng 3,8 GB cho `large-v3-turbo` + `medium`). Khi bạn cần dùng lượng VRAM đó cho việc khác — một LLM cục bộ, một mô hình tạo ảnh, một lượt huấn luyện — opons-voxd có thể gỡ chúng ra rồi nạp lại sau, mà không cần thoát. Việc gỡ mô hình cũng hủy luôn ngữ cảnh CUDA của tiến trình, nên opons-voxd biến mất hoàn toàn khỏi `nvidia-smi` (0 MB VRAM). Nạp lại mất chưa đến một giây nếu các tệp mô hình vẫn còn trong page cache, nếu không thì vài giây.

**Trạng thái của biểu tượng khay:**

| Biểu tượng | Trạng thái |
|:---:|---|
| vòng tròn trắng | đã nạp mô hình, sẵn sàng |
| chấm tròn đỏ | đang ghi âm |
| chấm tròn cam | đang chuyển thành văn bản |
| vòng tròn xám có gạch chéo (⊘) | đã gỡ mô hình, đã giải phóng bộ nhớ GPU, tắt đọc chính tả |
| cung tròn ba phần tư màu xanh lam | đang nạp lại mô hình |

Khi mô hình đã được gỡ, nhấp vào biểu tượng hoặc nhấn phím tắt push-to-talk sẽ hiện thông báo lỗi thay vì ghi âm.

**Thủ công:** nhấp chuột phải vào biểu tượng khay > *Unload models (free GPU memory)* (gỡ mô hình, giải phóng bộ nhớ GPU), rồi *Reload models* (nạp lại mô hình).

**Từ một tác tử AI (MCP):** khởi động daemon với `-m` / `--mcp` (tắt theo mặc định). `make` cũng biên dịch `opons-voxd-mcp`, một máy chủ [Model Context Protocol](https://modelcontextprotocol.io) giao tiếp qua stdio, chỉ liên kết với libc (khoảng 1,5 MB RAM). Nó cung cấp ba công cụ: `unload_models`, `reload_models` và `get_status`. Với Claude Code:

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

Phần lớn các ứng dụng khách khác (Claude Desktop, Cursor, LM Studio...) dùng đoạn JSON này:

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

Chi tiết đầy đủ trong [README chính](../README.md#free-the-gpu-for-local-ai-mcp-server).

---

## Phím tắt push-to-talk

Tệp `launch.sh` đi kèm dùng `ctrl+alt+w` (khuyên dùng với bàn phím AZERTY) hoặc `ctrl+alt+z` (khuyên dùng với bàn phím QWERTY). Phím này nằm ngay cạnh `A`, nên có thể bấm cả tổ hợp chỉ bằng tay trái (ngón út trên `Ctrl`, ngón cái trên `Alt`, ngón áp út hoặc ngón giữa trên `W`/`Z`) mà không phá vỡ tư thế gõ phím, và ở hầu hết các môi trường desktop, tổ hợp này chưa được gán sẵn cho chức năng nào. Cấu hình bằng `OPONS_VOXD_PTT_HOTKEY`. Chi tiết đầy đủ trong [README chính](../README.md#push-to-talk-hotkey).

---

## Giấy phép

Mọi thành phần đều theo giấy phép MIT: mô hình Whisper (OpenAI), whisper.cpp (Georgi Gerganov) và dự án này.

Tài liệu đầy đủ: xem [README chính](../README.md).

---

## Phong cách viết mã

Dự án này tuân theo **phong cách viết mã của nhân Linux** (`Documentation/process/coding-style.rst`) với một số điều chỉnh của Olivier Pons: dấu ngoặc nhọn kiểu K&R, thụt lề 4 dấu cách, `struct name` không dùng typedef, không dùng tiền tố (`s_`, `t_`, `e_`), `return value;` không có dấu ngoặc đơn, chú thích kiểu kernel-doc, tối đa 80 ký tự mỗi dòng. Chi tiết đầy đủ trong [README chính](../README.md).
