# Mtiririko wangu wa kazi: ongea kwa lugha yako, andika kwa Kiingereza

🌍 **Lugha:**
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [Deutsch](WORKFLOW.de.md) · [中文](WORKFLOW.zh.md) · [日本語](WORKFLOW.ja.md) · [Español](WORKFLOW.es.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md) · [한국어](WORKFLOW.ko.md)

---

Lugha yangu ya kwanza ni Kifaransa, lakini sehemu kubwa ya kazi yangu ya programu — ujumbe wa commit, maoni ndani ya msimbo, issue za GitHub, mazungumzo na timu za kimataifa, nyaraka — hufanyika kwa Kiingereza. Kuandika Kiingereza fasaha si tatizo; kizuizi halisi ni *kufikiri* kwa Kiingereza huku sehemu nyingine ya ubongo wako ikifanya kazi kwa lugha yako ya kwanza. Uandishi wa sauti huondoa kizuizi hicho kabisa, mradi tu huhitaji kujitafsiri mwenyewe.

## Mpangilio

Ninatumia opons-voxd nikiwa na vitufe viwili vya mkato vya push-to-talk vinavyojitegemea:

- **`Ctrl+Alt+W` — uandishi wa sauti wa kawaida.** Ninakishikilia, ninaongea Kifaransa, ninakiachia, na maandishi ya Kifaransa yanaandikwa mahali pa kishale. Kwa madokezo, mazungumzo na wenzangu Wafaransa, na chochote kinachopaswa kubaki kwa Kifaransa.
- **`Ctrl+Alt+K` — uandishi wa sauti pamoja na tafsiri.** Kitendo kilekile, lakini ninaongea Kifaransa na maandishi ya *Kiingereza* ndiyo yanayotokea mahali pa kishale. Kwa chochote kinacholenga wasomaji wa Kiingereza: ujumbe wa commit, maelezo ya PR, maoni ndani ya msimbo, issue za GitHub, na chaneli za mazungumzo za Kiingereza.

Sinakili na kubandika kwenye zana ya tafsiri, sihami kutoka dirisha moja hadi jingine, siandiki upya. Ninaongea, na Kiingereza kinatokea palepale kishale changu kilipo.

## Kwa nini vitufe viwili badala ya kimoja

Tafsiri hufanya kazi vizuri tu kwa modeli iliyofunzwa hasa kwa kazi hiyo. Kwa kitufe cha kila siku cha Kifaransa natumia `large-v3-turbo` kwa sababu ni ya haraka — lakini tafsiri yake ni dhaifu, kwa kuwa imeboreshwa kwa kasi ya unukuzi, si kwa tafsiri. Kitufe cha tafsiri, kwa upande wake, hupakia modeli ya pili (`medium`) inayotafsiri ipasavyo. Modeli zote mbili hukaa kwenye kumbukumbu ya GPU kwa wakati mmoja; kwenye kadi yenye nafasi wazi ya zaidi ya GB chache, gharama hiyo ya ziada si jambo la kujali. Ninapohitaji kumbukumbu hiyo tena kwa kazi ya AI ya ndani, wakala anayeiendesha huziondoa modeli zote mbili kupitia seva ya MCP ya opons-voxd na kuzipakia upya baadaye; wakati huo aikoni ya trei hugeuka kuwa pete ya kijivu yenye mstari wa mshazari, hivyo najua kwamba uandishi wa sauti umezimwa (angalia [Kuachilia GPU kwa ajili ya AI ya ndani](README.sw.md#kuachilia-gpu-kwa-ajili-ya-ai-ya-ndani-seva-ya-mcp)).

## Jaribu

Huu ni mpangilio wa kawaida wa opons-voxd — hakuna fork wala patch inayohitajika. Angalia [Configuration](../README.md#configuration) na [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey) kwenye README kuu kuhusu `OPONS_VOXD_PTT_HOTKEY`, `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `OPONS_VOXD_TRANSLATE_MODEL` na `OPONS_VOXD_LANGUAGE`.

Ikiwa kila siku unazungumza lugha isiyo Kiingereza lakini kazi yako unaifanya kwa Kiingereza, huenda hiki ndicho kipengele chenye manufaa zaidi katika mradi huu.
