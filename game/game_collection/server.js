const WebSocket = require('ws');
const wss = new WebSocket.Server({ port: 9999 });

const clients = new Set();
const CHAT_PASSWORD = '9527'; // 修改为新密码

wss.on('connection', (ws) => {
    clients.add(ws);
    console.log('New client connected, total clients:', clients.size);

    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message.toString());
            console.log('Received message:', data);
            
            if (!data.type || !data.username) {
                console.log('Invalid message format:', data);
                return;
            }

            // 广播消息给所有客户端
            const broadcastMessage = data.type === 'join' ? 
                {
                    username: 'System',
                    message: `${data.username} 加入了聊天室`
                } : 
                {
                    username: data.username,
                    message: data.message
                };

            console.log('Broadcasting:', broadcastMessage);
            
            clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify(broadcastMessage));
                }
            });
        } catch (error) {
            console.error('Error processing message:', error);
        }
    });

    ws.on('close', () => {
        clients.delete(ws);
        console.log('Client disconnected, remaining clients:', clients.size);
    });

    ws.on('error', (error) => {
        console.error('WebSocket error:', error);
        clients.delete(ws);
    });
});

// 定期清理断开的连接
setInterval(() => {
    const initialSize = clients.size;
    clients.forEach((client) => {
        if (client.readyState === WebSocket.CLOSED) {
            clients.delete(client);
        }
    });
    if (initialSize !== clients.size) {
        console.log('Cleaned up disconnected clients, remaining:', clients.size);
    }
}, 30000);

console.log('WebSocket server is running on port 9999'); 