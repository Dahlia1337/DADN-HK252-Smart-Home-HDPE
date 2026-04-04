// src/repositories/LogRepository.js
import db from '../config/db.js';

class LogRepository {
    async getLatestSensorData() {
        const [rows] = await db.query(`
            SELECT
                (
                    SELECT temperature 
                    FROM sensor_logs 
                    WHERE temperature IS NOT NULL 
                    ORDER BY timestamp DESC 
                    LIMIT 1
                ) AS temperature,
                (
                    SELECT humidity 
                    FROM sensor_logs 
                    WHERE humidity IS NOT NULL 
                    ORDER BY timestamp DESC 
                    LIMIT 1
                ) AS humidity,
                (
                    SELECT timestamp 
                    FROM sensor_logs 
                    ORDER BY timestamp DESC 
                    LIMIT 1
                ) AS timestamp
        `);

        const result = rows[0];

        // Nếu cả 2 đều null → chưa có dữ liệu nào trong DB
        if (result.temperature === null && result.humidity === null) {
            return null;
        }

        return result;
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