import { Thermometer, Droplet, Zap, Shield, Lightbulb, DoorClosed, Fan, Tv } from "lucide-react";
import { useState, useEffect } from "react";
import axios from "axios";

export function Dashboard() {
  const [sensorData, setSensorData] = useState({ temperature: 24, humidity: 65, timestamp: "" });
  const [devices, setDevices] = useState({
    livingRoomLight: false,
    garageDoor: false,
    bedroomFan: false,
    livingRoomTV: false,
  });
  const [loading, setLoading] = useState<string | null>(null);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    const fetchSensorData = async () => {
      try {
        const response = await axios.get('http://localhost:3000/api/sensors/latest');
        setSensorData(response.data);
        setError(null);
      } catch (error) {
        console.error('Error fetching sensor data:', error);
        setError('Không thể lấy dữ liệu cảm biến');
      }
    };

    fetchSensorData();
    const interval = setInterval(fetchSensorData, 5000); // Update every 5s
    return () => clearInterval(interval);
  }, []);

  const toggleDevice = async (deviceKey: string, deviceId: number) => {
    const newStatus = !devices[deviceKey as keyof typeof devices];
    const action = newStatus ? 'TURN_ON' : 'TURN_OFF';

    setLoading(deviceKey);
    try {
      const response = await axios.post(`http://localhost:3000/api/devices/${deviceId}/control`, { action });
      console.log('Device control response:', response.data);
      setDevices(prev => ({ ...prev, [deviceKey]: newStatus }));
      setError(null);
    } catch (error) {
      console.error('Error controlling device:', error);
      setError(`Lỗi điều khiển ${deviceKey}`);
      // Revert the state if request fails
      setDevices(prev => ({ ...prev, [deviceKey]: !newStatus }));
    } finally {
      setLoading(null);
    }
  };

  return (
    <div className="space-y-6">
      <h2 className="text-3xl font-bold text-gray-900">Dashboard</h2>

      {/* Error Banner */}
      {error && (
        <div className="bg-red-50 border border-red-200 rounded-lg p-4">
          <p className="text-sm text-red-700">{error}</p>
        </div>
      )}

      {/* Quick Stats */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        {/* Temperature Card */}
        <div className="bg-white rounded-xl p-6 shadow-sm border border-gray-200">
          <div className="flex items-center justify-between mb-4">
            <Thermometer className="text-orange-500" size={32} />
          </div>
          <div className="text-3xl font-bold text-gray-900">{sensorData.temperature}°C</div>
          <div className="text-sm text-gray-500 mt-1">Temperature</div>
          <div className="mt-4 h-12 flex items-end gap-1">
            {[20, 35, 50, 45, 60, 55, 70, 65, 75, 80].map((height, i) => (
              <div
                key={i}
                className="flex-1 bg-orange-200 rounded-t"
                style={{ height: `${height}%` }}
              />
            ))}
          </div>
        </div>

        {/* Humidity Card */}
        <div className="bg-white rounded-xl p-6 shadow-sm border border-gray-200">
          <div className="flex items-center justify-between mb-4">
            <Droplet className="text-blue-500" size={32} />
          </div>
          <div className="text-3xl font-bold text-gray-900">{sensorData.humidity}%</div>
          <div className="text-sm text-gray-500 mt-1">Humidity</div>
        </div>

        {/* Active Devices Card */}
        <div className="bg-white rounded-xl p-6 shadow-sm border border-gray-200">
          <div className="flex items-center justify-between mb-4">
            <Zap className="text-yellow-500" size={32} />
          </div>
          <div className="text-3xl font-bold text-gray-900">12 Online</div>
          <div className="text-sm text-gray-500 mt-1">2 Offline</div>
        </div>

        {/* Security Status Card */}
        <div className="bg-white rounded-xl p-6 shadow-sm border border-gray-200">
          <div className="flex items-center justify-between mb-4">
            <Shield className="text-green-500" size={32} />
          </div>
          <div className="text-3xl font-bold text-gray-900">Armed</div>
          <div className="text-sm text-gray-500 mt-1">System Status</div>
        </div>
      </div>

      {/* Quick Controls */}
      <div>
        <h3 className="text-xl font-semibold text-gray-900 mb-4">Quick Controls</h3>
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
          {/* Living Room Light */}
          <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-200">
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-3">
                <div className={`p-2 rounded-lg ${devices.livingRoomLight ? 'bg-yellow-100' : 'bg-gray-100'}`}>
                  <Lightbulb className={devices.livingRoomLight ? 'text-yellow-500' : 'text-gray-400'} size={24} />
                </div>
                <div>
                  <div className="font-medium text-gray-900">Living Room Light</div>
                  <div className="text-sm text-gray-500">{devices.livingRoomLight ? 'On' : 'Off'}</div>
                </div>
              </div>
              <button
                onClick={() => toggleDevice('livingRoomLight', 1)}
                className={`relative inline-flex h-6 w-11 items-center rounded-full transition-colors ${
                  devices.livingRoomLight ? 'bg-green-500' : 'bg-gray-300'
                }`}
              >
                <span
                  className={`inline-block h-4 w-4 transform rounded-full bg-white transition-transform ${
                    devices.livingRoomLight ? 'translate-x-6' : 'translate-x-1'
                  }`}
                />
              </button>
            </div>
          </div>

          {/* Garage Door */}
          <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-200">
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-3">
                <div className={`p-2 rounded-lg ${devices.garageDoor ? 'bg-blue-100' : 'bg-gray-100'}`}>
                  <DoorClosed className={devices.garageDoor ? 'text-blue-500' : 'text-gray-400'} size={24} />
                </div>
                <div>
                  <div className="font-medium text-gray-900">Garage Door</div>
                  <div className="text-sm text-gray-500">{devices.garageDoor ? 'Open' : 'Closed'}</div>
                </div>
              </div>
              <button
                onClick={() => toggleDevice('garageDoor', 3)}
                className={`relative inline-flex h-6 w-11 items-center rounded-full transition-colors ${
                  devices.garageDoor ? 'bg-green-500' : 'bg-gray-300'
                }`}
              >
                <span
                  className={`inline-block h-4 w-4 transform rounded-full bg-white transition-transform ${
                    devices.garageDoor ? 'translate-x-6' : 'translate-x-1'
                  }`}
                />
              </button>
            </div>
          </div>

          {/* Bedroom Fan */}
          <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-200">
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-3">
                <div className={`p-2 rounded-lg ${devices.bedroomFan ? 'bg-cyan-100' : 'bg-gray-100'}`}>
                  <Fan className={devices.bedroomFan ? 'text-cyan-500' : 'text-gray-400'} size={24} />
                </div>
                <div>
                  <div className="font-medium text-gray-900">Bedroom Fan</div>
                  <div className="text-sm text-gray-500">{devices.bedroomFan ? 'On' : 'Off'}</div>
                </div>
              </div>
              <button
                onClick={() => toggleDevice('bedroomFan', 2)}
                className={`relative inline-flex h-6 w-11 items-center rounded-full transition-colors ${
                  devices.bedroomFan ? 'bg-green-500' : 'bg-gray-300'
                }`}
              >
                <span
                  className={`inline-block h-4 w-4 transform rounded-full bg-white transition-transform ${
                    devices.bedroomFan ? 'translate-x-6' : 'translate-x-1'
                  }`}
                />
              </button>
            </div>
          </div>

          {/* Living Room TV */}
          <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-200">
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-3">
                <div className={`p-2 rounded-lg ${devices.livingRoomTV ? 'bg-purple-100' : 'bg-gray-100'}`}>
                  <Tv className={devices.livingRoomTV ? 'text-purple-500' : 'text-gray-400'} size={24} />
                </div>
                <div>
                  <div className="font-medium text-gray-900">Living Room TV</div>
                  <div className="text-sm text-gray-500">{devices.livingRoomTV ? 'On' : 'Off'}</div>
                </div>
              </div>
              <button
                onClick={() => toggleDevice('livingRoomTV', 4)}
                className={`relative inline-flex h-6 w-11 items-center rounded-full transition-colors ${
                  devices.livingRoomTV ? 'bg-green-500' : 'bg-gray-300'
                }`}
              >
                <span
                  className={`inline-block h-4 w-4 transform rounded-full bg-white transition-transform ${
                    devices.livingRoomTV ? 'translate-x-6' : 'translate-x-1'
                  }`}
                />
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
