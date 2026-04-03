var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', () => {
    initWebSocket();
    setupForm();
});

function initWebSocket() {
    websocket = new WebSocket(gateway);
    websocket.onopen = () => console.log('Kết nối thành công');
    websocket.onclose = () => setTimeout(initWebSocket, 2000);
}

function setupForm() {
    document.getElementById("settingsForm").addEventListener("submit", function (e) {
        e.preventDefault();

        const data = JSON.stringify({
            page: "setting",
            value: {
                ssid: document.getElementById("ssid").value,
                password: document.getElementById("password").value,
            }
        });

        if (websocket.readyState === WebSocket.OPEN) {
            websocket.send(data);
            console.log("📤 Gửi:", data);
            alert("✅ Thông tin đã được gửi! ESP32 đang khởi động lại để kết nối...");
        } else {
            alert("⚠️ Lỗi: Chưa kết nối được với ESP32!");
        }
    });
}