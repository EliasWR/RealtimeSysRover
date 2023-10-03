document.addEventListener('DOMContentLoaded', function() {
    const socket = new WebSocket('ws://localhost:12345');

    socket.onopen = function() {
        console.log('Connected to the server');
    };

    socket.onerror = function(error) {
        console.log(`WebSocket Error: ${error}`);
    };

    function sendCommand(command) {
        if(socket.readyState === WebSocket.OPEN) {
            socket.send(command);
        }
    }

    document.getElementById('btnForward').addEventListener('click', function() {
        sendCommand('move_forward');
    });

    document.getElementById('btnBackward').addEventListener('click', function() {
        sendCommand('move_backward');
    });

    document.getElementById('btnLeft').addEventListener('click', function() {
        sendCommand('turn_left');
    });

    document.getElementById('btnRight').addEventListener('click', function() {
        sendCommand('turn_right');
    });

    document.getElementById('xSlider').addEventListener('input', function(event) {
        sendCommand(`x_axis_${event.target.value}`);
    });

    document.getElementById('ySlider').addEventListener('input', function(event) {
        sendCommand(`y_axis_${event.target.value}`);
    });
});

document.addEventListener("DOMContentLoaded", function() {
    const joystickOptions = {
        zone: document.getElementById('joystickContainer'),
        size: 150,
        position: {left: '50%', top: '50%'}
    };

    const manager = nipplejs.create(joystickOptions);

    manager.on('move', function(evt, data) {
        // Get the x and y values of the joystick. They range from -1 to 1.
        const x = data.distance * Math.cos(data.angle.radian) / joystickOptions.size;
        const y = data.distance * Math.sin(data.angle.radian) / joystickOptions.size;

        // Now you can send x and y values via WebSocket to your robot controller
        const command = {
            type: "joystick",
            x: x,
            y: y
        };
        websocket.send(JSON.stringify(command));
    });
});