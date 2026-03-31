import express from 'express';
import apiController from '../controllers/ApiController.js';

const router = express.Router();

// Các Endpoint 
router.post('/login', apiController.login);
router.get('/api/sensors/latest', apiController.getLatestSensors); 
router.post('/api/devices/:id/control', apiController.controlDevice); 
router.post('/api/config/threshold', apiController.configThreshold); 
router.get('/api/logs', apiController.getLogs); 

export default router;