# opons-voxd

> 참고: opons-voxd는 jakovius/voxd와 관련이 없습니다. 이름은 독자적으로 지은 것으로, `opons`는 작성자 O. Pons, `voxd`는 vox + daemon(라틴어와 Unix의 명명 전통)에서 왔습니다.

> **⚠ 하위 호환성은 의도적으로 포기했습니다**. 이 프로젝트의 이전 이름은 `voice_in_linux`입니다. 이름을 바꾸면서 바이너리 이름, 소스 파일 이름, 모든 환경 변수(`VOICE_IN_*` → `OPONS_VOXD_*`)가 바뀌었고, 런타임의 `xdotool` 의존성도 사라졌습니다(`libxtst-dev`를 이용한 XTest 직접 입력 합성으로 대체). 별칭도 유예 기간도 없으니 실행 스크립트, 자동 시작 항목, `apt install` 줄을 수정하세요.

**Linux용 로컬 음성 받아쓰기. 여러분의 목소리는 절대 컴퓨터 밖으로 나가지 않습니다.**

🌍 **번역**:
[English](../README.md) · [Français](README.fr.md) · [Deutsch](README.de.md) · [中文](README.zh.md) · [日本語](README.ja.md) · [Español](README.es.md) · [Italiano](README.it.md) · [हिन्दी](README.hi.md) · [العربية](README.ar.md) · [বাংলা](README.bn.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [اردو](README.ur.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [मराठी](README.mr.md) · [తెలుగు](README.te.md) · [Türkçe](README.tr.md) · [தமிழ்](README.ta.md) · [Tiếng Việt](README.vi.md)

📖 **[내 워크플로: 모국어로 말하고 영어로 입력하기](WORKFLOW.ko.md)**(실제 활용 사례)

---

## 개발 동기

Linux의 어떤 애플리케이션에든 받아쓰기로 텍스트를 입력하고 싶었지만, 제 목소리를 제3자 서버로 보내고 싶지는 않았습니다. 찾아본 기존 솔루션(브라우저 확장 프로그램, 클라우드 API, SaaS 플랫폼)은 모두 전사를 위해 오디오를 원격 서버로 스트리밍합니다. 즉, 말하는 모든 단어가 인터넷을 거쳐 갑니다. 이메일, 기밀 문서, 고객 데이터, 개인 메모까지 전부요.

이 프로젝트는 가볍고 완전히 로컬에서 동작하는 대안입니다. OpenAI의 오픈 소스 음성 인식 모델을 구동하는 [whisper.cpp](https://github.com/ggerganov/whisper.cpp)를 사용해 모든 처리가 여러분의 컴퓨터 안에서 이루어집니다. 초기 설정 이후에는 네트워크 연결이 필요 없고, 어떤 데이터도 컴퓨터 밖으로 나가지 않습니다.

AI의 도움을 받아 만들었습니다.

---

## 작동 방식

두 가지 받아쓰기 방식을 동시에 사용할 수 있습니다.

### 트레이 아이콘 모드(클릭으로 켜고 끄기, 클립보드에 복사)

1. 시스템 트레이에 작은 아이콘이 나타납니다
2. **왼쪽 클릭** → 녹음 시작(아이콘이 빨간색으로 바뀜)
3. **한 번 더 왼쪽 클릭** → 녹음 중지, Whisper로 오디오를 로컬에서 전사
4. 전사된 텍스트가 자동으로 클립보드에 복사됩니다(X11의 두 선택 영역 모두)
5. 데스크톱 알림에 텍스트가 10초 동안 표시됩니다
6. `Ctrl+Shift+V`, `Shift+Insert` 또는 가운데 클릭으로 어디에든 붙여 넣을 수 있습니다

아이콘을 **오른쪽 클릭**하면 Toggle / Unload models / Reload models / Quit 메뉴가 열립니다([로컬 AI를 위한 GPU 메모리 확보](#로컬-ai를-위한-gpu-메모리-확보mcp-서버) 참고).

### 푸시투토크 모드(단축키를 누르고 있는 동안 녹음, 커서 위치에 입력)

1. **`Ctrl+Shift+Space`를 누르고 있기** → 녹음 시작(아이콘이 빨간색으로 바뀜)
2. **키에서 손 떼기** → 녹음 중지, 오디오 전사
3. 전사된 텍스트가 현재 포커스가 있는 창의 **키보드 커서 위치에 바로 입력**됩니다(X11 XTest 확장 사용)

단축키는 `OPONS_VOXD_PTT_HOTKEY`로 설정할 수 있습니다. 독립된 두 번째 단축키(`OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, 기본값 `ctrl+alt+t`)도 같은 방식으로 작동하지만, 대신 영어 번역문을 입력합니다. 자세한 내용은 [메인 README](../README.md#push-to-talk-and-translate-hotkey)를 참고하세요.

### 스크린샷

| 대기 중 | 녹음 중 |
|:---:|:---:|
| ![opons-voxd 대기 중](../screenshots/opons_voxd_inactive.png) | ![opons-voxd 녹음 중](../screenshots/opons_voxd_active.png) |

---

> ⚠️ **실용적인 응답 속도를 얻으려면 CUDA를 지원하는 NVIDIA GPU가 필수입니다**.
> CUDA가 없으면 15초 분량의 음성을 전사하는 데 **10～15초**가 걸립니다(CPU만 사용).
> CUDA를 켜면 같은 전사가 **1초 안에** 끝납니다.

---

## 기능

- **100% 로컬**: 오디오는 CPU나 GPU에서 처리되며 어디로도 전송되지 않습니다
- **GPU 가속**: NVIDIA CUDA를 지원해 거의 즉시 전사합니다
- **99개 언어**: OpenAI Whisper 기반
- **경량**: Python 없이 순수 C로 작성했습니다. 데몬과, libc에만 링크되는 25 KB짜리 MCP 서버(선택 사항)가 전부입니다
- **시스템 트레이 통합**: 작업 표시줄에 눈에 띄지 않는 아이콘을 표시합니다
- **푸시투토크 단축키**: `Ctrl+Shift+Space`(변경 가능)를 누르고 있으면 녹음되고, 손을 떼면 전사 결과가 키보드 커서 위치에 입력됩니다
- **번역 푸시투토크 단축키**: 따로 설정할 수 있는 두 번째 단축키로, 말한 내용을 그대로 전사하는 대신 영어로 번역해 입력합니다
- **이중 클립보드**: 텍스트를 X11의 PRIMARY와 CLIPBOARD 선택 영역 모두에 넣습니다
- **데스크톱 알림**: 전사된 텍스트를 알림으로 보여 줍니다
- **음성 명령**: `commands/`에 언어별로 명령 파일이 하나씩 있으며, 기본적으로 꺼져 있습니다(`OPONS_VOXD_COMMANDS=1`). `commands/xx.txt` 파일을 만들기만 하면 언어를 추가할 수 있고, 다시 컴파일할 필요가 없습니다.
- **일시적인 알림**: 기본적으로 알림은 표시된 뒤 완전히 사라집니다. `OPONS_VOXD_NOTIFY_PERSIST=1`로 설정하면 알림 기록에 남습니다.
- **알림 모드**: `OPONS_VOXD_NOTIFY=normal|quiet|silent|off`로 성공 알림을 제어합니다(기본값 `quiet`: 말풍선만, 소리 없음). 오류는 어떤 모드에서든 예외 없이 항상 소리와 함께 표시됩니다. 전체 표는 메인 README를 참고하세요.
- **자동 대문자화**: 문장 첫 글자를 자동으로 대문자로 바꿉니다
- **필요할 때 GPU 메모리 확보**: Whisper 모델을 VRAM에서 언로드하고(트레이 메뉴에서, 또는 내장 MCP 서버를 통해 어떤 AI 에이전트에서든), 작업이 끝나면 다시 로드합니다
- **자동 시작**: 로그인할 때 실행되도록 설정할 수 있습니다

---

## 사전 요구 사항

### 운영 체제

| 요구 사항 | 최소 버전 | 확인 방법 |
|---|---|---|
| Ubuntu / Linux Mint | 22.04 / 21 | `lsb_release -a` |
| Linux 커널 | 5.15+ | `uname -r` |
| 디스플레이 서버 | X11 | `echo $XDG_SESSION_TYPE` |

> Wayland는 지원하지 않습니다(GtkStatusIcon에 X11이 필요). Linux Mint는 기본으로 X11을 사용합니다.

### 빌드 도구(필수)

| 패키지 | 최소 버전 | 설치 |
|---|---|---|
| gcc | 9.0+ | `sudo apt install build-essential` |
| cmake | 3.14+ | `sudo apt install cmake` |
| pkg-config | 0.29+ | `sudo apt install pkg-config` |
| git | 2.25+ | `sudo apt install git` |

### 개발 라이브러리(필수)

| 패키지 | 최소 버전 | 설치 |
|---|---|---|
| libgtk-3-dev | 3.22+ | `sudo apt install libgtk-3-dev` |
| libnotify-dev | 0.7+ | `sudo apt install libnotify-dev` |
| libportaudio2 | 19.6+ | `sudo apt install libportaudio2` |
| libcairo2-dev | 1.14+ | `sudo apt install libcairo2-dev` |
| libx11-dev | 1.6+ | `sudo apt install libx11-dev` |
| libxtst-dev | 1.2+ | `sudo apt install libxtst-dev` |

> `libportaudio-dev`는 필요 없습니다. 빌드 과정에서 `portaudio.h`를 받아 오고(처음 `make`할 때 `curl`로) `libportaudio.so.2`에 직접 링크합니다. 자세한 내용은 [메인 README](../README.md#development-libraries-required)를 참고하세요.

### 런타임 도구(필수)

| 도구 | 용도 | 설치 |
|---|---|---|
| xclip | X11 클립보드에 복사 | `sudo apt install xclip` |
| notify-send | 데스크톱 알림 | `sudo apt install libnotify-bin` |

### NVIDIA GPU(선택 사항이지만 성능에 결정적)

CUDA가 없으면 15초 분량의 음성을 처리하는 데 **10～15초**가 걸립니다. CUDA가 있으면 **1초 미만**으로, **10～50배** 빨라집니다.

| 구성 요소 | 최소 버전 | 확인 방법 |
|---|---|---|
| NVIDIA 드라이버 | 570+ | `nvidia-smi` |
| CUDA Toolkit | 12.0+ | `nvcc --version` |

`nvidia-smi`는 작동하는데 `nvcc`가 작동하지 않으면 CUDA Toolkit을 설치하세요.

```bash
sudo apt install nvidia-cuda-toolkit
```

`nvidia-smi`에 표시되는 CUDA 버전이 `nvcc`가 보고하는 버전보다 **낮으면** 드라이버를 업데이트하세요.

```bash
ubuntu-drivers devices
sudo apt install nvidia-driver-590
sudo reboot
```

---

## 설치

```bash
# 1. 시스템 의존성
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git curl \
    libgtk-3-dev libnotify-dev libportaudio2 libcairo2-dev libx11-dev libxtst-dev \
    xclip libnotify-bin

# 2. 저장소 복제
git clone https://github.com/olivierpons/opons-voxd.git
cd opons-voxd

# 3. whisper.cpp 빌드 및 모델 다운로드
make setup

# 4. 바이너리 빌드: opons-voxd(데몬)와 opons-voxd-mcp(MCP 서버, 선택 사항)
make

# 5. 실행
./opons-voxd
```

---

## 로컬 AI를 위한 GPU 메모리 확보(MCP 서버)

Whisper 모델은 항상 GPU 메모리에 올라가 있습니다(`large-v3-turbo` + `medium` 합계 약 3.8 GB). 로컬 LLM, 이미지 모델, 학습 작업처럼 다른 일에 그 VRAM이 필요할 때, opons-voxd는 종료하지 않고도 모델을 언로드했다가 나중에 다시 로드할 수 있습니다. 언로드하면 프로세스의 CUDA 컨텍스트도 해제되므로 opons-voxd는 `nvidia-smi`에서 완전히 사라집니다(VRAM 0 MB). 다시 로드하는 데는 모델 파일이 아직 페이지 캐시에 있으면 1초도 걸리지 않고, 그렇지 않으면 몇 초가 걸립니다.

**트레이 아이콘 상태**:

| 아이콘 | 상태 |
|:---:|---|
| 흰색 고리 | 모델 로드됨, 준비 완료 |
| 빨간색 원 | 녹음 중 |
| 주황색 원 | 전사 중 |
| 사선이 그어진 회색 고리(⊘) | 모델 언로드됨, GPU 메모리 해제됨, 받아쓰기 꺼짐 |
| 파란색 4분의 3 호 | 모델 다시 로드 중 |

언로드된 동안에는 아이콘을 클릭하거나 푸시투토크 단축키를 눌러도 녹음이 시작되지 않고 오류 알림이 표시됩니다.

**직접 하기**: 트레이 아이콘을 오른쪽 클릭하고 *Unload models (free GPU memory)*(모델 언로드, GPU 메모리 해제)를 선택한 뒤, 다시 쓰려면 *Reload models*(모델 다시 로드)를 선택합니다.

**AI 에이전트에서 하기(MCP)**: 데몬을 `-m` / `--mcp` 옵션으로 실행합니다(기본값은 꺼짐). `make`는 `opons-voxd-mcp`도 함께 빌드합니다. stdio로 통신하는 [Model Context Protocol](https://modelcontextprotocol.io) 서버로, libc에만 링크되어 있습니다(RAM 약 1.5 MB). `unload_models`, `reload_models`, `get_status` 세 가지 도구를 제공합니다. Claude Code에서는 다음과 같이 등록합니다.

```bash
claude mcp add --scope user opons-voxd -- /path/to/opons-voxd/opons-voxd-mcp
```

다른 대부분의 클라이언트(Claude Desktop, Cursor, LM Studio 등)는 다음 JSON을 사용합니다.

```json
{ "mcpServers": { "opons-voxd": { "command": "/path/to/opons-voxd/opons-voxd-mcp" } } }
```

자세한 내용은 [메인 README](../README.md#free-the-gpu-for-local-ai-mcp-server)를 참고하세요.

---

## 푸시투토크 단축키

함께 제공되는 `launch.sh`는 `ctrl+alt+w`(AZERTY 자판에 권장) 또는 `ctrl+alt+z`(QWERTY 자판에 권장)를 사용합니다. 이 키는 `A` 바로 옆에 있어서 타이핑 자세를 흐트러뜨리지 않고 왼손만으로 조합 전체를 누를 수 있으며(새끼손가락은 `Ctrl`, 엄지는 `Alt`, 약지나 중지는 `W`/`Z`), 대부분의 데스크톱 환경에서 기본으로 할당되어 있지 않습니다. `OPONS_VOXD_PTT_HOTKEY`로 설정합니다. 자세한 내용은 [메인 README](../README.md#push-to-talk-hotkey)를 참고하세요.

---

## 라이선스

Whisper 모델(OpenAI), whisper.cpp(Georgi Gerganov), 그리고 이 프로젝트까지 모두 MIT 라이선스를 따릅니다.

전체 문서는 [메인 README](../README.md)를 참고하세요.

---

## 코딩 스타일

이 프로젝트는 **Linux 커널 코딩 스타일**(`Documentation/process/coding-style.rst`)에 Olivier Pons가 일부 조정을 더한 규칙을 따릅니다. K&R 중괄호, 4칸 들여쓰기, typedef 없는 `struct name`, 접두사 없음(`s_`, `t_`, `e_`), 괄호 없는 `return value;`, kernel-doc 주석, 한 줄 최대 80자입니다. 자세한 내용은 [메인 README](../README.md)를 참고하세요.
