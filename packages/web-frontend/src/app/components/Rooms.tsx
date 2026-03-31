import { Sofa, ChefHat, Bed, Bath, Thermometer, Droplet } from "lucide-react";
import { useState } from "react";

interface RoomData {
  id: string;
  name: string;
  icon: any;
  temperature: number;
  humidity: number;
  brightness: number;
  fanSpeed: number;
  masterSwitch: boolean;
}

export function Rooms() {
  const [rooms, setRooms] = useState<RoomData[]>([
    {
      id: "living-room",
      name: "Living Room",
      icon: Sofa,
      temperature: 24,
      humidity: 65,
      brightness: 75,
      fanSpeed: 2,
      masterSwitch: true,
    },
    {
      id: "kitchen",
      name: "Kitchen",
      icon: ChefHat,
      temperature: 26,
      humidity: 70,
      brightness: 100,
      fanSpeed: 1,
      masterSwitch: true,
    },
    {
      id: "bedroom",
      name: "Bedroom",
      icon: Bed,
      temperature: 22,
      humidity: 60,
      brightness: 40,
      fanSpeed: 3,
      masterSwitch: true,
    },
    {
      id: "bathroom",
      name: "Bathroom",
      icon: Bath,
      temperature: 23,
      humidity: 75,
      brightness: 60,
      fanSpeed: 0,
      masterSwitch: false,
    },
  ]);

  const updateRoom = (id: string, updates: Partial<RoomData>) => {
    setRooms(prev => prev.map(room =>
      room.id === id ? { ...room, ...updates } : room
    ));
  };

  return (
    <div className="space-y-6">
      <h2 className="text-3xl font-bold text-gray-900">Rooms</h2>

      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {rooms.map((room) => {
          const Icon = room.icon;

          return (
            <div key={room.id} className="bg-white rounded-xl shadow-sm border border-gray-200 overflow-hidden">
              {/* Header */}
              <div className="bg-gradient-to-r from-blue-50 to-indigo-50 p-6 border-b border-gray-200">
                <div className="flex items-center justify-between">
                  <div className="flex items-center gap-4">
                    <div className="p-3 bg-white rounded-lg shadow-sm">
                      <Icon className="text-blue-600" size={28} />
                    </div>
                    <div>
                      <h3 className="text-xl font-semibold text-gray-900">{room.name}</h3>
                      <div className="flex items-center gap-4 mt-1 text-sm text-gray-600">
                        <div className="flex items-center gap-1">
                          <Thermometer size={16} />
                          <span>{room.temperature}°C</span>
                        </div>
                        <div className="flex items-center gap-1">
                          <Droplet size={16} />
                          <span>{room.humidity}%</span>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>

              {/* Body - Controls */}
              <div className="p-6 space-y-6">
                {/* Brightness Control */}
                <div>
                  <div className="flex items-center justify-between mb-2">
                    <label className="text-sm font-medium text-gray-700">Brightness</label>
                    <span className="text-sm text-gray-500">{room.brightness}%</span>
                  </div>
                  <input
                    type="range"
                    min="0"
                    max="100"
                    value={room.brightness}
                    onChange={(e) => updateRoom(room.id, { brightness: parseInt(e.target.value) })}
                    className="w-full h-2 bg-gray-200 rounded-lg appearance-none cursor-pointer accent-blue-600"
                  />
                </div>

                {/* Fan Speed Control */}
                <div>
                  <div className="flex items-center justify-between mb-2">
                    <label className="text-sm font-medium text-gray-700">Fan Speed</label>
                    <span className="text-sm text-gray-500">
                      {room.fanSpeed === 0 ? 'Off' : `Level ${room.fanSpeed}`}
                    </span>
                  </div>
                  <div className="flex gap-2">
                    {[0, 1, 2, 3].map((speed) => (
                      <button
                        key={speed}
                        onClick={() => updateRoom(room.id, { fanSpeed: speed })}
                        className={`flex-1 py-2 px-4 rounded-lg border-2 transition-colors ${
                          room.fanSpeed === speed
                            ? 'border-blue-500 bg-blue-50 text-blue-700'
                            : 'border-gray-200 bg-white text-gray-700 hover:bg-gray-50'
                        }`}
                      >
                        {speed === 0 ? 'Off' : speed}
                      </button>
                    ))}
                  </div>
                </div>
              </div>

              {/* Footer - Master Switch */}
              <div className="bg-gray-50 px-6 py-4 border-t border-gray-200">
                <div className="flex items-center justify-between">
                  <div>
                    <div className="font-medium text-gray-900">Master Control</div>
                    <div className="text-sm text-gray-500">
                      {room.masterSwitch ? 'All devices active' : 'All devices off'}
                    </div>
                  </div>
                  <button
                    onClick={() => updateRoom(room.id, { masterSwitch: !room.masterSwitch })}
                    className={`relative inline-flex h-7 w-12 items-center rounded-full transition-colors ${
                      room.masterSwitch ? 'bg-green-500' : 'bg-gray-300'
                    }`}
                  >
                    <span
                      className={`inline-block h-5 w-5 transform rounded-full bg-white transition-transform ${
                        room.masterSwitch ? 'translate-x-6' : 'translate-x-1'
                      }`}
                    />
                  </button>
                </div>
              </div>
            </div>
          );
        })}
      </div>
    </div>
  );
}
