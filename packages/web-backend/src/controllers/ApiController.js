// src/controllers/ApiController.js
import logRepository from '../repositories/LogRepository.js';
import deviceRepository from '../repositories/DeviceRepository.js';
import systemConfigRepository from '../repositories/SystemConfigRepository.js';
import mqttService from '../services/mqttService.js';

class ApiController {
    // 1. Lấy dữ liệu cảm biến mới nhất
    // Lấy temperature mới nhất và humidity mới nhất RIÊNG BIỆT từ sensor_logs
    // → Đảm bảo frontend luôn thấy giá trị mới nhất của từng sensor
    //   dù chúng không cùng được ghi trong 1 lần INSERT
    async getLatestSensors(req, res) {
        try {
            const data = await logRepository.getLatestSensorData();
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

    // 2. Điều khiển thiết bị
    async controlDevice(req, res) {
        try {
            const deviceId = req.params.id;
            const { action } = req.body; // "TURN_ON" hoặc "TURN_OFF"

            const device = await deviceRepository.getDeviceById(deviceId);
            if (!device) {
                return res.status(404).json({ error: 'Device not found' });
            }

            // Gửi lệnh MQTT
            const success = await mqttService.publishCommand(device.feed_key, action);
            if (!success) {
                return res.status(500).json({ error: 'Failed to send MQTT command' });
            }

            // door dùng OPEN/CLOSED, các thiết bị khác dùng ON/OFF
            let newStatus;
            if (device.feed_key === 'door') {
                newStatus = action === 'TURN_ON' ? 'OPEN' : 'CLOSED';
            } else {
                newStatus = action === 'TURN_ON' ? 'ON' : 'OFF';
            }

            await deviceRepository.updateDevice(deviceId, {
                name: device.name,
                type: device.type,
                status: newStatus,
            });

            res.status(200).json({ 
                deviceId: deviceId,
                deviceName: device.name,
                action: action,
                status: newStatus,
                success: true 
            });
        } catch (error) {
            console.error('ERROR - controlDevice:', error);
            res.status(400).json({ error: 'Invalid input' }); 
        }
    }

    // ✅ Trả về trạng thái on/off của tất cả device cho frontend khi reload
    async getDeviceStatus(req, res) {
        try {
            const statusMap = await deviceRepository.getDeviceStatusMap();
            res.status(200).json(statusMap);
        } catch (error) {
            console.error('ERROR - getDeviceStatus:', error);
            res.status(500).json({ error: 'Server error' });
        }
    }

    // 3. Cấu hình ngưỡng nhiệt độ
    async configThreshold(req, res) {
        try {
            const { temperature } = req.body; 
            await systemConfigRepository.setThreshold(temperature);
            res.status(200).json({ temperature: temperature, success: true }); 
        } catch (error) {
            res.status(500).json({ error: 'Server error' });
        }
    }

    // 4. Truy vấn nhật ký (Logs)
    async getLogs(req, res) {
        try {
            const { from, to } = req.query;
            const logs = await logRepository.getLogs(from, to);
            res.status(200).json(logs);
        } catch (error) {
            res.status(500).json({ error: 'Server error' });
        }
    }

    // 5. Đăng nhập
    async login(req, res) {
        const { user, pass } = req.body; 
        if (user === 'admin' && pass === 'password') {
            const token = "fake-jwt-token";
            res.status(200).json({ token: token, redirect: '/dashboard' }); 
        } else {
            res.status(401).json({ error: 'Unauthorized' }); 
        }
    }
}

export default new ApiController();