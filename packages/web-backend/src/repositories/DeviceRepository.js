// src/repositories/DeviceRepository.js
import db from '../config/db.js';

class DeviceRepository {
    async getAllDevices() {
        const [rows] = await db.query('SELECT * FROM devices');
        return rows;
    }

    async getDeviceById(id) {
        const [rows] = await db.query('SELECT * FROM devices WHERE id = ?', [id]);
        return rows[0];
    }

    async addDevice(device) {
        const { name, type, status } = device;
        const [result] = await db.query('INSERT INTO devices (name, type, status) VALUES (?, ?, ?)', [name, type, status]);
        return result.insertId;
    }

    async updateDevice(id, device) {
        const { name, type, status } = device;
        await db.query('UPDATE devices SET name = ?, type = ?, status = ? WHERE id = ?', [name, type, status, id]);
    }

    async deleteDevice(id) {
        await db.query('DELETE FROM devices WHERE id = ?', [id]);
    }

    // Kiểm tra status có nghĩa là "đang bật/mở" không
    // Bao gồm tất cả giá trị có thể có từ MQTT: 'ON', 'TURN_ON', 'OPEN', '1', 'true'
    _isActive(status) {
        if (!status) return false;
        const s = status.toString().trim().toUpperCase();
        return ['ON', 'TURN_ON', 'OPEN', '1', 'TRUE'].includes(s);
    }

    // Lấy trạng thái 4 device theo feed_key thay vì id
    // Dùng feed_key vì MQTT update theo feed_key → đảm bảo luôn đúng
    async getDeviceStatusMap() {
        const [rows] = await db.query(
            `SELECT feed_key, status FROM devices 
             WHERE feed_key IN ('led-state', 'fan-state', 'door', 'tv-state')`
        );

        // Map feed_key → trạng thái boolean
        const map = {};
        rows.forEach(row => {
            map[row.feed_key] = this._isActive(row.status);
        });

        return {
            livingRoomLight: map['led-state']  ?? false,
            bedroomFan:      map['fan-state']  ?? false,
            garageDoor:      map['door']       ?? false,
            livingRoomTV:    map['tv-state']   ?? false,
        };
    }
}

export default new DeviceRepository();