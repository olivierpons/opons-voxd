# 내 워크플로: 모국어로 말하고 영어로 입력하기

🌍 **번역**:
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [Deutsch](WORKFLOW.de.md) · [中文](WORKFLOW.zh.md) · [日本語](WORKFLOW.ja.md) · [Español](WORKFLOW.es.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md)

---

저는 프랑스어가 모국어지만, 소프트웨어 작업은 대부분 영어로 합니다. 커밋 메시지, 코드 주석, GitHub 이슈, 해외 팀과의 채팅, 문서가 모두 영어입니다. 영어를 유창하게 타이핑하는 것 자체는 어렵지 않습니다. 진짜 병목은 머리의 나머지 부분이 모국어로 돌아가는 동안 영어로 *생각*해야 한다는 점입니다. 직접 번역할 필요만 없다면 받아쓰기가 이 병목을 완전히 없애 줍니다.

## 설정

저는 opons-voxd에 서로 독립된 푸시투토크 단축키 두 개를 설정해 씁니다.

- **`Ctrl+Alt+W`: 일반 받아쓰기.** 누르고 있는 동안 프랑스어로 말하고 손을 떼면 커서 위치에 프랑스어 텍스트가 입력됩니다. 메모, 프랑스어권 동료와의 채팅처럼 프랑스어 그대로 두어야 하는 모든 글에 씁니다.
- **`Ctrl+Alt+K`: 받아쓰기와 번역.** 동작은 같지만, 프랑스어로 말하면 커서 위치에 *영어*가 입력됩니다. 커밋 메시지, PR 설명, 코드 주석, GitHub 이슈, 영어 채팅 채널처럼 영어로 읽힐 모든 글에 씁니다.

번역기에 복사해 붙여 넣을 일도, 컨텍스트를 전환할 일도, 다시 타이핑할 일도 없습니다. 말만 하면 커서가 있는 바로 그 자리에 영어가 나타납니다.

## 단축키가 하나가 아니라 둘인 이유

번역은 실제로 그 작업을 학습한 모델이어야 잘됩니다. 평소 쓰는 프랑스어 단축키에는 빠른 `large-v3-turbo`를 쓰지만, 이 모델은 번역이 아니라 전사 속도에 맞춰 최적화되어 있어서 번역 품질이 떨어집니다. 그래서 번역 단축키는 제대로 번역하는 두 번째 모델(`medium`)을 로드합니다. 두 모델은 동시에 GPU 메모리에 올라가 있지만, 여유 메모리가 몇 GB 이상인 GPU라면 이 추가 부담은 문제가 되지 않습니다. 로컬 AI 작업에 그 메모리가 필요할 때는 그 작업을 실행하는 에이전트가 opons-voxd MCP 서버를 통해 두 모델을 언로드하고, 작업이 끝나면 다시 로드합니다. 그동안 트레이 아이콘이 사선이 그어진 회색 고리로 바뀌므로 받아쓰기가 꺼져 있다는 것을 알 수 있습니다([로컬 AI를 위한 GPU 메모리 확보](README.ko.md#로컬-ai를-위한-gpu-메모리-확보mcp-서버) 참고).

## 사용해 보기

이것은 opons-voxd의 표준 설정이므로 포크도 패치도 필요 없습니다. `OPONS_VOXD_PTT_HOTKEY`, `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `OPONS_VOXD_TRANSLATE_MODEL`, `OPONS_VOXD_LANGUAGE`에 대해서는 메인 README의 [Configuration](../README.md#configuration)과 [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey)를 참고하세요.

평소에는 영어가 아닌 언어로 말하지만 일은 영어로 한다면, 아마 이 기능이 이 프로젝트에서 가장 쓸모 있을 것입니다.
