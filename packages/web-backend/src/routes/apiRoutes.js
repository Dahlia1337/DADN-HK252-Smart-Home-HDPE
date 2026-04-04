import express from 'express';
import apiController from '../controllers/ApiController.js';

const router = express.Router();

// Auth
router.post('/login', apiController.login);

// Sensors
router.get('/api/sensors/latest', apiController.getLatestSensors);

// Devices
// ✅ Route /status phải đặt TRƯỚC /:id để Express không hiểu nhầm "status" là id
router.get('/api/devices/status', apiController.getDeviceStatus);
router.post('/api/devices/:id/control', apiController.controlDevice);

// Config & Logs
router.post('/api/config/threshold', apiController.configThreshold);
router.get('/api/logs', apiController.getLogs);

export default router;