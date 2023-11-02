document.addEventListener('DOMContentLoaded', function () {
    // GUI is a client in terms of communicating with the c++ server
    // let server_ip = '192.168.0.100';
    let server_port = '12345';
    console.log('Connecting to the server at ' + server_ip + ':' + server_port + '...');
    const socket = new WebSocket('ws://' + server_ip + ':' + server_port + '/');


    socket.onopen = function () {
        console.log('Connected to the server');
    };

    socket.onerror = function (error) {
        console.log(`WebSocket Error: ${error}`);
    };

    function sendCommand(command) {
        console.log(command);
        /*
        const message = YourGeneratedMessageClass.create({
            // ... your data
        });

        const buffer = YourGeneratedMessageClass.encode(message).finish();

        // To decode
        const decodedMessage = YourGeneratedMessageClass.decode(buffer);

        */

        if (socket.readyState === WebSocket.OPEN) {
            //socket.send(buffer);
            

            // Convert the message string to a blob if it's not already in binary format
            var messageBlob = new Blob([command]);

            // Wait until the blob is fully converted to a binary buffer
            var reader = new FileReader();
            reader.onloadend = function() {
                // Get the ArrayBuffer from the reader
                var arrayBuffer = reader.result;
                var messageLength = arrayBuffer.byteLength;

                // Create a new ArrayBuffer for 4 bytes
                var lengthBuffer = new ArrayBuffer(4);

                // Create a DataView to interact with the buffer
                var view = new DataView(lengthBuffer);

                // Write the length of your message to the buffer
                // Assuming the system is little-endian you could use view.setUint32(0, messageLength, true);
                // If you know the server is expecting big-endian, use false
                view.setUint32(0, messageLength, true);

                // Send the length buffer
                socket.send(lengthBuffer);

                // Now send the actual message
                socket.send(arrayBuffer);
            };

            // Start reading the blob as binary
            reader.readAsArrayBuffer(messageBlob);

        }
    }

    let pressed = ("ontouchstart" in document.documentElement) ? 'touchstart' : 'mousedown';
    let released = ("ontouchstart" in document.documentElement) ? 'touchend' : 'mouseup';


    document.getElementById('btnForward').addEventListener(pressed, function (e) {
        e.preventDefault();
        sendCommand('move_forward');
    });
    document.getElementById('btnForward').addEventListener(released, function () {
        sendCommand('stop');
    });

    document.getElementById('btnBackward').addEventListener(pressed, function (e) {
        e.preventDefault();
        sendCommand('move_backward');
    });
    document.getElementById('btnBackward').addEventListener(released, function () {
        sendCommand('stop');
    });

    document.getElementById('btnLeft').addEventListener(pressed, function (e) {
        e.preventDefault();
        sendCommand('turn_left');
    });
    document.getElementById('btnLeft').addEventListener(released, function () {
        sendCommand('stop');
    });

    document.getElementById('btnRight').addEventListener(pressed, function (e) {
        e.preventDefault();
        sendCommand('turn_right');
    });
    document.getElementById('btnRight').addEventListener(released, function () {
        sendCommand('stop');
    });

    document.getElementById('btnALeft').addEventListener(pressed, function (e) {
        e.preventDefault();
        sendCommand('turn_around_left');
    });
    document.getElementById('btnALeft').addEventListener(released, function () {
        sendCommand('stop');
    });

    document.getElementById('btnARight').addEventListener(pressed, function (e) {
        e.preventDefault();
        sendCommand('turn_around_right');
    });
    document.getElementById('btnARight').addEventListener(released, function () {
        sendCommand('stop');
    });

    document.getElementById('btnStop').addEventListener(pressed, function (e) {
        e.preventDefault();
        sendCommand('stop');
    });


    let Joy1 = new JoyStick('joystick-left', {}, function (stickData) {
        sendCommand('joystick_rover_' + Math.floor(stickData.x) + '_' + Math.floor(stickData.y));
    });
    let Joy2 = new JoyStick('joystick-right', {}, function (stickData) {
        sendCommand('joystick_camera_' + Math.floor(stickData.x) + '_' + Math.floor(stickData.y));
    });
});
