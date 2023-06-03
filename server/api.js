import express from 'express';
export const api = express();
import {bots} from "./db.js"

api.get('/bots', (req, res) => {
    res.send(Object.values(bots));
});

