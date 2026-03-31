// src/controllers/ApiController.js
import logRepository from '../repositories/LogRepository.js';
import deviceRepository from '../repositories/DeviceRepository.js';
import systemConfigRepository from '../repositories/SystemConfigRepository.js';
import mqttService from '../services/mqttService.js';

class ApiController {
    // 1. Lấy dữ liệu cảm biến mới nhất [cite: 74, 76]
    async getLatestSensors(req, res) {
        try {
            const data = await logRepository.getLatestSensorData();
            // Trả về format yêu cầu [cite: 79, 80, 81, 82]
            res.status(200).json({
                temperature: data ? data.temperature : 30.5,
                humidity: data ? data.humidity : 70,
                timestamp: data ? data.timestamp : new Date().toISOString()
            });
        } catch (error) {
            console.error("DEBUG - getLatestSensors Error:", error);
            res.status(500).json({ error: 'Server error' }); 
        }
    }

    // 2. Điều khiển thiết bị [cite: 84, 86]
    async controlDevice(req, res) {
        try {
            const deviceId = req.params.id;
            const { action } = req.body; // VD: "TURN_ON" [cite: 90]

            // Sử dụng Observer Pattern để gửi lệnh qua MQTT [cite: 148]
            mqttService.publishCommand(deviceId, action);

            res.status(200).json({ action: action, success: true });
        } catch (error) {
            res.status(400).json({ error: 'Invalid input' }); 
        }
    }

    // 3. Cấu hình ngưỡng nhiệt độ [cite: 92, 94]
    async configThreshold(req, res) {
        try {
            const { temperature } = req.body; 

            await systemConfigRepository.setThreshold(temperature);

            res.status(200).json({ temperature: temperature, success: true }); 
        } catch (error) {
            res.status(500).json({ error: 'Server error' });
        }
    }

    // 4. Truy vấn nhật ký (Logs) [cite: 100, 102]
    async getLogs(req, res) {
        try {
            const { from, to } = req.query;
            const logs = await logRepository.getLogs(from, to);

            res.status(200).json(logs); // Trả về mảng logs [cite: 105]
        } catch (error) {
            res.status(500).json({ error: 'Server error' });
        }
    }

    // 5. API Đăng nhập (Mở rộng từ Sequence Diagram) [cite: 62, 65]
    async login(req, res) {
        const { user, pass } = req.body; 
        // TODO: Validate trong DB
        if (user === 'admin' && pass === 'password') { // Giả lập hợp lệ 
            const token = "fake-jwt-token";
            res.status(200).json({ token: token, redirect: '/dashboard' }); 
        } else {
            res.status(401).json({ error: 'Unauthorized' }); 
        }
    }
}

export default new ApiController();