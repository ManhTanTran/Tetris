# Tetris

## Giới thiệu game

Tetris là một thể loại game xếp hình 

- **1. Xếp hình**: Mỗi khối rơi từ trên xuống và người chơi phải xoay và di chuyển chúng sao cho xếp thành các hàng đầy đủ.
- **2. Điểm số**: Người chơi sẽ nhận được điểm khi hoàn thành một hàng. Càng hoàn thành nhiều hàng liên tiếp, điểm số càng cao.
- **3. Tăng độ khó theo level**: Khi người chơi chơi lâu, tốc độ rơi của các khối tăng dần, khiến trò chơi trở nên khó hơn.

- [I. Bắt đầu game](#1-bắt-đầu-game) 
## I. Bắt đầu game

Chờ một chút xong đó mọi người ấn nút Enter trên bàn phím để bắt đầu chơi game

![image](https://github.com/user-attachments/assets/9bca7fae-c9ea-4e94-af8c-83b2ea6c21c9)

Sau đó người chơi sẽ đến giao diện game như thế này

![image](https://github.com/user-attachments/assets/2cf849c8-6e74-4ade-9216-6b04975a7fc5)

Để có thể bắt đầu game, người chơi vui lòng ấn nút Enter trên bàn phím một lần nữa để bắt đầu trò chơi

## II. Hướng dẫn

Mỗi khi người chơi xếp được thành 1 hàng hoàn chỉnh thì hàng đó sẽ mất đi và cho người chơi 100 điểm, khi đạt đủ 1000 điểm người chơi sẽ được lên level mới
và mỗi level tăng thì các khối block sẽ rơi xuống càng nhanh hơn

![image](https://github.com/user-attachments/assets/b63bb229-f106-42f7-b5b3-e818868024b4)

Lưu ý: đặc biệt trong game, người chơi có thể ấn C để lưu khối đó lại và thay bằng khối khác
rồi ấn lại C nếu người chơi muốn dùng lại khối đó.

![image](https://github.com/user-attachments/assets/194a2d79-89de-4ebd-93d7-c2c2d52baf4a)

Nếu người chơi muốn dừng game để có thể đi ra ngoài hoặc nghỉ ngơi, hãy ấn nút P để dừng trò chơi
và muốn tiếp tục thì lại ấn nút P

![image](https://github.com/user-attachments/assets/b50eae02-9e34-42dd-ab49-1ffb92e9c4bc)

Khi các block đã rơi xuống đáy rồi người chơi có một khoảng thời gian nhất định để có thể di chuyển các khối chứ không
bị đổi sang khối khác luôn.

Người chơi có thể ấn nút R để chơi lại từ đầu.

## III. Các thành phần trong game
Chúng ta có 7 khối như thế này

![image](https://github.com/user-attachments/assets/3b53ed3e-8d08-4889-9747-4f588e60e722)

Score: là bảng tính điểm của người chơi

![image](https://github.com/user-attachments/assets/e2439edf-e301-40dc-bfa1-f46edca00282)

Level: là level hiện tại của người chơi xem họ đã đạt đến được level nào

![image](https://github.com/user-attachments/assets/6276dca8-2b5b-424c-a07f-717f0cf1c806)

Next Piece: khối tiếp theo mà người chơi sử dụng

![image](https://github.com/user-attachments/assets/5c7520d3-6eec-4e4d-8d70-3a421fb64225)

Hold Piece: khối mà người chơi đã hold

![image](https://github.com/user-attachments/assets/a9df5c57-802d-43d8-a997-b465eb926a47)


## IV. Khi thất bại

![image](https://github.com/user-attachments/assets/71ce0859-5f5a-49ba-8d0f-19396fb720ef)

# Về source code game:

- **Folder draw**:
- khởi tạo window và renderer
- chứa tất cả các ảnh trong game (được đánh số)
- chứa hầu hết các câu lệnh của SDL2, SDL2_image, SDL2_ttf, SDL2_text, SDL2_mixer sử dụng để vẽ load ảnh, chữ và vẽ lên renderer, window
- sử dụng chữ để tạo ra các từ thua cuộc hay pause game, và dùng nhạc cho game hay hơn.
- **Folder elements**: Bao gồm các nút di chuyển lên xuống trái phải
- **Folder events**: Xử lý các sự kiện trong game
- game: tất cả các sự kiện trong một màn chơi
- lose: xử lý khi thua
- restart_menu: xử lý khi người chơi muốn chơi lại màn chơi
- pause_menu: xử lý khi người chơi dừng game


