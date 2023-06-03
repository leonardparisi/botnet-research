import { createServer } from 'net';
import { unencryptCMD } from './encryption.js';
import { api } from "./api.js"
import { bots, socket_map, requests } from "./db.js"
import { config } from 'dotenv'
config()

const api_port = process.env.API_PORT || 3000
const socket_port = process.env.SOCKET_PORT || 9000

const socketServer = createServer((socket) => {

    socket.on('data', (rawData) => {
        const decryptedData = unencryptCMD(rawData.toString()).trim();
        try {
            const data = JSON.parse(decryptedData);
            if (data['Type'] == "Connection") {
                data['Status'] = "Connected"
                data['Socket'] = socket;
                bots[data['ID']] = data;
                socket_map[socket] = data['ID'];
                console.log('Client connected: ' + data);
            } else if (data['Type'] == "Output") {
                requests[data["RequestID"]].push(data)
                console.log(requests)
            }
        } catch (SyntaxError) {
            console.log("Failed to parse JSON data: " + decryptedData);
        }
    });


    socket.on('end', () => {
        console.log('Client disconnected');
        bots[socket_map[socket]]['status'] = "Disconnected"
    });

    socket.on('error', (err) => {
        console.error('Socket error:', err);
        bots[socket_map[socket]]['status'] = "Disconnected"
    });
});

socketServer.listen(socket_port, () => {
    console.log('Socket server listening on port 9000');
});

api.socketServer = socketServer;
api.listen(process.env.API_PORT, () => {
    console.log(`API server listening on port ${api_port}`);
});