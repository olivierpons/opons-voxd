# Alur Kerja Saya: Berbicara dalam Bahasa Sendiri, Diketik dalam Bahasa Inggris

🌍 **Bahasa:**
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [Deutsch](WORKFLOW.de.md) · [中文](WORKFLOW.zh.md) · [日本語](WORKFLOW.ja.md) · [Español](WORKFLOW.es.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [Tiếng Việt](WORKFLOW.vi.md) · [한국어](WORKFLOW.ko.md)

---

Bahasa ibu saya bahasa Prancis, tetapi sebagian besar pekerjaan perangkat lunak saya — pesan commit, komentar kode, issue GitHub, obrolan dengan tim internasional, dokumentasi — berlangsung dalam bahasa Inggris. Mengetik bahasa Inggris dengan lancar bukan bagian yang sulit; hambatan sebenarnya adalah *berpikir* dalam bahasa Inggris sementara bagian otak lainnya bekerja dalam bahasa ibu. Dikte menghilangkan hambatan ini sepenuhnya jika Anda tidak perlu menerjemahkan ucapan sendiri.

## Pengaturan

Saya menjalankan opons-voxd dengan dua hotkey push-to-talk yang saling independen:

- **`Ctrl+Alt+W` — dikte biasa.** Saya menahannya, berbicara dalam bahasa Prancis, melepaskannya, lalu teks bahasa Prancis diketik di kursor. Untuk catatan, obrolan dengan rekan berbahasa Prancis, dan apa pun yang harus tetap dalam bahasa Prancis.
- **`Ctrl+Alt+K` — dikte dan terjemahkan.** Gerakannya sama, tetapi saya berbicara dalam bahasa Prancis dan yang muncul di kursor adalah *bahasa Inggris*. Untuk apa pun yang ditujukan kepada pembaca berbahasa Inggris: pesan commit, deskripsi PR, komentar kode, issue GitHub, kanal obrolan berbahasa Inggris.

Tanpa menyalin-tempel ke penerjemah, tanpa berpindah konteks, tanpa mengetik ulang. Saya berbicara, dan teks bahasa Inggris muncul tepat di posisi kursor saya.

## Mengapa dua hotkey, bukan satu

Terjemahan hanya bagus jika memakai model yang memang dilatih untuk tugas itu. Untuk hotkey bahasa Prancis sehari-hari, saya memakai `large-v3-turbo` karena cepat — tetapi terjemahannya buruk, karena model ini dioptimalkan untuk kecepatan transkripsi, bukan untuk terjemahan. Sebagai gantinya, hotkey terjemahan memuat model kedua (`medium`) yang menerjemahkan dengan benar. Kedua model berada di memori GPU secara bersamaan; pada kartu grafis yang masih memiliki ruang kosong lebih dari beberapa GB, biaya tambahan ini tidak menjadi masalah. Saat saya membutuhkan memori itu kembali untuk pekerjaan AI lokal, agen yang menjalankannya mengeluarkan kedua model dari memori melalui server MCP opons-voxd, lalu memuatnya kembali setelah selesai; selama itu, ikon tray berubah menjadi cincin abu-abu bergaris miring, sehingga saya tahu dikte sedang nonaktif (lihat [Membebaskan GPU untuk AI lokal](README.id.md#membebaskan-gpu-untuk-ai-lokal-server-mcp)).

## Coba sendiri

Ini konfigurasi standar opons-voxd — tidak perlu fork atau patch. Lihat [Configuration](../README.md#configuration) dan [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey) di README utama untuk `OPONS_VOXD_PTT_HOTKEY`, `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `OPONS_VOXD_TRANSLATE_MODEL`, dan `OPONS_VOXD_LANGUAGE`.

Jika sehari-hari Anda berbicara dalam bahasa selain bahasa Inggris tetapi bekerja dalam bahasa Inggris, ini mungkin fitur paling berguna di seluruh proyek ini.
