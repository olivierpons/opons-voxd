# İş Akışım: Kendi Dilinizde Konuşun, İngilizce Yazılsın

🌍 **Diller:**
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [Deutsch](WORKFLOW.de.md) · [中文](WORKFLOW.zh.md) · [日本語](WORKFLOW.ja.md) · [Español](WORKFLOW.es.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md) · [한국어](WORKFLOW.ko.md)

---

Ana dilim Fransızca, ama yazılım işlerimin çoğu — commit mesajları, kod yorumları, GitHub issue'ları, uluslararası ekiplerle yazışmalar, dokümantasyon — İngilizce yürüyor. Akıcı İngilizce yazmak işin zor kısmı değil; asıl darboğaz, beynin geri kalanı ana dilde çalışırken İngilizce *düşünmek*. Kendi sözlerinizi kendiniz çevirmek zorunda kalmıyorsanız dikte bu darboğazı tamamen ortadan kaldırır.

## Kurulum

opons-voxd'yi birbirinden bağımsız iki push-to-talk kısayoluyla kullanıyorum:

- **`Ctrl+Alt+W` — normal dikte.** Basılı tutuyorum, Fransızca konuşuyorum, bırakıyorum ve Fransızca metin imlece yazılıyor. Notlar, Fransızca konuşan meslektaşlarla yazışmalar ve Fransızca kalması gereken her şey için.
- **`Ctrl+Alt+K` — dikte ve çeviri.** Aynı hareket, ama Fransızca konuşuyorum ve imlece *İngilizce* metin yazılıyor. İngilizce okuyan bir kitleye yönelik her şey için: commit mesajları, PR açıklamaları, kod yorumları, GitHub issue'ları, İngilizce sohbet kanalları.

Çeviri aracına kopyala-yapıştır yok, bağlam değiştirme yok, yeniden yazma yok. Konuşuyorum ve İngilizce metin, imlecin zaten bulunduğu yerde beliriyor.

## Neden bir değil de iki kısayol

Çeviri ancak bu iş için gerçekten eğitilmiş bir modelle iyi sonuç verir. Günlük Fransızca kısayolunda hızlı olduğu için `large-v3-turbo` kullanıyorum — ama çevirisi zayıf, çünkü çeviri için değil, transkripsiyon hızı için optimize edilmiş. Çeviri kısayolu ise onun yerine düzgün çeviri yapan ikinci bir model (`medium`) yüklüyor. İki model de aynı anda GPU belleğinde duruyor; birkaç GB'tan fazla boş belleği olan bir kartta bu ek yük sorun olmaz. Bu belleği yerel bir yapay zekâ işi için geri almam gerektiğinde, o işi çalıştıran ajan iki modeli opons-voxd MCP sunucusu üzerinden bellekten kaldırıyor, iş bitince de yeniden yüklüyor; bu sırada tepsi simgesi gri, üstü çizili bir halkaya dönüşüyor ve diktenin devre dışı olduğunu anlıyorum (bkz. [Yerel yapay zekâ için GPU'yu boşaltma](README.tr.md#yerel-yapay-zekâ-için-gpuyu-boşaltma-mcp-sunucusu)).

## Deneyin

Bu, opons-voxd'nin standart yapılandırmasıdır — fork ya da yama gerekmez. `OPONS_VOXD_PTT_HOTKEY`, `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `OPONS_VOXD_TRANSLATE_MODEL` ve `OPONS_VOXD_LANGUAGE` için ana README'deki [Configuration](../README.md#configuration) ve [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey) bölümlerine bakın.

Günlük hayatta İngilizce dışında bir dil konuşup İngilizce çalışıyorsanız, bu muhtemelen projedeki en kullanışlı özelliktir.
