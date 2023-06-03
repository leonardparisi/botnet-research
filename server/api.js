import express from 'express';
import { bots, requests } from "./db.js"
import { encryptCMD } from './encryption.js';

const TIMEOUT = 60000;

export const api = express();
api.use(express.json());

let lastRequestId = 0

api.get('/bots', (req, res) => {
    res.send(Object.values(bots).map(({ socket, ...bot })=>bot));
});

api.post('/bots/:id', async (req, res) => {
    const id = req.params.id;
    const commands = req.body.commands;
    const commandRequest = req.body;
    lastRequestId += 1
    commandRequest['request_id'] = lastRequestId.toString();
    const bot = bots[id]
    requests[lastRequestId] = []
    if (bot === undefined) {
        res.send(404)
    }
    bot['Socket'].write(encryptCMD(JSON.stringify(commandRequest)));

    const results = await new Promise((resolve, reject) => {
        let timeout = setTimeout(() => {
            clearTimeout(timeout);
            resolve("Timed Out");
        }, TIMEOUT);

        const checkArrayLength = () => {
            if (requests[lastRequestId].length === commands.length) {
                clearTimeout(timeout);
                resolve(requests[lastRequestId]);
            } else {
                setTimeout(checkArrayLength, 100);
            }
        };

        checkArrayLength();
    })

    res.send({
        "bot": bot['Status'],
        "commands": commands,
        "request_id": lastRequestId,
        "results": results
    });
});