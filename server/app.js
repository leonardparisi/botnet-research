import { createServer } from 'net';
import { unencryptCMD } from './encryption.js';

const server = createServer((socket) => {
    socket.on('data', (data) => {
        const decryptedData = unencryptCMD(data.toString());
        console.log('Received:', decryptedData);
        socket.write('Server reply: ' + decryptedData);
    });

    socket.on('end', () => {
        console.log('Client disconnected');
    });

    socket.on('error', (err) => {
        console.error('Socket error:', err);
    });
});

server.listen(9000, () => {
    console.log('Server listening on port 9000');
});

server.on('error', (err) => {
    console.error('Server error:', err);
});

