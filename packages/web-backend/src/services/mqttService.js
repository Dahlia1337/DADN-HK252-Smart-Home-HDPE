import mqtt from 'mqtt';
import dotenv from 'dotenv';
import path from 'path';
import pool from '../config/db.js';

dotenv.config({
    path: path.resolve(process.cwd(), 'packages/web-backend/.env')
});

class MqttService {
    constructor() {
        const username = process.env.MQTT_USERNAME;
        const password = process.env.MQTT_KEY;
        const brokerUrl = process.env.MQTT_BROKER_URL;

        this.username = username;
        this.client = mqtt.connect(brokerUrl, {
            username,
            password,
            reconnectPeriod: 3000,
            connectTimeout: 30000
        });

        // Cache giữ giá trị cuối cùng nhận được — KHÔNG bao giờ reset về null
        // Mục đích: khi chỉ nhận 1 trong 2, vẫn có đủ dữ liệu để INSERT vào DB
        this.sensorCache = {
            temperature: null,
            humidity: null
        };

        this.client.on('connect', () => {
            console.log('✅ Connected to Adafruit IO');
            this.subscribeToAllFeeds();
        });

        this.client.on('reconnect', () => console.log('🔄 Reconnecting...'));
        this.client.on('error', (err) => console.error('❌ MQTT Error:', err.message));
    }

    async publishCommand(feedKey, action) {
        if (!this.client?.connected) return false;
        let value;
        if (action === "ON" || action === "TURN_ON") value = "1";
        else if (action === "OFF" || action === "TURN_OFF") value = "0";
        else value = action.toString();

        const topic = `${this.username}/feeds/${feedKey}`;
        this.client.publish(topic, value, { qos: 1 });

        console.log(`📤 Published to ${topic}: ${value}`);

        await pool.execute(
            'INSERT INTO action_logs (device, action) VALUES (?, ?)',
            [feedKey, value]
        );

        return true;
    }


    subscribeToAllFeeds() {
        const feedKeys = [
            'humidity', 'temperature', 'door',
            'fan-speed', 'fan-state', 'led-state',
            'tv-state', 'rgb-state', 'system-state'
        ];

        const topics = feedKeys.map(key => `${this.username}/feeds/${key}`);

        this.client.subscribe(topics, (err, granted) => {
            if (err) {
                console.error('❌ Subscribe error:', err);
            } else {
                console.log(`Đã subscribe thành công ${granted.length} feeds`);
            }
        });

        this.client.on('message', async (receivedTopic, message) => {
            const dataString = message.toString();
            const feedKey = receivedTopic.split('/').pop();
            console.log(`Nhận dữ liệu từ [${feedKey}]:`, dataString);

            try {
                // 1. Xử lý cảm biến Nhiệt độ / Độ ẩm
                if (feedKey === 'temperature' || feedKey === 'humidity') {
                    const dataValue = parseFloat(dataString);

                    // Cập nhật cache với giá trị mới nhận được
                    this.sensorCache[feedKey] = dataValue;

                    // Chỉ cần có ít nhất 1 giá trị hợp lệ là INSERT ngay
                    // Giá trị còn lại lấy từ cache (lần nhận trước đó)
                    // → Frontend luôn thấy dữ liệu mới nhất sau mỗi lần nhận bất kỳ sensor nào
                    if (this.sensorCache.temperature !== null || this.sensorCache.humidity !== null) {
                        const [result] = await pool.execute(
                            'INSERT INTO sensor_logs (temperature, humidity) VALUES (?, ?)',
                            [this.sensorCache.temperature, this.sensorCache.humidity]
                        );
                        console.log(`💾 Đã lưu thông số cảm biến (ID: ${result.insertId}) — temp: ${this.sensorCache.temperature}, hum: ${this.sensorCache.humidity}`);
                        // Không reset cache → giá trị cũ vẫn dùng được cho lần nhận tiếp theo
                    }
                }

                // 2. Xử lý trạng thái Bật/Tắt, Đóng/Mở
                else if (['led-state', 'fan-state', 'door', 'tv-state'].includes(feedKey)) {
                    await pool.execute(
                        'UPDATE devices SET status = ? WHERE feed_key = ?',
                        [dataString, feedKey]
                    );

                    await pool.execute(
                        'INSERT INTO action_logs (device, action) VALUES (?, ?)',
                        [feedKey, dataString]
                    );
                    console.log(`💡 Đã cập nhật trạng thái (status) của ${feedKey} thành ${dataString}`);
                }

                // 3. Xử lý thông số mở rộng (Tốc độ quạt, Màu đèn)
                else if (['fan-speed', 'rgb-state'].includes(feedKey)) {
                    const targetFeedKey = feedKey === 'fan-speed' ? 'fan-state' : 'led-state';

                    await pool.execute(
                        'UPDATE devices SET current_value = ? WHERE feed_key = ?',
                        [dataString, targetFeedKey]
                    );

                    await pool.execute(
                        'INSERT INTO action_logs (device, action) VALUES (?, ?)',
                        [feedKey, dataString]
                    );
                    console.log(`⚙️ Đã cập nhật thông số (current_value) của ${targetFeedKey} thành ${dataString}`);
                }

                // 4. Ghi log các hệ thống khác (system-state...)
                else {
                    await pool.execute(
                        'INSERT INTO action_logs (device, action) VALUES (?, ?)',
                        [feedKey, dataString]
                    );
                    console.log(`📝 Đã ghi log hệ thống cho ${feedKey}: ${dataString}`);
                }

            } catch (error) {
                console.error(`❌ Lỗi xử lý DB cho feed [${feedKey}]:`, error.message);
            }
        });
    }
}

export default new MqttService();