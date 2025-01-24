import cv2
import numpy as np
import serial
import time
from PIL import Image
import threading
import queue


# 逆推函数：将8x8字符字模数据转换为numpy图像
def char_data_to_image(char_data):
    char_width = 5
    char_height = 8
    img_array = np.ones((16, 20), dtype=np.uint8) * 255  # 初始为白色背景

    for char_idx in range(8):
        x_offset = (char_idx % 4) * char_width
        y_offset = (char_idx // 4) * char_height

        for row in range(char_height):
            row_data = char_data[char_idx][row]

            for col in range(char_width):
                if row_data & (1 << (4 - col)):
                    img_array[y_offset + row, x_offset + col] = 0  # 黑色像素

    return img_array


# 转换为8×8二维数组存储字符字模（十六进制）
def img_to_char_data(image):
    threshold = 128
    image = image.convert('L')
    img_array = np.array(image)
    img_array = np.where(img_array > threshold, 255, 0)

    char_data = [[0] * 8 for _ in range(8)]  # 8个字符，每个字符有8行数据
    char_width = 5
    char_height = 8

    for char_idx in range(8):
        x_offset = (char_idx % 4) * char_width
        y_offset = (char_idx // 4) * char_height

        for row in range(char_height):
            row_data = 0
            for col in range(char_width):
                pixel = img_array[y_offset + row, x_offset + col]
                if pixel == 0:
                    row_data |= (1 << (4 - col))
            char_data[char_idx][row] = row_data

    return char_data


# 串口通信初始化
def init_serial(port, baudrate):
    ser = serial.Serial(port, baudrate)
    return ser


# 发送一帧数据包到单片机
def send_frame_data(ser, frame_data):
    ser.write(bytes([START_BYTE]))  # 发送起始位
    for char_data in frame_data:
        for row in char_data:
            ser.write(bytes([row]))  # 将字节数据发送到串口
    ser.write(bytes([END_BYTE]))  # 发送结束位


# 获取源视频帧率，并根据帧率调整time.sleep
def get_fps_and_adjust_sleep(mp4_file):
    cap = cv2.VideoCapture(mp4_file)
    if not cap.isOpened():
        print("无法打开视频文件！")
        return None

    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))  # 总帧数
    fps = cap.get(cv2.CAP_PROP_FPS)  # 获取源视频帧率
    cap.release()
    
    # 根据帧率调整time.sleep
    if fps >= 60:
        return 1 / 148, total_frames
    elif fps >= 30:
        return 1 / 35, total_frames
    elif fps >= 24:
        return 1 / 26, total_frames
    else:
        # 如果帧率不是 30fps、60fps 或 24fps，则取默认速度
        return 1 / 25, total_frames


# 视频帧处理线程
def process_video_frame(mp4_file, frame_queue, sleep_time):
    cap = cv2.VideoCapture(mp4_file)
    if not cap.isOpened():
        print("无法打开视频文件！")
        return

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        img = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
        img = img.resize((20, 16))

        frame_data = img_to_char_data(img)

        # 将数据放入队列供串口发送和显示
        frame_queue.put(frame_data)

        # 控制帧处理速度
        time.sleep(sleep_time)

    cap.release()


# 预览视频和串口发送线程
def display_and_send(frame_queue, serial_port, baudrate, total_frames):
    # 初始化串口通信
    ser = init_serial(serial_port, baudrate)
    frame_count = 0  # 帧计数
    start_time = time.time()  # 初始时间
    current_frame_index = 0  # 当前帧索引

    while True:
        frame_data = frame_queue.get()  # 获取队列中的数据
        if frame_data is None:
            break  # 结束线程

        # 发送帧数据
        send_frame_data(ser, frame_data)

        # 预览图像
        img_array = char_data_to_image(frame_data)
        enlarged_image = cv2.resize(img_array, (img_array.shape[1] * 10, img_array.shape[0] * 10),
                                    interpolation=cv2.INTER_NEAREST)
        cv2.imshow("Frame", enlarged_image)
        cv2.waitKey(1)

        # 帧率计算
        frame_count += 1
        current_frame_index += 1  # 更新当前帧索引
        elapsed_time = time.time() - start_time
        if elapsed_time >= 1.0:
            frame_rate = frame_count / elapsed_time  # 帧率 = 帧数 / 时间
            progress = (current_frame_index / total_frames) * 100  # 播放进度
            print(f"当前帧率: {frame_rate: .2f} 帧/秒, 播放进度: {progress: .2f}%")
            frame_count = 0  # 重置帧计数
            start_time = time.time()  # 重置计时器

    ser.close()
    cv2.destroyAllWindows()


if __name__ == '__main__':

    # 参数
    mp4_file = "BadApple.mp4"  # MP4 文件路径
    serial_port = "COM6"  # 串口号
    baudrate = 57600  # 波特率

    START_BYTE = 0xFF  # 起始位
    END_BYTE = 0xFE  # 结束位

    # 获取源视频帧率并计算sleep时间
    sleep_time, total_frames = get_fps_and_adjust_sleep(mp4_file)
    print("每帧延时时间：", sleep_time, "s")
    if sleep_time is None:
        print("无法获取帧率，程序退出。")
        exit()

    # 创建队列
    frame_queue = queue.Queue()

    # 启动视频帧处理线程
    video_thread = threading.Thread(target=process_video_frame, args=(mp4_file, frame_queue, sleep_time))
    video_thread.start()

    # 启动显示和串口发送线程
    display_send_thread = threading.Thread(target=display_and_send, args=(frame_queue, serial_port,
                                                                          baudrate, total_frames))
    display_send_thread.start()

    # 等待线程结束
    video_thread.join()
    display_send_thread.join()
