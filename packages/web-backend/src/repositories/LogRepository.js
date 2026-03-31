// src/repositories/LogRepository.js
import db from '../config/db.js';

class LogRepository {
    async getLatestSensorData() {
        const [rows] = await db.query('SELECT * FROM sensor_logs ORDER BY timestamp DESC LIMIT 1');
        return rows[0];
    }

    async getLogs(fromDate, toDate) {
        const [rows] = await db.query(
            'SELECT * FROM action_logs WHERE time >= ? AND time <= ? ORDER BY time DESC',
            [fromDate, toDate]
        );
        return rows;
    }
}

export default new LogRepository();
