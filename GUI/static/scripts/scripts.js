document.addEventListener('DOMContentLoaded', function() {
    // GUI is a client in terms of communicating with the c++ server
    let server_ip = '192.168.0.100';
    let server_port = '12345';
    const socket = new WebSocket('ws://' + server_ip + ':' + server_port + '/');

    socket.onopen = function() {
        console.log('Connected to the server');
    };

    socket.onerror = function(error) {
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


        if(socket.readyState === WebSocket.OPEN) {
            //socket.send(buffer);
            socket.send(command);
        }
    }

    let pressed = ("ontouchstart" in document.documentElement) ? 'touchstart' : 'mousedown';
    let released = ("ontouchstart" in document.documentElement) ? 'touchend' : 'mouseup';

        
    document.getElementById('btnForward').addEventListener(pressed, function(e) {
        e.preventDefault();
        sendCommand('move_forward');
    });
    document.getElementById('btnForward').addEventListener(released, function() {
        sendCommand('stop');
    });

    document.getElementById('btnBackward').addEventListener(pressed, function(e) {
        e.preventDefault();
        sendCommand('move_backward');
    });
    document.getElementById('btnBackward').addEventListener(released, function() {
        sendCommand('stop');
    });

    document.getElementById('btnLeft').addEventListener(pressed, function(e) {
        e.preventDefault();
        sendCommand('turn_left');
    });
    document.getElementById('btnLeft').addEventListener(released, function() {
        sendCommand('stop');
    });

    document.getElementById('btnRight').addEventListener(pressed, function(e) {
        e.preventDefault();
        sendCommand('turn_right');
    });
    document.getElementById('btnRight').addEventListener(released, function() {
        sendCommand('stop');
    });

    document.getElementById('btnALeft').addEventListener(pressed, function(e) {
        e.preventDefault();
        sendCommand('turn_around_left');
    });
    document.getElementById('btnALeft').addEventListener(released, function() {
        sendCommand('stop');
    });

    document.getElementById('btnARight').addEventListener(pressed, function(e) {
        e.preventDefault();
        sendCommand('turn_around_right');
    });
    document.getElementById('btnARight').addEventListener(released, function() {
        sendCommand('stop');
    });

    document.getElementById('btnStop').addEventListener(pressed, function(e) {
        e.preventDefault();
        sendCommand('stop');
    });

    
    let Joy1 = new JoyStick('joystick-left', {}, function(stickData) {
        sendCommand('joystick_rover_' + Math.floor(stickData.x) + '_' + Math.floor(stickData.y)
    });
    let Joy2 = new JoyStick('joystick-right', {}, function(stickData) {
        sendCommand('joystick_camera_' + Math.floor(stickData.x) + '_' + Math.floor(stickData.y)
    });
});
