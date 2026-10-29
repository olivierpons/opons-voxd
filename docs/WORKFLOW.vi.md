# Quy trình làm việc của tôi: nói tiếng mẹ đẻ, ra chữ tiếng Anh

🌍 **Bản dịch:**
[English](../WORKFLOW.md) · [Français](WORKFLOW.fr.md) · [Deutsch](WORKFLOW.de.md) · [中文](WORKFLOW.zh.md) · [日本語](WORKFLOW.ja.md) · [Español](WORKFLOW.es.md) · [Italiano](WORKFLOW.it.md) · [हिन्दी](WORKFLOW.hi.md) · [العربية](WORKFLOW.ar.md) · [বাংলা](WORKFLOW.bn.md) · [Português](WORKFLOW.pt.md) · [Русский](WORKFLOW.ru.md) · [اردو](WORKFLOW.ur.md) · [Bahasa Indonesia](WORKFLOW.id.md) · [Kiswahili](WORKFLOW.sw.md) · [मराठी](WORKFLOW.mr.md) · [తెలుగు](WORKFLOW.te.md) · [Türkçe](WORKFLOW.tr.md) · [தமிழ்](WORKFLOW.ta.md) · [한국어](WORKFLOW.ko.md)

---

Tiếng mẹ đẻ của tôi là tiếng Pháp, nhưng phần lớn công việc phần mềm của tôi lại diễn ra bằng tiếng Anh: thông điệp commit, chú thích trong mã, issue trên GitHub, trò chuyện với các nhóm quốc tế, tài liệu. Gõ tiếng Anh trôi chảy không phải là chuyện khó; điểm nghẽn thật sự là phải *suy nghĩ* bằng tiếng Anh trong khi phần còn lại của bộ não vẫn đang chạy bằng tiếng mẹ đẻ. Nếu không phải tự dịch, đọc chính tả sẽ xóa bỏ hoàn toàn điểm nghẽn đó.

## Thiết lập

Tôi dùng opons-voxd với hai phím tắt push-to-talk hoạt động độc lập:

- **`Ctrl+Alt+W` — đọc chính tả thông thường.** Giữ phím, nói tiếng Pháp, thả ra, văn bản tiếng Pháp được gõ ngay tại con trỏ. Dùng cho ghi chú, trò chuyện với đồng nghiệp nói tiếng Pháp, và mọi thứ cần giữ nguyên tiếng Pháp.
- **`Ctrl+Alt+K` — đọc chính tả kèm dịch.** Vẫn thao tác đó, nhưng tôi nói tiếng Pháp mà chữ hiện ra tại con trỏ lại là *tiếng Anh*. Dùng cho mọi thứ viết cho người đọc tiếng Anh: thông điệp commit, mô tả PR, chú thích trong mã, issue trên GitHub, kênh trò chuyện bằng tiếng Anh.

Không phải chép sang công cụ dịch rồi dán lại, không phải chuyển ngữ cảnh, không phải gõ lại. Tôi nói, và tiếng Anh hiện ra đúng chỗ con trỏ đang đứng.

## Vì sao hai phím tắt chứ không phải một

Chỉ mô hình thật sự được huấn luyện cho việc dịch mới dịch tốt. Với phím tắt tiếng Pháp dùng hằng ngày, tôi chọn `large-v3-turbo` vì nó nhanh — nhưng mô hình này được tối ưu cho tốc độ chuyển giọng nói thành văn bản chứ không phải cho việc dịch, nên dịch khá kém. Vì vậy, phím tắt dịch nạp một mô hình thứ hai (`medium`), mô hình này dịch tốt. Cả hai mô hình cùng nằm trong bộ nhớ GPU một lúc; với card đồ họa còn trống hơn vài GB, phần tốn thêm này không đáng bận tâm. Khi cần lấy lại lượng bộ nhớ đó cho một tác vụ AI cục bộ, tác tử chạy tác vụ ấy sẽ gỡ cả hai mô hình qua máy chủ MCP của opons-voxd rồi nạp lại khi xong việc; trong lúc đó, biểu tượng khay chuyển thành vòng tròn xám có gạch chéo, nhờ vậy tôi biết đọc chính tả đang tắt (xem [Giải phóng GPU cho AI cục bộ](README.vi.md#giải-phóng-gpu-cho-ai-cục-bộ-máy-chủ-mcp)).

## Hãy thử

Đây là cấu hình chuẩn của opons-voxd — không cần fork hay patch gì cả. Xem [Configuration](../README.md#configuration) và [Push-to-talk-and-translate hotkey](../README.md#push-to-talk-and-translate-hotkey) trong README chính để biết về `OPONS_VOXD_PTT_HOTKEY`, `OPONS_VOXD_PTT_TRANSLATE_HOTKEY`, `OPONS_VOXD_TRANSLATE_MODEL` và `OPONS_VOXD_LANGUAGE`.

Nếu hằng ngày bạn nói một ngôn ngữ khác tiếng Anh nhưng lại làm việc bằng tiếng Anh, thì đây có lẽ là tính năng hữu ích nhất của dự án này.
