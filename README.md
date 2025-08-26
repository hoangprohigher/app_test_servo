# Hướng dẫn sử dụng nhanh

## Kết nối
- Chọn cổng COM trong mục **Kết nối** → bấm **Kết nối**.
- (Tùy chọn) bấm **Làm mới** nếu chưa thấy cổng.

## Cấu hình servo
- Chọn **Số servo** (1..16).
- Chọn **kênh PCA9685** (0..15) cho từng servo (không trùng).
- Bấm **Gửi CFG + HOME** để áp dụng và đưa tất cả về 0°.

## Nhập mảng góc
- Ô **Mảng góc**: nhập đủ N giá trị (0..180), cách nhau dấu cách hoặc dấu phẩy.
- Ví dụ (5 servo): `30 60 90 120 150`.
- (Tùy chọn) điền **Tên mảng góc** và bấm **Lưu mảng** (đặt tên).

## Chạy
- **Chạy tuần tự (4s/servo)**: app sẽ đặt từng servo theo thứ tự, mỗi servo cách 4 giây. Kết thúc sẽ hỏi có lưu mảng không.
- **Đặt tất cả ngay**: đặt đồng thời toàn bộ góc. Có thể chọn lưu mảng sau khi đặt.

## Quản lý mảng đã lưu
- Khung **Các mảng góc đã lưu**:
  - **Chạy tuần tự (từ danh sách)**: chạy mảng được chọn.
  - **Đặt tất cả ngay (từ danh sách)**: đặt đồng thời theo mảng được chọn.
  - **Tải vào ô góc**: nạp mảng đã chọn lên ô nhập để chỉnh/sửa và lưu lại.
  - **Xoá mảng đã chọn**: xóa khỏi danh sách.

## Tiện ích
- **HOME (về 0°)**: đưa tất cả servo về 0° bất cứ lúc nào.
- **In mapping**: in lại danh sách Servo → Kênh hiện tại.
- **Công cụ → Kiểm tra & cài thư viện**: kiểm tra/cài pyserial.

## Ghi chú sử dụng
- Số lượng góc nhập phải đúng bằng số servo.
- Góc hợp lệ: 0..180.
- Nếu không thấy phản hồi, kiểm tra đúng cổng COM và đã nạp firmware.

## Chạy bản .exe (nếu có)
- Mở `ServoPCA9685.exe` → thao tác y hệt các bước ở trên.
