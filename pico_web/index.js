const express = require('express');
const mqtt = require('mqtt');
const path = require('path');
const util = require('util');
const fs = require('fs');
const bodyParser = require('body-parser');
const e = require('express');
const {response} = require('express');
const app = express();

const readFile = util.promisify(fs.readFile);
const writeFile = util.promisify(fs.writeFile);

// Create variables for MQTT use here
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended : true}));
function read(filePath = './message.json') {
    return readFile(path.resolve(__dirname, filePath)).then(data => JSON.parse(data));
}
function write(data, filePath = './message.json') {
    return writeFile(path.resolve(__dirname, filePath), JSON.stringify(data));
}


// create an MQTT instance
const broker = "mqtt://192.168.50.111";
const client = mqtt.connect(broker);

// Check that you are connected to MQTT and subscribe to a topic (connect event)
client.on("connect", () =>{
    console.log("Connected");
    client.subscribe('Quinn/messages', err =>{
        if(!err){
            console.log("Subscribe to topic: Quinn/messages");
        }
    });
});

// handle instance where MQTT will not connect (error event)
client.on("error", err =>{
    console.error("Connection error:", err);
});

// Handle when a subscribed message comes in (message event)
client.on("message", async (topic, message) =>{
    console.log("New message: ", message.toString());

    try {
        const messages = await read();
        const msgObject = {
            id: Date.now(),
            msg: message.toString()
        };
        messages.push(msgObject);
        await write(messages);
        console.log("Messages saved");
    } catch (e){
        console.error("Error writing message: ", e);
    }
});

// Route to serve the home page
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname,"Overview.html"))
});

// route to serve the JSON array from the file message.json when requested from the home page
app.get('/messages', async (req,res) =>{
    const data = await read();
    res.json(data);
});

// Route to serve the page to add a message
app.get('/add', (req,res) =>{
res.sendFile(path.join(__dirname,"message.html"));
});

//Route to show a selected message. Note, it will only show the message as text. No html needed
app.get('/:id', async (req,res) => {
    const data = await read();
    const msg = data.find(m => m.id == req.params.id);
    if(!msg) return res.sendStatus(404);
    res.json(msg);
});

// Route to CREATE a new message on the server and publish to mqtt broker
app.post('/', async (req, res) =>{
    const topic = req.body.topic;
    const msg = req.body.msg;
    if (!topic || !msg) return res.sendStatus(404);

    client.publish(topic, msg);

    if (topic !== 'Quinn/messages') {
        const messages = await read();
        messages.push({
            id: Date.now(),
            msg: msg
        });
        await write(messages);
    }
    res.sendStatus(200);
});
// Route to delete a message by id (Already done for you)

app.delete('/:id', async (req, res) => {
    try {
        const messages = await read();
        await write(messages.filter(c => c.id !== req.params.id));
        res.sendStatus(200);
    } catch (e) {
        res.sendStatus(200);
    }
});

// listen to the port
app.listen(3000);