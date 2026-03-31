import { Lightbulb, DoorClosed, Fan, Tv, Lock, Thermometer, Shield } from "lucide-react";
import { useState, useEffect } from "react";
import axios from "axios";

interface HistoryEntry {
  id: number;
  device: string;
  action: string;
  time: string;
  icon: any;
}

export function History() {
  const [logs, setLogs] = useState<HistoryEntry[]>([]);
  const [fromDate, setFromDate] = useState('');
  const [toDate, setToDate] = useState('');

  useEffect(() => {
    fetchLogs();
  }, [fromDate, toDate]);

  const fetchLogs = async () => {
    try {
      const params = {};
      if (fromDate) params.from = fromDate;
      if (toDate) params.to = toDate;
      const response = await axios.get('http://localhost:3000/api/logs', { params });
      setLogs(response.data);
    } catch (error) {
      console.error('Error fetching logs:', error);
    }
  };

  const getIcon = (device: string) => {
    switch (device.toLowerCase()) {
      case 'light': return Lightbulb;
      case 'fan': return Fan;
      case 'door': return DoorClosed;
      case 'tv': return Tv;
      default: return Shield;
    }
  };

  return (
    <div className="space-y-6">
      <h2 className="text-3xl font-bold text-gray-900">History</h2>

      {/* Date Filters */}
      <div className="flex gap-4 mb-4">
        <input
          type="date"
          value={fromDate}
          onChange={(e) => setFromDate(e.target.value)}
          className="px-3 py-2 border border-gray-300 rounded-lg"
          placeholder="From Date"
        />
        <input
          type="date"
          value={toDate}
          onChange={(e) => setToDate(e.target.value)}
          className="px-3 py-2 border border-gray-300 rounded-lg"
          placeholder="To Date"
        />
      </div>

      {/* History Table */}
      <div className="bg-white rounded-xl shadow-sm border border-gray-200 overflow-hidden">
        <table className="w-full">
          <thead className="bg-gray-50 border-b border-gray-200">
            <tr>
              <th className="px-6 py-4 text-left text-sm font-semibold text-gray-900">Time</th>
              <th className="px-6 py-4 text-left text-sm font-semibold text-gray-900">Device</th>
              <th className="px-6 py-4 text-left text-sm font-semibold text-gray-900">Action</th>
            </tr>
          </thead>
          <tbody className="divide-y divide-gray-200">
            {logs.map((entry, index) => {
              const Icon = getIcon(entry.device);

              return (
                <tr
                  key={entry.id}
                  className={index % 2 === 0 ? 'bg-white' : 'bg-gray-50'}
                >
                  <td className="px-6 py-4 text-sm text-gray-600 font-mono">{new Date(entry.time).toLocaleString()}</td>
                  <td className="px-6 py-4">
                    <div className="flex items-center gap-3">
                      <div className="p-2 bg-blue-50 rounded-lg">
                        <Icon className="text-blue-600" size={18} />
                      </div>
                      <span className="text-sm font-medium text-gray-900">{entry.device}</span>
                    </div>
                  </td>
                  <td className="px-6 py-4 text-sm text-gray-600">{entry.action}</td>
                </tr>
              );
            })}
          </tbody>
        </table>
      </div>

      {/* Summary Stats */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
        <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-200">
          <div className="text-2xl font-bold text-gray-900">42</div>
          <div className="text-sm text-gray-600 mt-1">Events Today</div>
        </div>
        <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-200">
          <div className="text-2xl font-bold text-gray-900">8</div>
          <div className="text-sm text-gray-600 mt-1">Active Devices</div>
        </div>
        <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-200">
          <div className="text-2xl font-bold text-gray-900">3.2 kWh</div>
          <div className="text-sm text-gray-600 mt-1">Energy Used Today</div>
        </div>
      </div>
    </div>
  );
}
